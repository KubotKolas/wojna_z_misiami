#include "defines.hpp"
#include "includes.hpp"
#include "thread_comm.hpp"
#include "thread_main.hpp"

int s2 = 0;

void *startKomWatek(void *ptr) {
  MPI_Status status;
  int is_message = false;
  packet pakiet;
  int tid;
  MPI_Comm_rank(MPI_COMM_WORLD, &tid);
  /* Obrazuje pętlę odbierającą pakiety o różnych typach */
  while (stan != FINISHED) {
    // debug("czekam na recv");
    MPI_Recv(&pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG,
             MPI_COMM_WORLD, &status);
    handleMess(&pakiet, &status);
    // MPI_Bcast(&pakiet, 1, MPI_PAKIET_T, rank, MPI_COMM_WORLD);
    // handleMess(&pakiet, &status);
  }
  return nullptr;
}

void requestMech(int n) {
  debug("Requesting %d mechs", n);
  packet p;
  timer += 1;
  mech_priority = timer;
  p.ts = timer;
  p.tag = REQ_MECH;
  p.data = n;
  p.src = rank;
  for (int i = 0; i < proc_number; i++) {
    if (i != rank) {
      MPI_Send(&p, 1, MPI_PAKIET_T, i, REQ_MECH, MPI_COMM_WORLD);
    }
  }
}

void requestDock() {
  debug("Requesting a dock");
  packet p;
  timer += 1;
  dock_priority = timer;
  p.ts = timer;
  p.tag = REQ_DOCK;
  p.data = 1;
  p.src = rank;
  for (int i = 0; i < proc_number; i++) {
    if (i != rank) {
      MPI_Send(&p, 1, MPI_PAKIET_T, i, REQ_DOCK, MPI_COMM_WORLD);
    }
  }
}

void sendMech(int dest, int n) {
  packet p;
  timer += 1;
  p.ts = timer;
  p.tag = T_MECH;
  p.src = rank;
  p.data = n;
  MPI_Send(&p, 1, MPI_PAKIET_T, dest, T_MECH, MPI_COMM_WORLD);
}

void sendDock(int dest, int n) {
  packet p;
  timer += 1;
  p.ts = timer;
  p.tag = T_DOCK;
  p.src = rank;
  p.data = n;
  MPI_Send(&p, 1, MPI_PAKIET_T, dest, T_DOCK, MPI_COMM_WORLD);
}

void handleMess(packet *pakiet, MPI_Status *status) {
  // debug("Received mess")
  timer = std::max(pakiet->ts, timer) + 1;

  switch (pakiet->tag) {
  case REQ_DOCK:
    // TODO: test
    // debug("Recieved dock req")
    {
      std::lock_guard<std::mutex> g(dock_mtx);
      dock_requests.push({pakiet->ts, pakiet->src});
    }
    switch (stan) {
    case IDLE:
    case AWAIT_MECH:
      checkDockQueue(0);
    break;
    case AWAIT_DOCK:
    case REPAIR:
      if (dock_counter > 1){
          checkDockQueue(dock_counter - 1);
      }
      break;
    default:
      break;
    }
    break;
  case REQ_MECH:
    // TODO: test
    // debug("Recieved mech req")
    {
      std::lock_guard<std::mutex> g(mech_mtx);
      mech_requests.push({pakiet->ts, pakiet->src});
    }
    switch (stan) {
    case IDLE:
    case AWAIT_MECH:
    checkMechQueue(0);
      break;
    case AWAIT_DOCK:
    case REPAIR:
      s2 = mech_counter - dmg;
      if (s2 > 0){
          checkMechQueue(s2);
          s2 = 0;
      }
      break;
    default:
      break;
    }
    break;
  case T_DOCK:
    // TODO: test
    {
      std::lock_guard<std::mutex> g(dock_mtx);
      dock_counter += pakiet->data;
    }
    switch (stan) {
    case IDLE:
    case AWAIT_MECH:
    checkDockQueue(0);
    break;
    case AWAIT_DOCK:
    case REPAIR:
      if (dock_counter > 1){
          checkDockQueue(dock_counter - 1);
      }
      break;
    default:
      break;
    }
    break;
  case T_MECH:
    // TODO: test
    {
      std::lock_guard<std::mutex> g(mech_mtx);
      mech_counter += pakiet->data;
    }
    switch (stan) {
    case IDLE:
    case AWAIT_MECH:
      checkMechQueue(0);
      break;
    case AWAIT_DOCK:
    case REPAIR:
      s2 = mech_counter - dmg;
      if (s2 > 0){
          checkMechQueue(s2);
          s2 = 0;
      }
      break;
    default:
      break;
    }
    break;
  default:
    // Unknown message type; throw an error
    // TODO: to implement
    debug("[!] [id:%d] Malformed request from %d!", rank, status->MPI_SOURCE)

        break;
  }
}

void checkDockQueue(int surplus) {
  if (surplus){
    {
      std::lock_guard<std::mutex> g(dock_mtx);
      while (dock_counter > 1 && !dock_requests.empty()) {
        int dest = dock_requests.top().second;
        sendDock(dest, 1);
        dock_counter -= 1;
      }
      return;
    }
  }
  else
  {  if (stan == REPAIR) {return;}
    {
      std::lock_guard<std::mutex> g(dock_mtx);
      if (dock_counter > 0 && !dock_requests.empty()) {
        if (dock_requests.top().first < dock_priority) {
          int dest = dock_requests.top().second;
          sendDock(dest, dock_counter);
          dock_requests.pop();
          dock_counter = 0;
          return;
        }
        if (dock_requests.top().first == dock_priority &&
            dock_requests.top().second < rank) {
          int dest = dock_requests.top().second;
          sendDock(dest, dock_counter);
          dock_requests.pop();
          dock_counter = 0;
          return;
        }
      }
    }
  }
}

void checkMechQueue(int surplus) {
  if (surplus){
    {
      std::lock_guard<std::mutex> g(mech_mtx);
      if (!mech_requests.empty()) {
        int dest = mech_requests.top().second;
        sendMech(dest, surplus);
        mech_counter -= surplus;
        return;
      }
    }
  }
  else
  { if (stan == REPAIR) {return;}
    {
      std::lock_guard<std::mutex> g(mech_mtx);
      if (mech_counter > 0 && !mech_requests.empty()) {
        if (mech_requests.top().first < mech_priority) {
          int dest = mech_requests.top().second;
          sendMech(dest, mech_counter);
          mech_requests.pop();
          mech_counter = 0;
          return;
        }
        if (mech_requests.top().first == mech_priority &&
            mech_requests.top().second < rank) {
          int dest = mech_requests.top().second;
          sendMech(dest, mech_counter);
          mech_requests.pop();
          mech_counter = 0;
          return;
        }
      }
    }
  }
}
#include "includes.hpp"
#include "defines.hpp"
#include "thread_main.hpp"
#include "thread_comm.hpp"


// int tid;
int timer = 0;
int dock_priority = MAX_INT;
int mech_priority = MAX_INT;
int dmg = 0;
int repair_progress = 0;
int waiting = 0;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> d_sleep(1, 5);

void mainLoop(int docks, int mechs, int proc_number){
    // MPI_Comm_rank(MPI_COMM_WORLD, &tid);

    while (stan != FINISHED) {
        debug("current state - %d", stan)
        switch (stan) {
            case INIT:
            // TODO: to implement
                dock_counter = docks / proc_number;
                if (rank < (docks % proc_number)){
                    dock_counter += 1;
                }
                mech_counter = mechs / proc_number;
                if (rank < (mechs % proc_number)){
                    mech_counter += 1;
                }
                debug("Docks: %d, Mechs:%d, T_DOCK: %d, T_MECH: %d", docks, mechs, dock_counter, mech_counter)
                stan = IDLE;
                break;
            case IDLE:
            // TODO: to implement
                if (dmg == 0){
                    dmg = rollDmg(mechs);
                    sleep(d_sleep(gen));
                    checkMechQueue();
                    checkDockQueue();
                }
                else {
                    stan = AWAIT_MECH;
                }
                debug("Dmg: %d", dmg)
                break;
            case AWAIT_MECH:
            // TODO: to implement
                if (mech_requests.empty()) {
                    debug("Mech counter: %d, waiting: %d, Mech_req_queue: %zu, my last priority: %d", mech_counter, waiting, mech_requests.size(), mech_priority)
                }
                else {
                    debug("Mech counter: %d, waiting: %d, Mech_req_queue: %zu, my last priority: %d, queue.top(): (%d, %d)", mech_counter, waiting, mech_requests.size(), mech_priority, mech_requests.top().first, mech_requests.top().second)
                }
                // debug("Dock counter: %d", dock_counter)
                debug("Dock counter: %d, waiting: %d, Dock_req_queue: %zu, my last priority: %d", dock_counter, waiting, dock_requests.size(), dock_priority)
                {
                    std::lock_guard<std::mutex> g(mech_mtx);
                    if (mech_counter < dmg){
                        if(!waiting){
                            requestMech(dmg);
                            waiting = 1;
                        }
                    }
                    else {
                        waiting = 0;
                        stan = AWAIT_DOCK;
                        mech_priority = MAX_INT;
                    }
                }
                checkMechQueue();
                checkDockQueue();
                break;
            case AWAIT_DOCK:
            // TODO: to implement
                debug("Dock counter: %d, waiting: %d, Dock_req_queue: %zu, my last priority: %d", dock_counter, waiting, dock_requests.size(), dock_priority)
                {
                    std::lock_guard<std::mutex> g(dock_mtx);
                    if (dock_counter == 0){
                        if(!waiting){
                            requestDock();
                            waiting = 1;
                        }
                    }
                    else {
                        waiting = 0;
                        stan = REPAIR;
                        dock_priority = MAX_INT;
                    }
                }
                checkDockQueue();
                break;
            case REPAIR:
            // TODO: to implement
                    repair_progress += repair();
                    if (repair_progress >= 100){
                        stan = IDLE;
                    }
                break;
            default:
            // Impossible state; raise error
            // TODO: to implement
                debug("[!] Enteres impossible state")
                break;
            
        }
        sleep(1);
    }
}



int rollDmg(int upper_limit){
    std::uniform_int_distribution<> d_dmg(1, upper_limit);
    return d_dmg(gen);
}

int repair(){
    std::uniform_int_distribution<> d_repair(1, 25);
    return d_repair(gen);
}


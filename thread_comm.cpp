#include "includes.hpp"
#include "defines.hpp"
#include <mpi.h>
#include "thread_comm.hpp"

void *startKomWatek(void *ptr)
{
    MPI_Status status;
    int is_message = false;
    packet pakiet;
    int tid;
    MPI_Comm_rank(MPI_COMM_WORLD, &tid);
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while ( stan!=FINISHED ) {
	debug("czekam na recv");
        MPI_Recv( &pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        timer = std::max(pakiet.ts, timer) + 1;

        switch ( pakiet.tag ) {
	    case REQ_DOCK: 
            // TODO: test
            dock_requests.push({pakiet.ts, pakiet.src});
            switch (stan) {
                case IDLE:
                case AWAIT_MECH:
                case AWAIT_DOCK:
                    if (dock_counter > 0 && !dock_requests.empty()){
                        int dest = dock_requests.top().second;
                        sendDock(dest, dock_counter);
                        dock_requests.pop();
                        dock_counter = 0;
                    }
                    break;
                default:
                    break;
            }
	        break;
	    case REQ_MECH: 
            // TODO: test
            mech_requests.push({pakiet.ts, pakiet.src});
            switch (stan) {
                case IDLE:
                case AWAIT_MECH:
                    if (mech_counter > 0 && !mech_requests.empty()){
                        int dest = mech_requests.top().second;
                        sendDock(dest, mech_counter);
                        mech_requests.pop();
                        mech_counter = 0;
                    }
                    break;
                default:
                    break;
            }
	        break;
        case T_DOCK:
            // TODO: test
            dock_counter += pakiet.data;
            break;
        case T_MECH:
            // TODO: test
            mech_counter += pakiet.data;
            break;
	    default:
            // Unknown message type; throw an error
            // TODO: to implement
            debug("[!] [id:%d] Malformed request from %d!", tid , status.MPI_SOURCE)

	        break;
        }
    }
    return nullptr;
}

void requestMech(int n){
    packet p;
    timer += 1;
    p.ts = timer;
    p.tag = REQ_MECH;
    p.data = n;
    p.src = rank;
    MPI_Bcast(&p, 1, MPI_PAKIET_T, rank, MPI_COMM_WORLD);
}

void requestDock(){
    
}

void sendMech(int dest, int n){

}

void sendDock(int dest, int n){

}
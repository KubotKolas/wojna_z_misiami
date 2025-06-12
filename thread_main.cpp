#include "includes.hpp"
#include "defines.hpp"
#include <unistd.h>

void mainLoop(){
    int tid;
    MPI_Comm_rank(MPI_COMM_WORLD, &tid);

    while (stan != FINISHED) {
        debug("[id:%d] current state - %d", tid, stan)
        switch (stan) {
            case INIT:
            // TODO: to implement
                break;
            case IDLE:
            // TODO: to implement
                break;
            case AWAIT_MECH:
            // TODO: to implement
                break;
            case AWAIT_DOCK:
            // TODO: to implement
                break;
            case REPAIR:
            // TODO: to implement
                break;
            default:
            // Impossible state; raise error
            // TODO: to implement
                debug("[!] [id:%d] Enteres impossible state!", tid)
                break;
            
        }
        sleep(1);
    }
}
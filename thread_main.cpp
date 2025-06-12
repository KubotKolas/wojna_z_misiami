#include "includes.hpp"
#include "defines.hpp"
#include "thread_main.hpp"
#include "thread_comm.hpp"


int tid;
int timer = 0;
int my_priority = MAX_INT;
int dmg = 0;
int repair_progress = 0;
int waiting = 0;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> d_sleep(1, 5);

void mainLoop(int docks, int mechs, int proc_number){
    MPI_Comm_rank(MPI_COMM_WORLD, &tid);

    while (stan != FINISHED) {
        debug("current state - %d", stan)
        switch (stan) {
            case INIT:
            // TODO: to implement
                dock_counter = docks / proc_number;
                if (tid <= docks % proc_number){
                    dock_counter += 1;
                }
                mech_counter = mechs / proc_number;
                if (tid <= mechs % proc_number){
                    mech_counter += 1;
                }
                break;
            case IDLE:
            // TODO: to implement
                if (dmg == 0){
                    dmg = rollDmg(mechs);
                    sleep(d_sleep(gen));
                }
                else {
                    stan = AWAIT_MECH;
                }
                break;
            case AWAIT_MECH:
            // TODO: to implement
                if (mech_counter < dmg){
                    if(!waiting){
                        requestMech(dmg-mech_counter);
                        waiting = 1;
                    }
                }
                else {
                    waiting = 0;
                    stan = AWAIT_DOCK;
                }
                break;
            case AWAIT_DOCK:
            // TODO: to implement
                if (dock_counter == 0){
                        if(!waiting){
                            requestDock();
                            waiting = 1;
                        }
                    }
                    else {
                        waiting = 0;
                        stan = REPAIR;
                    }
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
                debug("[!] [id:%d] Enteres impossible state!", tid)
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


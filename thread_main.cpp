#include "functions.hpp"

void mainLoop(int docks, int mechanics, int number_processes)
{
    while (state != FINISHED)
    {
        switch (state)
        {
        case INIT:
            dock_counter = docks / number_processes;
            if (p_num < (docks % number_processes))
                dock_counter += 1;
            mech_counter = mechanics / number_processes;
            if (p_num < (mechanics % number_processes))
                mech_counter += 1;
            state = IDLE;
            break;
        case IDLE:
            damage = rand() % max(1, mechanics / 2) + 1;
            sleep(rand() % 5 + 1);
            checkMechQueue(0);
            checkDockQueue(0);
            state = AWAIT_MECH;
            break;
        case AWAIT_MECH:
            checkMechQueue(0);
            checkDockQueue(0);
            {
                lock_guard<mutex> g(mech_mutex);
                if (mech_counter < damage)
                {
                    if (!waiting)
                    {
                        requestMech(damage);
                        waiting = true;
                    }
                }
                else
                {
                    waiting = false;
                    state = AWAIT_DOCK;
                    mech_priority = MAX_INT;
                }
            }
            break;
        case AWAIT_DOCK:
        {
            lock_guard<mutex> g(dock_mutex);
            if (dock_counter == 0)
            {
                if (!waiting)
                {
                    requestDock();
                    waiting = true;
                }
            }
            else
            {
                waiting = false;
                state = REPAIR;
                dock_priority = MAX_INT;
            }
        }
            surplus1 = mech_counter - damage;
            if (surplus1 > 0)
            {
                checkMechQueue(surplus1);
                surplus1 = 0;
            }
            if (dock_counter > 1)
            {
                checkDockQueue(dock_counter - 1);
            }
            break;
        case REPAIR:
            repair_progress += rand() % 10 + 15;
            if (repair_progress >= 100)
            {
                state = IDLE;
                damage = 0;
                repair_progress = 0;
            }
            surplus1 = mech_counter - damage;
            if (surplus1 > 0)
            {
                checkMechQueue(surplus1);
                surplus1 = 0;
            }
            if (dock_counter > 1)
            {
                checkDockQueue(dock_counter - 1);
            }
            break;
        default:
            debug("[!] Enteres impossible state");
            break;
        }
        string dock_vis(dock_counter, (char)79);
        dock_vis += string(docks - dock_counter, (char)45);
        string mech_vis(mech_counter, (char)79);
        mech_vis += string(mechanics - mech_counter, (char)45);
        string rep_vis(repair_progress / 10, (char)79);
        rep_vis += string(10 - repair_progress / 10, (char)45);
        if (state != REPAIR)
        {
            debug("State: %s\tDamage: %d\tMechanics: %d\t%s\tDocks: %d\t%s", states[state], damage, mech_counter, mech_vis.c_str(), dock_counter, dock_vis.c_str());
        }
        else
        {
            debug("State: %s\tDamage: %d\tMechanics: %d\t%s\tDocks: %d\t%s\tRepair progress: %s", states[state], damage, mech_counter, mech_vis.c_str(), dock_counter, dock_vis.c_str(), rep_vis.c_str());
        }
        sleep(1);
    }
}
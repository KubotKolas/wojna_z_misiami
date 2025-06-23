#include "functions.hpp"

void *startCommThread(void *ptr)
{
    while (state != FINISHED)
    {
        MPI_Recv(&packet, 1, MPI_PACKET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        handleMessage(&packet, &status);
    }
    return nullptr;
}

void requestMech(int n)
{
    Packet p;
    timer += 1;
    mech_priority = timer;
    p.timestamp = timer;
    p.tag = REQ_MECH;
    p.data = n;
    p.source = p_num;
    for (int i = 0; i < number_processes; i++)
    {
        if (i != p_num)
        {
            MPI_Send(&p, 1, MPI_PACKET_T, i, REQ_MECH, MPI_COMM_WORLD);
        }
    }
}

void requestDock()
{
    Packet p;
    timer += 1;
    dock_priority = timer;
    p.timestamp = timer;
    p.tag = REQ_DOCK;
    p.data = 1;
    p.source = p_num;
    for (int i = 0; i < number_processes; i++)
    {
        if (i != p_num)
        {
            MPI_Send(&p, 1, MPI_PACKET_T, i, REQ_DOCK, MPI_COMM_WORLD);
        }
    }
}

void sendMech(int destination, int n)
{
    Packet p;
    timer += 1;
    p.timestamp = timer;
    p.tag = T_MECH;
    p.source = p_num;
    p.data = n;
    MPI_Send(&p, 1, MPI_PACKET_T, destination, T_MECH, MPI_COMM_WORLD);
}

void sendDock(int dest, int n)
{
    Packet p;
    timer += 1;
    p.timestamp = timer;
    p.tag = T_DOCK;
    p.source = p_num;
    p.data = n;
    MPI_Send(&p, 1, MPI_PACKET_T, dest, T_DOCK, MPI_COMM_WORLD);
}

void handleMessage(Packet *packet, MPI_Status *status)
{
    timer = max(packet->timestamp, timer) + 1;

    switch (packet->tag)
    {
    case REQ_DOCK:
    {
        lock_guard<mutex> g(dock_mutex);
        dock_requests.push({packet->timestamp, packet->source});
    }
        switch (state)
        {
        case IDLE:
        case AWAIT_MECH:
            checkDockQueue(0);
            break;
        case AWAIT_DOCK:
        case REPAIR:
            if (dock_counter > 1)
            {
                checkDockQueue(dock_counter - 1);
            }
            break;
        default:
            break;
        }
        break;
    case REQ_MECH:
    {
        lock_guard<mutex> g(mech_mutex);
        mech_requests.push({packet->timestamp, packet->source});
    }
        switch (state)
        {
        case IDLE:
        case AWAIT_MECH:
            checkMechQueue(0);
            break;
        case AWAIT_DOCK:
        case REPAIR:
            surplus2 = mech_counter - damage;
            if (surplus2 > 0)
            {
                checkMechQueue(surplus2);
                surplus2 = 0;
            }
            break;
        default:
            break;
        }
        break;
    case T_DOCK:
    {
        lock_guard<mutex> g(dock_mutex);
        dock_counter += packet->data;
    }
        switch (state)
        {
        case IDLE:
        case AWAIT_MECH:
            checkDockQueue(0);
            break;
        case AWAIT_DOCK:
        case REPAIR:
            if (dock_counter > 1)
            {
                checkDockQueue(dock_counter - 1);
            }
            break;
        default:
            break;
        }
        break;
    case T_MECH:
    {
        lock_guard<mutex> g(mech_mutex);
        mech_counter += packet->data;
    }
        switch (state)
        {
        case IDLE:
        case AWAIT_MECH:
            checkMechQueue(0);
            break;
        case AWAIT_DOCK:
        case REPAIR:
            surplus2 = mech_counter - damage;
            if (surplus2 > 0)
            {
                checkMechQueue(surplus2);
                surplus2 = 0;
            }
            break;
        default:
            break;
        }
        break;
    default:
        debug("[!] [id:%d] Malformed request from %d!", p_num, status->MPI_SOURCE);
    }
}

void checkDockQueue(int surplus)
{
    if (surplus)
    {
        {
            lock_guard<mutex> g(dock_mutex);
            while (dock_counter > 1 && !dock_requests.empty())
            {
                int dest = dock_requests.top().second;
                sendDock(dest, 1);
                dock_counter -= 1;
            }
            return;
        }
    }
    else
    {
        if (state == REPAIR)
        {
            return;
        }
        {
            lock_guard<mutex> g(dock_mutex);
            if (dock_counter > 0 && !dock_requests.empty())
            {
                if (dock_requests.top().first < dock_priority)
                {
                    int dest = dock_requests.top().second;
                    sendDock(dest, dock_counter);
                    dock_requests.pop();
                    dock_counter = 0;
                    return;
                }
                if (dock_requests.top().first == dock_priority &&
                    dock_requests.top().second < p_num)
                {
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

void checkMechQueue(int surplus)
{
    if (surplus)
    {
        {
            lock_guard<mutex> g(mech_mutex);
            if (!mech_requests.empty())
            {
                int dest = mech_requests.top().second;
                sendMech(dest, surplus);
                mech_counter -= surplus;
                return;
            }
        }
    }
    else
    {
        if (state == REPAIR)
        {
            return;
        }
        {
            lock_guard<mutex> g(mech_mutex);
            if (mech_counter > 0 && !mech_requests.empty())
            {
                if (mech_requests.top().first < mech_priority)
                {
                    int dest = mech_requests.top().second;
                    sendMech(dest, mech_counter);
                    mech_requests.pop();
                    mech_counter = 0;
                    return;
                }
                if (mech_requests.top().first == mech_priority &&
                    mech_requests.top().second < p_num)
                {
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
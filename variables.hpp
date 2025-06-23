#pragma once

#include <algorithm>
#include <functional>
#include <limits>
#include <mpi.h>
#include <mutex>
#include <pthread.h>
#include <queue>
#include <random>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utility>
#include <vector>

using namespace std;

extern const int MAX_INT;
extern const int NITEMS;
extern int p_num;

extern int timer;
extern int dock_counter;
extern int mech_counter;
extern int surplus1;
extern int surplus2;

extern int damage;
extern int repair_progress;

extern int dock_priority;
extern int mech_priority;
extern bool waiting;

extern int number_processes;
extern int provided;
extern const int docks;
extern const int mechanics;

extern const char *states[];
extern const char *messages[];

using request = pair<int, int>;

extern priority_queue<request, vector<request>, greater<request>> dock_requests;
extern priority_queue<request, vector<request>, greater<request>> mech_requests;

extern mutex dock_mutex;
extern mutex mech_mutex;

extern pthread_t threadComm;

struct Packet
{
    int timestamp = 0;
    int tag = 0;
    int source = 0;
    int data = 0;
};

extern Packet packet;

enum Message
{
    REQ_DOCK,
    REQ_MECH,
    T_DOCK,
    T_MECH
};

enum State
{
    INIT,
    IDLE,
    AWAIT_MECH,
    AWAIT_DOCK,
    REPAIR,
    FINISHED
};

extern State state;

extern MPI_Datatype MPI_PACKET_T;

extern MPI_Status status;


#ifdef DEBUG
#define debug(FORMAT, ...) printf("%c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n", 27, (1 + (p_num / 7)) % 2, 31 + (6 + p_num) % 7, p_num, ##__VA_ARGS__, 27, 0, 37);
#else
#define debug(...) ;
#endif

#define println(FORMAT, ...) printf("%c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n", 27, (1 + (p_num / 7)) % 2, 31 + (6 + p_num) % 7, p_num, ##__VA_ARGS__, 27, 0, 37);
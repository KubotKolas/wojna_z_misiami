#pragma once
#include <functional>
#include <mpi.h>
#include <mutex>
#include <queue>
#include <utility>
#include <vector>

extern int rank;

#ifdef DEBUG
#define debug(FORMAT, ...) printf("%c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n", 27, (1 + (rank / 7)) % 2, 31 + (6 + rank) % 7, rank, ##__VA_ARGS__, 27, 0, 37);
#else
#define debug(...) ;
#endif

#define println(FORMAT, ...) printf("%c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n", 27, (1 + (rank / 7)) % 2, 31 + (6 + rank) % 7, rank, ##__VA_ARGS__, 27, 0, 37);

enum MessType
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

extern State stan;

struct packet
{
    int ts;
    int tag;
    int src;
    int data;
    int rndid;
};

#define NITEMS 5

extern int MAX_INT;

extern MPI_Datatype MPI_PAKIET_T;

extern int timer;
extern int dock_counter;
extern int mech_counter;
extern int dock_priority;
extern int mech_priority;

using request = std::pair<int, int>;

extern std::priority_queue<request, std::vector<request>, std::greater<request>> dock_requests;
extern std::priority_queue<request, std::vector<request>, std::greater<request>> mech_requests;

extern int proc_number;

extern std::mutex dock_mtx;
extern std::mutex mech_mtx;

extern const char *states[];
extern const char *msgs[];
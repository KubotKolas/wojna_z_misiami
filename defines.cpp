#include "defines.hpp"
#include "includes.hpp"

State stan = INIT;
MPI_Datatype MPI_PAKIET_T;
std::priority_queue<request, std::vector<request>, std::greater<request>> dock_requests;
std::priority_queue<request, std::vector<request>, std::greater<request>> mech_requests;
int MAX_INT = std::numeric_limits<int>::max();
int dock_counter = 0;
int mech_counter = 0;

int proc_number = 0;

int dmg = 0;

std::mutex dock_mtx;
std::mutex mech_mtx;

const char *states[] = {"INIT", "IDLE", "AWAIT_MECH", "AWAIT_DOCK", "REPAIR", "FINISHED"};

const char *msgs[] = {"REQ_DOCK", "REQ_MECH", "T_DOCK", "T_MECH"};
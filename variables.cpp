#include "variables.hpp"

const int MAX_INT = numeric_limits<int>::max();
const int NITEMS = 4;
int p_num = 0;

int timer = 0;
int dock_counter = 0;
int mech_counter = 0;
int surplus1 = 0;
int surplus2 = 0;

int damage = 0;
int repair_progress = 0;

int dock_priority = MAX_INT;
int mech_priority = MAX_INT;
bool waiting = false;

int number_processes = 0;
int provided = 0;
const int docks = 4;
const int mechanics = 20;

const char *states[] = {"INIT", "IDLE", "AWAIT_MECH", "AWAIT_DOCK", "REPAIR", "FINISHED"};
const char *messages[] = {"REQ_DOCK", "REQ_MECH", "T_DOCK", "T_MECH"};

priority_queue<request, vector<request>, greater<request>> dock_requests;
priority_queue<request, vector<request>, greater<request>> mech_requests;

mutex dock_mutex;
mutex mech_mutex;

pthread_t threadComm;

Packet packet;

State state = INIT;

MPI_Datatype MPI_PACKET_T;

MPI_Status status;
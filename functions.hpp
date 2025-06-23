#pragma once

#include "variables.hpp"

void mainLoop(int docks, int mechanics, int number_processes);

void checkThreadSupport(int provided);

void initializePacket();

void *startCommThread(void *pointer);

void handleMessage(Packet *p, MPI_Status *status);

void requestMech(int n);

void requestDock();

void sendMech(int destination, int n);

void sendDock(int destination, int n);

void checkDockQueue(int surplus);

void checkMechQueue(int surplus);

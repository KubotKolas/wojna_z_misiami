#pragma once

#include "defines.hpp"
void *startKomWatek(void *ptr);

void requestMech(int n);

void requestDock();

void sendMech(int dest, int n=1);

void sendDock(int dest, int n=1);
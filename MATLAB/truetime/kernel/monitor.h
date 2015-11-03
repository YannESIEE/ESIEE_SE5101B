/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef MONITOR_H
#define MONITOR_H

class Monitor {
 public:
  char* name;
  UserTask *heldBy;
  List *waitingQ;
  bool display;

  Monitor(char *n);
  ~Monitor();
};

/**
 * Monitor Constructor 
 */
Monitor::Monitor(char *n) {
  if (n==NULL) {
    name = NULL;
  } else {
    name = new char[strlen(n)+1];
    strcpy(name, n);
  }
  heldBy = NULL;
  waitingQ = NULL;
}

/**
 * Monitor Destructor 
 */
Monitor::~Monitor() {
  if (name) {
    delete[] name;
  }
  if (waitingQ) delete waitingQ;
}

#endif

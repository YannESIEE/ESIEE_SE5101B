/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef GET_PERIOD
#define GET_PERIOD

#include "getnode.cpp"

// get deadline of specific task
double ttGetPeriod(char *nameOfTask) {
    
  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttGetPeriod: Non-existent task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return 0.0;
  }
  UserTask* task = (UserTask*) dn->data; 
  if (task->periodichandler == NULL) {
    char buf[200];
    sprintf(buf, "ttGetPeriod: Task '%s' is not periodic!", nameOfTask);
    MEX_ERROR(buf);
    return 0.0;
  }

  return task->periodichandler->timer->period;
}

// get period of calling task
double ttGetPeriod() {

  if (!rtsys->running->isUserTask()) {
    MEX_ERROR("ttGetPeriod: Can not be called by interrupt handler!");
    return 0.0;
  }

  UserTask* task = (UserTask*) (rtsys->running); 
  if (task->periodichandler == NULL) {
    char buf[200];
    sprintf(buf,"ttGetPeriod: Task '%s' is not periodic!", task->name);
    MEX_ERROR(buf);
    return 0.0;
  }

  return task->periodichandler->timer->period;
}

#endif

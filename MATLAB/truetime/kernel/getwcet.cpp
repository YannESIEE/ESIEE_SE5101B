/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef GET_WCET
#define GET_WCET

#include "getnode.cpp"

// get wcet of specific task
double ttGetWCET(char *nameOfTask) {
  
  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttGetWCET: Non-existent task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return 0.0;
  }
  
  UserTask* task = (UserTask*) dn->data; 
  return task->wcExecTime;
}

// get wcet of calling task
double ttGetWCET() {

  if (!rtsys->running->isUserTask()) {
    MEX_ERROR("ttGetWCET: Can not be called by interrupt handler!");
    return 0.0;
  }

  UserTask* task = (UserTask*) (rtsys->running);

  return task->wcExecTime;
}

#endif

/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef SET_WCET
#define SET_WCET

#include "getnode.cpp"

// Setting wcet of specific task
void ttSetWCET(double value, char *nameOfTask) {
  
  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttSetWCET: Non-existent task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return;
  }
  
  UserTask* task = (UserTask*) dn->data; 
  task->wcExecTime = value;
} 

// Setting wcet of calling task
void ttSetWCET(double value) {

  if (rtsys->running == NULL) {
    MEX_ERROR("ttSetWCET: No running task!");
    return;
  }

  if (!rtsys->running->isUserTask()) {
    MEX_ERROR("ttSetWCET: Can not be called by interrupt handler!");
    return;
  }

  ttSetWCET(value, rtsys->running->name);
}

#endif

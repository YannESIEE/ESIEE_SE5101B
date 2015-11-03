/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef GET_DEADLINE
#define GET_DEADLINE

#include "getnode.cpp"

// get deadline of specific task
double ttGetDeadline(char *nameOfTask) {
    
  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttGetDeadline: Non-existent task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return 0.0;
  }

  UserTask* task = (UserTask*) dn->data; 
  return task->deadline;
}

// get deadline of calling task
double ttGetDeadline() {

  if (!rtsys->running->isUserTask()) {
    MEX_ERROR("ttGetDeadline: Can not be called by interrupt handler!");
    return 0.0;
  }

  UserTask* task = (UserTask*) (rtsys->running); 

  return task->deadline ;
}

#endif

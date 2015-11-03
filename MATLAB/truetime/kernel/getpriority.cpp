/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef GET_PRIORITY
#define GET_PRIORITY

#include "getnode.cpp"

// get priority of specific task
double ttGetPriority(char *nameOfTask) {

  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttGetPriority: Non-existent task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return 0.0;
  }
  
  UserTask* task = (UserTask*) dn->data; 
  return task->priority;
}

// get priority of calling task
double ttGetPriority() {

  if (!rtsys->running->isUserTask()) {
    MEX_ERROR("ttGetPriority: Can not be called by interrupt handler!");
    return 0.0;
  }

  UserTask* task = (UserTask*) (rtsys->running); 
  
  return task->priority;
}

#endif

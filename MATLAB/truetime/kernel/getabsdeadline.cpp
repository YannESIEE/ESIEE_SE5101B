/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef GET_ABS_DEADLINE
#define GET_ABS_DEADLINE

#include "getnode.cpp"

// get absolute deadline of specific task
double ttGetAbsDeadline(char *nameOfTask) {

  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttGetAbsDeadline: Non-existent task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return 0.0;
  }

  UserTask* task = (UserTask*) dn->data; 
  if (task->nbrJobs == 0) {
    char buf[200];
    sprintf(buf, "ttGetAbsDeadline: No running job of task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return 0.0;
  }

  return task->absDeadline;
}  

// get absolute deadline of calling task
double ttGetAbsDeadline() {

  if (!rtsys->running->isUserTask()) {
    MEX_ERROR("ttGetAbsDeadline: Can not be called by interrupt handler!");
    return 0.0;
  }

  UserTask* task = (UserTask*) (rtsys->running); 

  return task->absDeadline;
}

#endif

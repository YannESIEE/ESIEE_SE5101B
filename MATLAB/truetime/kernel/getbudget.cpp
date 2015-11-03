/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef GET_BUDGET
#define GET_BUDGET

#include "getnode.cpp"

// get budget of specific task
double ttGetBudget(char *nameOfTask) {
  
  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttGetBudget: Non-existent task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return 0.0;
  }

  UserTask* task = (UserTask*) dn->data; 
  if (task->nbrJobs == 0) {
    char buf[200];
    sprintf(buf, "ttGetBudget: No running job of task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return 0.0;
  }

  return task->budget;
}

// get budget of calling task
double ttGetBudget() {

  if (!rtsys->running->isUserTask()) {
    MEX_ERROR("ttGetBudget: Can not be called by interrupt handler!");
    return 0.0;
  }

  UserTask* task = (UserTask*) (rtsys->running); 

  return task->budget;
}

#endif

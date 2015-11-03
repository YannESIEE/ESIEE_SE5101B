/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef SET_BUDGET
#define SET_BUDGET

#include "getnode.cpp"

// set budget of specific task
void ttSetBudget(double budget, char *nameOfTask) {
  
  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttSetBudget: Non-existent task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return;
  }
  if (budget < -EPS) {
    MEX_ERROR("ttSetBudget: Negative budget!");
    return;
  }

  UserTask* task = (UserTask*) dn->data; 
  if (task->nbrJobs == 0) {
    char buf[200];
    sprintf(buf, "ttSetBudget: No running job of task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return;
  }

  if (rtsys->running == task->exectimeORhandler) {
    task->budget -= (rtsys->time - task->lastStart); // should be zero
    task->lastStart = rtsys->time;
  }
  
  double diff = budget - task->budget; 
  task->budget = budget;

  // Update WCET overrun timer
  if (task->exectimeORhandler != NULL) {
    task->exectimeORhandler->timer->time += diff;
    task->exectimeORhandler->moveToList(rtsys->timeQ);
  }
}

// set budget of calling task
void ttSetBudget(double budget) {

  if (rtsys->running == NULL) {
    MEX_ERROR("ttSetBudget: No running task!");
    return;
  }

  if (!rtsys->running->isUserTask()) {
    MEX_ERROR("ttSetBudget: Can not be called by interrupt handler!");
    return;
  }

  ttSetBudget(budget, rtsys->running->name);
}

#endif

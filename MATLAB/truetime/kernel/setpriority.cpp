/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef SET_PRIORITY
#define SET_PRIORITY

#include "getnode.cpp"

// Setting priority of specific task
void ttSetPriority(double prio, char *nameOfTask) {
  
  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttSetPriority: Non-existent task '%s'\n",nameOfTask);
    MEX_ERROR(buf);
    return;
  }
  if (prio < EPS) {
    MEX_ERROR("ttSetPriority: Priorities should be positive numbers!");
    return;
  }

  UserTask* task = (UserTask*) dn->data; 

  task->priority = prio;
  
  // Reshuffle readyQ if task there (relevant for FP)
  if (task->myList == rtsys->readyQ)
    task->moveToList(rtsys->readyQ);
}

// Setting priority of calling task
void ttSetPriority(double prio) {
  
  if(rtsys->running == NULL) {
    MEX_ERROR("ttSetPriority: No running task");
    return;
  }

  if (!rtsys->running->isUserTask()) {
    MEX_ERROR("ttSetPriority: Can not be called by interrupt handler!");
    return;
  }

  ttSetPriority(prio, rtsys->running->name);
}

#endif

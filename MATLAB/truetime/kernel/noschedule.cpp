/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef NO_SCHEDULE
#define NO_SCHEDULE

#include "getnode.cpp"

void ttNoSchedule(char* name) {

  DataNode* dn1, *dn2;
  UserTask* task;
  InterruptHandler* handler;

  // can only be called during initialization phase
  if (!rtsys->init_phase) {
    MEX_ERROR("ttNoSchedule: Can only be called from the init-function!");
    return;
  }

  dn1 = getNode(name, rtsys->taskList);
  dn2 = getNode(name, rtsys->handlerList);
  
  if (dn1 == NULL && dn2 == NULL) {
    char buf[200];
    sprintf(buf, "ttNoSchedule: Non-existent task or handler '%s'\n", name);
    MEX_ERROR(buf);
  }
  
  if (dn1 != NULL) {
    task = (UserTask*) dn1->data; 
    task->display = false;
    rtsys->nbrOfSchedTasks--;
  }
  
  if (dn2 != NULL) {
    handler = (InterruptHandler*) dn2->data; 
    handler->display = false;
    rtsys->nbrOfSchedHandlers--;
  }
}

#endif

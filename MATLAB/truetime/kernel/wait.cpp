/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef WAIT
#define WAIT

#include "exitmonitor.cpp"
#include "getnode.cpp"

void ttWait(char *nameOfEvent) {

  DataNode* dn;
  Event* ev;
  UserTask* task;

  dn = getNode(nameOfEvent, rtsys->eventList);
  if (dn == NULL) {
    // Event does not exist 
    char buf[100];
    sprintf(buf, "ttWait: Non-existent event '%s'!", nameOfEvent);
    MEX_ERROR(buf);
    return;
  }

  task = (UserTask*) rtsys->running;
  ev = (Event*) dn->data;
  
  rtsys->running->moveToList(ev->waitingQ);
  task->state = WAITING;

  // Execute suspend hook
  task->suspend_hook(task);

  if (!ev->isFree) {
    ttExitMonitor(ev->mon->name);
  }
}

#endif

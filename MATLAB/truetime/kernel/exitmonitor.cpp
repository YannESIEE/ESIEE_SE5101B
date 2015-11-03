/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef EXIT_MONITOR
#define EXIT_MONITOR

#include "getnode.cpp"

void ttExitMonitor(char *nameOfMonitor) {

  Monitor* mon;
  UserTask* task;

  DataNode* dn = getNode(nameOfMonitor, rtsys->monitorList);
  if (dn == NULL) {
    // Monitor does not exist 
    char buf[200];
    sprintf(buf, "ttExitMonitor: Non-existent monitor '%s'!", nameOfMonitor);
    MEX_ERROR(buf);
    return;
  }

  task = (UserTask*) rtsys->running;
  mon = (Monitor*) dn->data;
  
  if (mon->heldBy != task) {
    char buf[200];
    sprintf(buf, "ttExitMonitor: Task '%s' not holding monitor '%s'!", task->name, nameOfMonitor);
    MEX_ERROR(buf);
    return;
  }

  // Priority Inheritance, reset
  task->tempPrio = 0.0;
  task->prioRaised = false;
  // Reshuffle readyQ
  if (task->myList == rtsys->readyQ) {
    task->moveToList(rtsys->readyQ);
  }

  mon->heldBy = NULL;

  // Move first waiting task to readyQ
  task = (UserTask*) mon->waitingQ->getFirst();
  if (task != NULL) {
    task->moveToList(rtsys->readyQ);
    task->state = READY;
    mon->heldBy = task;
  }

}

#endif

/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef ENTER_MONITOR
#define ENTER_MONITOR

#include "getnode.cpp"

void ttEnterMonitor(char *nameOfMonitor) {
  
  Monitor* mon;
  UserTask* task;
  
  DataNode* dn = getNode(nameOfMonitor, rtsys->monitorList);
  if (dn == NULL) {
    // Monitor does not exist 
    char buf[200];
    sprintf(buf, "ttEnterMonitor: Non-existent monitor '%s'!", nameOfMonitor);
    MEX_ERROR(buf);
    return;
  }

  task = (UserTask*) rtsys->running;
  mon = (Monitor*) dn->data;

  if (mon->heldBy == NULL) { // Free 
    mon->heldBy = task;
  }
  else { // Not free
    
    task->moveToList(mon->waitingQ);
    task->state = WAITING;
    // Execute suspend hook
    task->suspend_hook(task);

    // Priority Inheritance
    double blockerPrio = (mon->heldBy->prioRaised) ? mon->heldBy->tempPrio : rtsys->prioFcn(mon->heldBy);
    double blockedPrio = rtsys->prioFcn(task);
    if (blockerPrio > blockedPrio) { // blocked by a lower-priority task
      mon->heldBy->tempPrio = blockedPrio;
      mon->heldBy->prioRaised = true;
      // Reshuffle readyQ
      if (mon->heldBy->myList == rtsys->readyQ) {
	mon->heldBy->moveToList(rtsys->readyQ);
      }
    }

  }

}

#endif

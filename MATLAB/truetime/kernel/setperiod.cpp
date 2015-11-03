/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef SET_PERIOD
#define SET_PERIOD

#include "getnode.cpp"

// Setting period of a specific task
void ttSetPeriod(double newPeriod, char *nameOfTask) {
  
  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttSetPeriod: Non-existent task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return;
  }
  UserTask* task = (UserTask*) dn->data; 
  if (task->periodichandler == NULL) {
    char buf[200];
    sprintf(buf, "ttSetPeriod: Task '%s' not periodic!", nameOfTask);
    MEX_ERROR(buf);
    return;
  }

  InterruptHandler* per_hdl = task->periodichandler;;
  double lastStart = per_hdl->timer->time - per_hdl->timer->period;
  double newTime = lastStart + newPeriod;
  per_hdl->timer->time = newTime;
  per_hdl->timer->period = newPeriod;

  // Update timeQ
  per_hdl->moveToList(rtsys->timeQ);

  // Reshuffle readyQ if task there (relevant for RM)
  if (task->myList == rtsys->readyQ)
    task->moveToList(rtsys->readyQ);
}

// Setting period of calling task
void ttSetPeriod(double newPeriod) {
  
  if(rtsys->running == NULL) {
    MEX_ERROR("ttSetPeriod: No running task!");
    return;
  }

  if (!rtsys->running->isUserTask()) {
    MEX_ERROR("ttSetPeriod: Can not be called by interrupt handler!");
    return;
  }

  ttSetPeriod(newPeriod, rtsys->running->name);
}
#endif

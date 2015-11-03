/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef CREATE_TIMER
#define CREATE_TIMER

#include "getnode.cpp"
#include "hdlerror.cpp"

void ttCreatePeriodicTimer(char *nameOfTimer, double start, double period, char *nameOfHandler) {
  
  DataNode* dn;

  if (strcmp(nameOfTimer,"") == 0) {
    MEX_ERROR("ttCreate(Periodic)Timer: Name should be a non-empty string!");
    return;
  }
  if (rtsys->prioFcn == NULL) {
    MEX_ERROR("ttCreate(Periodic)Timer: Kernel must be initialized before creation of timers!\n");
    return;
  }
  if (start - rtsys->time < -EPS) {
    MEX_ERROR("ttCreate(Periodic)Timer: Can't create timer backwards in time!");
    return;
  }
  dn = getNode(nameOfTimer, rtsys->timerList);
  if (dn != NULL) { 
    MEX_ERROR("ttCreate(Periodic)Timer: Name of timer not unique! Timer not created!");
    return;
  }
  dn = getNode(nameOfHandler, rtsys->handlerList);
  if (dn == NULL) {
    // Handler does not exist 
    char buf[200];
    sprintf(buf, "ttCreate(Periodic)Timer: Non-existent interrupt handler '%s'!", nameOfHandler);
    MEX_ERROR(buf);
    return;
  }
  
  InterruptHandler* hdl = (InterruptHandler*) dn->data;
  if (hdl->type == UNUSED) {
    Timer* t = new Timer(nameOfTimer);
    t->time = start;
    t->period = period;
    t->isPeriodic = (period > 0.0);
    hdl->timer = t;
    hdl->type = TIMER;

    // Insert pointer to handler and timer name in timerlist
    rtsys->timerList->appendNode(new DataNode(hdl, t->name));

    // Enter handler in timeQ
    hdl->moveToList(rtsys->timeQ);
  } else {
    // Handler already assigned, display error message 
    handler_error(hdl, "ttCreate(Periodic)Timer:");
  }
}    

void ttCreateTimer(char *nameOfTimer, double when, char *nameOfHandler) {
  
  // Creating timer with negative period will create a one-shot timer
  ttCreatePeriodicTimer(nameOfTimer, when, -1.0, nameOfHandler);
}

#endif

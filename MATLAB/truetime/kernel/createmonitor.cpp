/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef CREATE_MONITOR
#define CREATE_MONITOR

#include "getnode.cpp"

void ttCreateMonitor(char *name, bool display) {

  DataNode *dn;
  Monitor* mon;

  if (strcmp(name,"") == 0) {
    MEX_ERROR("ttCreateMonitor: Name should be a non-empty string!"); 
    return;
  }
  if (rtsys->prioFcn == NULL) {
    MEX_ERROR("ttCreateMonitor: Kernel must be initialized before creation of monitors!");
    return;
  }

  dn = getNode(name, rtsys->monitorList);
  if (dn!=NULL) {
    MEX_ERROR("ttCreateMonitor: Name of monitor not unique!");
    return;
  }

  mon = new Monitor(name); 
  mon->display = display;
  mon->waitingQ = new List("WaitingQ", rtsys->prioSort); // Sort waiting tasks by priority function
  if (display) rtsys->nbrOfSchedMonitors++;
  
  rtsys->monitorList->appendNode(new DataNode(mon, mon->name));
  
  rtsys->nbrOfMonitors++;

}

#endif

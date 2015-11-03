/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef CREATE_EVENT
#define CREATE_EVENT

#include "getnode.cpp"

// Create a free event
bool ttCreateEvent(char *name) {

  DataNode *dn;
  Event* e;

  if (strcmp(name, "") == 0) {
    MEX_ERROR("ttCreateEvent: Name should be a non-empty string!");
    return false;
  }
  if (rtsys->prioFcn == NULL) {
    MEX_ERROR("ttCreateEvent: Kernel must be initialized before creation of events!");
    return false;
  }
  dn = getNode(name, rtsys->eventList);
  if (dn != NULL) {
    MEX_ERROR("ttCreateEvent: Name of event not unique!");
    return false;
  }

  e = new Event(name, true, NULL);
  e->waitingQ = new List("WaitingQ", rtsys->prioSort); // Sort waiting tasks by priority function
  rtsys->eventList->appendNode(new DataNode(e, e->name));
  return true;
}

// Create an event associated with a monitor
void ttCreateEvent(char *name, char *monitor) {

  DataNode *dn;
  Event* e;
  Monitor* mon;

  dn = getNode(monitor, rtsys->monitorList);
  if (dn==NULL) {
    // Monitor does not exist 
    char buf[200];
    sprintf(buf, "ttCreateEvent: Non-existent monitor '%s'!", monitor);
    MEX_ERROR(buf);
    return;
  }
  
  if (ttCreateEvent(name)) {
    mon = (Monitor*) dn->data;
    dn = (DataNode*) rtsys->eventList->getLast();
    e = (Event*) dn->data;
    e->mon = mon;
    e->isFree = false;
  }
}


#endif

/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef CREATE_TRIGGER
#define CREATE_TRIGGER

#include "getnode.cpp"
#include "hdlerror.cpp"

void ttCreateExternalTrigger(char *nameOfHandler, double latency) {

  DataNode* dn = getNode(nameOfHandler, rtsys->handlerList);
  if (dn==NULL) {
    char buf[200];
    sprintf(buf, "ttCreateExternalTrigger: Non-existent interrupt handler '%s'!",nameOfHandler);
    MEX_ERROR(buf);
    return;
  }
  if (rtsys->prioFcn == NULL) {
    MEX_ERROR("ttCreateExternalTrigger: Kernel must be initialized before creation of triggers!");
    return;
  }

  InterruptHandler* hdl = (InterruptHandler*) dn->data;
  if (hdl->type == UNUSED) {
    hdl->trigger = new Trigger(latency);
    hdl->type = EXTERNAL;

    rtsys->triggerList->appendNode(new DataNode(hdl, ""));
    rtsys->nbrOfTriggers++;
  } else {
    // Handler already assigned, display error message
    handler_error(hdl, "ttCreateExternalTrigger:");
  }
}

#endif

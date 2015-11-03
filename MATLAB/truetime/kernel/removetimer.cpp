/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef REMOVE_TIMER
#define REMOVE_TIMER

#include "getnode.cpp"

void ttRemoveTimer(char *nameOfTimer) {

  DataNode* dn;
  InterruptHandler* hdl;
  
  if (strcmp(nameOfTimer, "")==0) {
    MEX_ERROR("ttRemoveTimer: Non-existent timer!");
    return;
  }
  dn = getNode(nameOfTimer, rtsys->timerList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttRemoveTimer: Non-existent timer '%s'!",nameOfTimer);
    MEX_ERROR(buf);
    return;
  } else {
    // Remove associated interrupt handler from Q
    hdl = (InterruptHandler*) dn->data;
    hdl->remove();
    // and delete timer
    delete hdl->timer;
    hdl->timer = NULL;
    hdl->type = UNUSED;
    rtsys->timerList->deleteNode(dn);
  }

}
#endif

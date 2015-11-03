/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef CREATE_HANDLER
#define CREATE_HANDLER

#include "getnode.cpp"

bool ttCreateInterruptHandler(char *name, double priority, double (*codeFcn)(int, void*)) {
  
  InterruptHandler* hdl;
  
  if (strcmp(name,"") == 0) {
    MEX_ERROR("ttCreateInterruptHandler: Name should be a non-empty string!");
    return false;
  }
  if (rtsys->prioFcn == NULL) {
    MEX_ERROR("ttCreateInterruptHandler: Kernel must be initialized before creation of handlers!");
    return false;
  }
  DataNode* dn =getNode(name, rtsys->handlerList);
  if (dn != NULL) {
    MEX_ERROR("ttCreateInterruptHandler: Name of handler not unique!");
    return false;
  }
  
  hdl = new InterruptHandler(name);
  hdl->codeFcn = codeFcn;

  hdl->priority = priority;
  hdl->handlerID = rtsys->nbrOfHandlers + 1;
  hdl->display = true;

  rtsys->handlerList->appendNode(new DataNode(hdl, hdl->name));

  rtsys->nbrOfSchedHandlers++;
  rtsys->nbrOfHandlers++;

  return true;
}

void ttCreateInterruptHandler(char *name, double priority, double (*codeFcn)(int, void*), void* data) {

  DataNode* n;

  if (ttCreateInterruptHandler(name, priority, codeFcn)) {
    n = (DataNode*) rtsys->handlerList->getLast();
    ((Task*) n->data)->data = data;
  } 
}

#endif

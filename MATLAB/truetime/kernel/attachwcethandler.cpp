/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef ATTACH_WCET_HANDLER
#define ATTACH_WCET_HANDLER

#include "getnode.cpp"
#include "hdlerror.cpp"

void ttAttachWCETHandler(char* nameOfTask, char* nameOfHandler) {

  DataNode *dn1, *dn2;

  dn1 = (DataNode*) getNode(nameOfTask, rtsys->taskList);
  if (dn1 == NULL) {
    char buf[200];
    sprintf(buf, "ttAttachWCETHandler: Non-existent task '%s'", nameOfTask);
    MEX_ERROR(buf);
    return;
  }
  dn2 = (DataNode*) getNode(nameOfHandler, rtsys->handlerList);
  if (dn2 == NULL) {
    char buf[200];
    sprintf(buf, "ttAttachWCETHandler: Non-existent handler '%s'", nameOfHandler);
    MEX_ERROR(buf);
    return;
  }

  UserTask* task = (UserTask*) dn1->data;
  InterruptHandler* hdl = (InterruptHandler*) dn2->data;
  
  if (hdl->type == UNUSED) {
    task->exectimeORhandler = hdl;
    hdl->usertask = task;
    hdl->timer = new Timer("");
    hdl->type = OVERRUN;
  } else {
    // Handler already assigned, display error message
    handler_error(hdl, "ttAttachWCETHandler:");
  }
}

#endif

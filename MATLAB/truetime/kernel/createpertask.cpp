/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef CREATE_PERIODIC_TASK
#define CREATE_PERIODIC_TASK

#include "createtask.cpp"

bool ttCreatePeriodicTask(char *name, double offset, double period, double priority, double (*codeFcn)(int, void*)) {

  DataNode* dn;
  InterruptHandler* hdl;
  Timer *t;
  
  if (ttCreateTask(name, period, priority, codeFcn)) {

    dn = (DataNode*) rtsys->taskList->getLast(); // last created task

    // Create interrupt handler invoked periodically to create task jobs
    // see codefunctions.cpp for the code function
    hdl = new InterruptHandler("perHandler");
    hdl->codeFcn = rtsys->periodicTaskHandlerCode;

    hdl->handlerID = rtsys->nbrOfHandlers + 1;
    hdl->priority = -1000.0;
    hdl->display = false;
    hdl->data = (UserTask*) dn->data;

    rtsys->handlerList->appendNode(new DataNode(hdl, NULL));
    rtsys->nbrOfHandlers++;

    // Creating periodic timer that triggers the handler
    t = new Timer("perTimer");
    t->time = offset;  // First timer expiry at task offset
    t->period = period;
    t->isPeriodic = true;

    hdl->timer = t;
    hdl->moveToList(rtsys->timeQ);
    hdl->type = TIMER;
    
    ((UserTask*) dn->data)->periodichandler = hdl;
    return true;
  } else {
    return false;
  }
}

void ttCreatePeriodicTask(char *name, double offset, double period, double priority, double (*codeFcn)(int, void*), void* data) {
  
  DataNode *n;

  if (ttCreatePeriodicTask(name, offset, period, priority, codeFcn)) {
    n = (DataNode*) rtsys->taskList->getLast();
    ((Task*) n->data)->data = data;
  }
}

#endif

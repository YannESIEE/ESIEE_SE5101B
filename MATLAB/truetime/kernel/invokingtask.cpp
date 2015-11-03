/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef INVOKING_TASK
#define INVOKING_TASK

// To be called from interrupt handler associated
// with task overruns to know the triggering task.
char *ttInvokingTask(void) {

  // Check typeid of running task
  if (rtsys->running->isUserTask()) {
    MEX_ERROR("ttInvokingTask: should be called from interrupt handler!");
    return NULL;
  }

  InterruptHandler* hdl = (InterruptHandler*) rtsys->running;

  if (hdl->usertask == NULL) {
    return NULL;
  } else {
    return hdl->usertask->name;
  }

}

#endif

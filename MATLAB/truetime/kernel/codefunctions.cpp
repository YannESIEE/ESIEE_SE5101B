/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef CODE_FUNCTIONS
#define CODE_FUNCTIONS

#include "createjob.cpp"

double contextSwitchCode(int seg, void* data) {

  if (seg == 1) {
    return rtsys->contextSimTime;
  } else {
    return FINISHED;
  }
}

double periodicTaskHandlerCode(int seg, void* data) {

  UserTask* task = (UserTask*) data;
  
  ttCreateJob(task->name);
  //printf("ttCreateJob %s at %f\n", task->name, rtsys->time);
  return FINISHED;
}

#endif

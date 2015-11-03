/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef SLEEP
#define SLEEP

#include "getnode.cpp"

void ttSleepUntil(double time) {
 
  if (rtsys->running == NULL) {
    MEX_ERROR("ttSleep(Until): No running task!");
    return;
  }
  
  if (rtsys->running->isUserTask()) {
    UserTask* task = (UserTask*) rtsys->running;
    task->release = time;
    task->moveToList(rtsys->timeQ);
    task->state = SLEEPING;
    // Call suspend hook of task
    task->suspend_hook(task);
  } else {
    printf("ttSleep(Until): Call from interrupt handler ignored!\n");
  }
}

void ttSleep(double duration) {
  
  ttSleepUntil(rtsys->time + duration);
}

#endif

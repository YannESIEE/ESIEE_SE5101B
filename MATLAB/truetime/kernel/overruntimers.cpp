/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef OR_TIMERS
#define OR_TIMERS

#include "getnode.cpp"

void setDLTimer(UserTask *task, double time) {

  InterruptHandler* hdl = task->deadlineORhandler;
  hdl->timer->time = time;
  hdl->timer->period = -1.0;
  hdl->timer->isPeriodic = false;
  
  hdl->moveToList(rtsys->timeQ);
}  

void setWCETTimer(UserTask *task, double time) {

  InterruptHandler* hdl = task->exectimeORhandler;
  hdl->timer->time = time;
  hdl->timer->period = -1.0;
  hdl->timer->isPeriodic = false;

  hdl->moveToList(rtsys->timeQ);
}  

void removeWCETTimer(UserTask* task) {

  if (task->exectimeORhandler)
    task->exectimeORhandler->remove();
}

void removeORTimers(UserTask *task) {

  if (task->deadlineORhandler)
    task->deadlineORhandler->remove();
  if (task->exectimeORhandler)
    task->exectimeORhandler->remove();
}

#endif

/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef LOG_START
#define LOG_START

#define simtime2time(time) ((time - rtsys->clockOffset) / rtsys->clockDrift)

void logstart(UserTask* task, int logID) {
  Log *log;
  
  log = task->logs[logID-1];
  if (log) {
    if (log->tempIndex < log->size) {
      log->vals[log->tempIndex] = simtime2time(rtsys->time);
      log->tempIndex++;
    }
  }
}

void ttLogStart(int logID) {
  UserTask* usertask;

  if (logID < 1 || logID > NBRUSERLOGS) {
    MEX_ERROR("ttLogStart: Log ID out of bounds!");
    return;
  }
  if (rtsys->running->isUserTask()) {
    usertask = (UserTask*) rtsys->running;
    logstart(usertask, logID+5);     // 5 pre-defined log types
  } else {
    printf("ttLogStart: Call from interrupt handler ignored!\n");
    return;
  }
}

#endif

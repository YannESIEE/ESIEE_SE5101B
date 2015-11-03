/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef LOG_NOW
#define LOG_NOW

#define simtime2time(time) ((time - rtsys->clockOffset) / rtsys->clockDrift)

void logwrite(UserTask* task, int logID, double value) {
  Log *log;

  log = task->logs[logID-1];
  if (log) {
    if (log->entries < log->size) {
      log->vals[log->entries] = value;
      log->entries++;
    }
  }
}

void ttLogNow(int logID) {
  UserTask* usertask;

  if (logID < 1 || logID > NBRUSERLOGS) {    
    MEX_ERROR("ttLogNow: Log ID out of bounds!");
    return;
  }
  if (rtsys->running->isUserTask()) {
    usertask = (UserTask*) rtsys->running;
    // 5 pre-defined log types    
    logwrite(usertask, logID+5, simtime2time(rtsys->time));
  } else {
    printf("ttLogNow: Call from interrupt handler ignored!\n");
    return;
  }
}

#endif

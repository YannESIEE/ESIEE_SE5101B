/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef LOG_STOP
#define LOG_STOP
#define simtime2time(time) ((time - rtsys->clockOffset) / rtsys->clockDrift)

void logstop(UserTask* task, int logID) {
  Log *log;  

  log = task->logs[logID-1];
  if (log) {
    if (log->entries < log->size) {
      // Subtract value written by logstart
      double r = simtime2time(rtsys->time) - log->vals[log->entries];
      log->vals[log->entries] = r;
      log->entries++;
    }
  }
}

void ttLogStop(int logID) {
  UserTask* usertask;

  if (logID < 1 || logID > NBRUSERLOGS) {
    MEX_ERROR("ttLogStop: Log ID out of bounds!");
    return;
  }
  if (rtsys->running->isUserTask()) {
    usertask = (UserTask*) rtsys->running;
    logstop(usertask, logID+5);     // 5 pre-defined log types
  } else {
    printf("ttLogStop: Call from interrupt handler ignored!\n");
    return;
  }
}

#endif

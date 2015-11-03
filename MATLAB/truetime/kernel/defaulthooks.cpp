/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef DEFAULT_HOOKS
#define DEFAULT_HOOKS

/**
 * Default hooks used to implement overrun handling, 
 * context switch simulation and logging.
 */

#include "overruntimers.cpp"
#include "logstart.cpp"
#include "logstop.cpp"
#include "lognow.cpp"

void default_arrival(UserTask *task) {

  //printf("Arrival-hook for task '%s' at time %5.8f\n",task->name,rtsys->time);

  // Store arrival time in relevant logs
  logstart(task, RESPONSETIMELOG);
  logstart(task, RELEASELATENCYLOG);
  logstart(task, STARTLATENCYLOG);

}

void default_release(UserTask *task) {

  //printf("Release-hook for task '%s' at time %5.8f\n",task->name,rtsys->time);

  // Set deadline OR-timer
  if (task->deadlineORhandler != NULL) {
    if(task->absDeadline > rtsys->time) {
      // Set up timer to expire at the abs deadline
      setDLTimer(task, task->absDeadline);
    }
    else {
      // execute deadline handler at once 
      task->deadlineORhandler->moveToList(rtsys->readyQ);
    }
  }

  // Store release latency in log
  logstop(task, RELEASELATENCYLOG);

}

void default_start(UserTask *task) {

  //printf("Start-hook for task '%s' at time %5.8f\n",task->name,rtsys->time);

  // Update the last start time for the task
  task->lastStart = rtsys->time;
  task->budget = task->wcExecTime;

  // Set exectime OR-timer
  if (task->exectimeORhandler != NULL && task->budget > EPS) {
    double time = rtsys->time + task->budget;
    setWCETTimer(task, time);
  }

  // Simulate context switch  
  if (rtsys->contextSwitchTime > EPS) {
    if (rtsys->suspended == NULL) {
      // System is idle, only context restore
      rtsys->contextSimTime = rtsys->contextSwitchTime / 2.0;
    } else {
      // full context save/restore
      rtsys->contextSimTime = rtsys->contextSwitchTime;
    }
    rtsys->kernelHandler->moveToList(rtsys->readyQ);

    // store context switch time in log
    logwrite(task, CONTEXTRESTORELOG, simtime2time(rtsys->time));
  }

  // store start latency in log
  logstop(task, STARTLATENCYLOG);  

  // execution time log entry
  Log* log = task->logs[EXECTIMELOG-1];
  if (log) {
    if (log->entries < log->size) {
      log->vals[log->entries] = 0.0;
      log->temp = simtime2time(rtsys->time);
    }
  }
}

void default_suspend(UserTask* task) {

  //printf("Suspend-hook for task '%s' at time %5.8f\n",task->name,rtsys->time);

  // Decrease budget
  task->budget -= (rtsys->time - task->lastStart);

  // Delete exectime OR-timer if it exists
  removeWCETTimer(task);

  // Save task as suspended
  rtsys->suspended = task;

  // update execution time log entry
  Log *log = task->logs[EXECTIMELOG-1];
  if (log) {
    if (log->entries < log->size) {
      log->vals[log->entries] += (simtime2time(rtsys->time) - log->temp);
      log->temp = simtime2time(rtsys->time);
    }
  }
}

void default_resume(UserTask* task) {

  //printf("Resume-hook for task '%s' at time %5.8f\n",task->name,rtsys->time);

  // Update the last start time for the task
  task->lastStart = rtsys->time;

  // Create exectime OR-timer
  if (task->exectimeORhandler != NULL && task->budget > EPS) {
    double time = rtsys->time + task->budget;
    setWCETTimer(task, time);
  }

  // Simulate context switch
  if (rtsys->suspended != NULL) {
    if (rtsys->contextSwitchTime > EPS && task != rtsys->suspended) {
      // full context save/restore
      rtsys->contextSimTime = rtsys->contextSwitchTime;
      rtsys->kernelHandler->moveToList(rtsys->readyQ);

      // store context switch time in log
      logwrite(task, CONTEXTRESTORELOG, simtime2time(rtsys->time));
    } 
    rtsys->suspended = NULL; 
  }

}

void default_finish(UserTask* task) {

  //printf("Finish-hook for task '%s' at time %5.8f\n",task->name,rtsys->time);
  
  // Delete deadline and exectime OR-timers if they exist
  removeORTimers(task);

  // Simulate context switch  
  double contextSave = 0.0;
  if (rtsys->contextSwitchTime > EPS) {
    if (rtsys->suspended == NULL) {
      // context save
      rtsys->contextSimTime = rtsys->contextSwitchTime / 2.0;
      rtsys->kernelHandler->moveToList(rtsys->readyQ);
    } else {
      // context save/restore triggered from next resume_hook
      rtsys->suspended = task;
    }
    contextSave = rtsys->contextSwitchTime / 2.0;
  }

  // store response time in log
  logstop(task, RESPONSETIMELOG);  

  // store execution time in log
  Log* log = task->logs[EXECTIMELOG-1];
  if (log) {
    double exectime = log->vals[log->entries] + (simtime2time(rtsys->time) + 
						 contextSave - log->temp);
    logwrite(task, EXECTIMELOG, exectime);
  }
}

#endif

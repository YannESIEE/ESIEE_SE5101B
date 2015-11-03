/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef SET_ABS_DEADLINE
#define SET_ABS_DEADLINE

#include "getnode.cpp"
#include "overruntimers.cpp"

// set absolute deadline of specific task
void ttSetAbsDeadline(double time, char *nameOfTask) {

  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttSetAbsDeadline: Non-existent task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return;
  }
  UserTask* task = (UserTask*) dn->data; 
  if (task->nbrJobs == 0) {
    char buf[200];
    sprintf(buf, "ttSetAbsDeadline: No running job for '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return;
  }
  
  // set the absolute deadline
  task->absDeadline = time;
    
  // Update deadline overrun timer
  if (task->deadlineORhandler != NULL) {
    if (rtsys->running == task->deadlineORhandler) {
      // SetAbsDeadline called from DL overrun handler
      // timer just expired, do nothing
    } else {
      // update deadline overrun timer
      setDLTimer(task, time);
    }
  }
    
  // Reshuffle readyQ if task there (relevant for EDF)
  if (task->myList == rtsys->readyQ)
    task->moveToList(rtsys->readyQ);
}

// Set absolute deadline of calling task
void ttSetAbsDeadline(double time) {
  
  if (rtsys->running == NULL) {
    MEX_ERROR("ttSetAbsDeadline: No running task!");
    return;
  }

  if (!rtsys->running->isUserTask()) {
    MEX_ERROR("ttSetAbsDeadline: Can not be called by interrupt handler!");
    return;
  }

  ttSetAbsDeadline(time, rtsys->running->name);
}

#endif

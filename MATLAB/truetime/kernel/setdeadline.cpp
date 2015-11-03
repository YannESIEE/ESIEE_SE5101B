/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef SET_DEADLINE
#define SET_DEADLINE

#include "getnode.cpp"

// Setting deadline of specific task
void ttSetDeadline(double value, char *nameOfTask) {
  
  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttSetDeadline: Non-existent task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return;
  }

  UserTask* task = (UserTask*) dn->data; 
  task->deadline = value;
  task->absDeadline = task->release + task->deadline;

  // Reshuffle readyQ if task there (relevant for DM/EDF)
  if (task->myList == rtsys->readyQ)
    task->moveToList(rtsys->readyQ);

} 

// Setting deadline of calling task
void ttSetDeadline(double value) {
  
  if(rtsys->running == NULL) {
    MEX_ERROR("ttSetDeadline: No running task!");
    return;
  }

  if (!rtsys->running->isUserTask()) {
    MEX_ERROR("ttSetDeadline: Can not be called by interrupt handler!");
    return;
  }

  ttSetDeadline(value, rtsys->running->name);
}

#endif

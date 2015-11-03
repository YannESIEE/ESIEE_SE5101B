/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef TAKE
#define TAKE

#include "getnode.cpp"

void ttTake(char *nameOfSemaphore) {
  
  Semaphore* sem;
  UserTask* task;
  
  DataNode* dn = getNode(nameOfSemaphore, rtsys->semaphoreList);
  if (dn == NULL) {
    // Semaphore does not exist 
    char buf[200];
    sprintf(buf, "ttTake: Non-existent semaphore '%s'!", nameOfSemaphore);
    MEX_ERROR(buf);
    return;
  }

  task = (UserTask*) rtsys->running;
  sem = (Semaphore*) dn->data;

  sem->value--;

  if (sem->value < 0) { // Not free 
    task->moveToList(sem->waitingQ);
    task->state = WAITING;
    // Execute suspend hook
    task->suspend_hook(task);

  }

}

#endif

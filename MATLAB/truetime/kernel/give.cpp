/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef GIVE
#define GIVE

#include "getnode.cpp"

void ttGive(char *nameOfSemaphore) {

  Semaphore* sem;
  UserTask* task;

  DataNode* dn = getNode(nameOfSemaphore, rtsys->semaphoreList);
  if (dn == NULL) {
    // Semaphore does not exist 
    char buf[200];
    sprintf(buf, "ttGive: Non-existent semaphore '%s'!", nameOfSemaphore);
    MEX_ERROR(buf);
    return;
  }

  task = (UserTask*) rtsys->running;
  sem = (Semaphore*) dn->data;
  
  if (sem->value >= sem->maxval) {  // Check if maximum already reached
    return;                         // In that case, do nothing
  }

  sem->value++;

  if (sem->value <= 0) {  // This test is probably unnecessary

    // Move first waiting task to readyQ
    task = (UserTask*) sem->waitingQ->getFirst();
    if (task != NULL) {
      task->moveToList(rtsys->readyQ);
      task->state = READY;
    }

  }

}

#endif

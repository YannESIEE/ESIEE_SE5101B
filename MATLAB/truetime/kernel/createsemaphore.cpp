/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef CREATE_SEMAPHORE
#define CREATE_SEMAPHORE

#include "getnode.cpp"

void ttCreateSemaphore(char *name, int initval, int maxval) {

  DataNode *dn;
  Semaphore* sem;

  if (strcmp(name,"") == 0) {
    MEX_ERROR("ttCreateSemaphore: Name should be a non-empty string!"); 
    return;
  }
  if (rtsys->prioFcn == NULL) {
    MEX_ERROR("ttCreateSemaphore: Kernel must be initialized before creation of semaphores!");
    return;
  }

  dn = getNode(name, rtsys->semaphoreList);
  if (dn!=NULL) {
    MEX_ERROR("ttCreateSemaphore: Name of semaphore not unique!");
    return;
  }

  sem = new Semaphore(name); 
  sem->value = initval;
  sem->maxval = maxval;
  sem->waitingQ = new List("WaitingQ", NULL); // waiting tasks in FIFO order
  rtsys->semaphoreList->appendNode(new DataNode(sem, sem->name));
}

void ttCreateSemaphore(char *name, int initval) {

  ttCreateSemaphore(name, initval, INT_MAX);

}


#endif

/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef GET_RELEASE
#define GET_RELEASE

#include "getnode.cpp"

// get release of specific task
double ttGetRelease(char *nameOfTask) {
    
  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttGetRelease: Non-existent task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return 0.0;
  }

  UserTask* task = (UserTask*) dn->data; 
  if (task->nbrJobs == 0) {
    char buf[200];
    sprintf(buf, "ttGetRelease: No jobs for task '%s'!",nameOfTask);
    MEX_ERROR(buf);
    return 0.0;
  }

  return task->release;
}

// get release of calling task
double ttGetRelease() {

  if (!rtsys->running->isUserTask()) {
    MEX_ERROR("ttGetRelease: Can not be called by interrupt handler!");
    return 0.0;
  }

  UserTask* task = (UserTask*) (rtsys->running); 

  return task->release;
}

#endif

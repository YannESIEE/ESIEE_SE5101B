/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef CREATE_JOB
#define CREATE_JOB

#include "getnode.cpp"

void ttCreateJob(char *taskName) {

  DataNode* dn = getNode(taskName, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttCreateJob: Non-existent task '%s'", taskName);
    MEX_ERROR(buf);
    return;
  }

  UserTask* task = (UserTask*) dn->data; 
  task->arrival_hook(task);

  if (task->nbrJobs == 0) {
    task->release = rtsys->time;
    task->absDeadline = rtsys->time + task->deadline;
    task->budget = task->wcExecTime;
    task->moveToList(rtsys->timeQ);
    task->release_hook(task); // arrival = release
  } else {
    task->pending->appendNode(new DataNode(new double(rtsys->time), NULL));
  }

  task->nbrJobs++;
  
  return;
}

#endif

/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef KILL_JOB
#define KILL_JOB

#include "getnode.cpp"

void ttKillJob(char *taskName) {

  DataNode* dn = getNode(taskName, rtsys->taskList);
  UserTask* task = (UserTask*) dn->data; 

  if (task == NULL) {
    char buf[200];
    sprintf(buf, "ttKillJob: Non-existent task '%s'!", taskName);
    MEX_ERROR(buf);
    return;
  }
  if (task->nbrJobs == 0) {
    char buf[200];
    sprintf(buf, "ttKillJob: No job to kill for task '%s'!", taskName);
    MEX_ERROR(buf);
    return;
  }
  if (task == rtsys->running) {
    char buf[200];
    sprintf(buf,"ttKillJob: Can not kill myself while running!");
    MEX_ERROR(buf);
    return;
  }

  task->execTime = 0.0;
  task->segment = 0;
  // Remove task from current Q
  task->remove();

  // Execute finished-hook
  task->finish_hook(task);
  task->state = IDLE;

  task->nbrJobs--;
  // Release next job if any
  if (task->nbrJobs > 0) {
    // next pending release
    dn = (DataNode*) task->pending->getFirst();
    double* release = (double*) dn->data;
    task->release = *release;
    task->absDeadline = *release + task->deadline;
    task->moveToList(rtsys->timeQ);
    task->pending->deleteNode(dn);
    delete release;
    // Execute release-hook 
    task->release_hook(task);
    task->state = SLEEPING;
  }

}

#endif

/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef CREATE_TASK
#define CREATE_TASK

#include "getnode.cpp"

bool ttCreateTask(char *name, double deadline, double priority, double (*codeFcn)(int, void*)) {

  UserTask* task;

  if (strcmp(name,"") == 0) {
    MEX_ERROR("ttCreate(Periodic)Task: Name should be a non-empty string!");
    return false;
  }
  if (rtsys->prioFcn == NULL) {
    MEX_ERROR("ttCreate(Periodic)Task: Kernel must be initialized before creation of tasks!");
    return false;
  }
  DataNode* dn = getNode(name, rtsys->taskList);
  if (dn != NULL) { 
    MEX_ERROR("ttCreate(Periodic)Task: Name of task not unique! Task not created!");
    return false;
  }
  if (priority < EPS) {
    MEX_ERROR("ttCreate(Periodic)Task: Priorities are supposed to be positive real numbers!");
    return false;
  }
  
  task = new UserTask(name);
  task->codeFcn = codeFcn;

  task->priority = priority;
  task->wcExecTime = deadline;
  task->deadline = deadline;
  
  task->state = IDLE;
  task->taskID = rtsys->nbrOfTasks + 1;
  task->isPreemptable = true;
  task->display = true;

  task->lastStart = 0.0;
  task->absDeadline = deadline;
  task->release = 0.0;
  task->budget = deadline;
  
  task->arrival_hook  = rtsys->default_arrival;
  task->release_hook  = rtsys->default_release;
  task->start_hook    = rtsys->default_start;
  task->suspend_hook  = rtsys->default_suspend;
  task->resume_hook   = rtsys->default_resume;
  task->finish_hook   = rtsys->default_finish;
      
  rtsys->taskList->appendNode(new DataNode(task, task->name));

  rtsys->nbrOfSchedTasks++;
  rtsys->nbrOfTasks++;

  return true;
}

void ttCreateTask(char *name, double deadline, double priority, double (*codeFcn)(int, void*), void* data) {

  DataNode *n;

  if (ttCreateTask(name, deadline, priority, codeFcn)) {
    n = (DataNode*) rtsys->taskList->getLast();
    ((Task*) n->data)->data = data;
  } 
}

#endif

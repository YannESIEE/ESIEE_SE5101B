/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef NON_PREEMPTABLE
#define NON_PREEMPTABLE

#include "getnode.cpp"

void ttNonPreemptable(char* nameOfTask) {

  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttNonPreemptable: Non-existent task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return;
  }

  UserTask* task = (UserTask*) dn->data; 
  task->isPreemptable = false;
}

#endif

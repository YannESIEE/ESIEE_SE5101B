/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef CREATE_LOG
#define CREATE_LOG

#include "getnode.cpp"

void ttCreateLog(char* taskname, int logtype, char* variable, int size) {

  DataNode* dn;
  UserTask* task;
  char buf[100];
  
  // can only be called during initialization phase
  if (!rtsys->init_phase) {
    MEX_ERROR("ttCreateLog: Can only be called from the init-function!");
    return;
  }
  if (logtype > USERLOG || logtype < RESPONSETIMELOG) {
    sprintf(buf,"ttCreateLog: unknown log type %d", logtype);
    MEX_ERROR(buf);
    return;
  }

  dn = getNode(taskname, rtsys->taskList);
  if (dn == NULL) {
    sprintf(buf, "ttCreateLog: Non-existent task '%s'", taskname);
    MEX_ERROR(buf);
    return;
  }
  task = (UserTask*) dn->data;
  if (logtype != USERLOG) {
    if (task->logs[logtype-1] != NULL) {
      MEX_ERROR("ttCreateLog: log already assigned");
      return;
    }
    task->logs[logtype-1] = new Log(variable, size);
  } else {
    if (task->nbrOfUserLogs == NBRUSERLOGS) {
      sprintf(buf, "ttCreateLog: Maximum number of user logs exceeded! Can not create log variable %s for task %s", variable, taskname);
      MEX_ERROR(buf);
      return;
    }
    task->logs[task->nbrOfUserLogs+5] = new Log(variable, size);
    task->nbrOfUserLogs++;
  }
}

#endif

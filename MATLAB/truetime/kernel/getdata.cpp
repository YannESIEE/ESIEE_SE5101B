/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef GET_DATA
#define GET_DATA

#include "getnode.cpp"

mxArray* ttGetDataAux(char *nameOfTask) {  
  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttGetData: Non-existent task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return NULL;
  }
  UserTask* task = (UserTask*) dn->data; 
  if (task->dataMATLAB == NULL) {
    char buf[200];
    sprintf(buf, "ttGetData: Task '%s' doesn't have data!", nameOfTask);
    MEX_ERROR(buf);
    return NULL;
  }
  return mxDuplicateArray(task->dataMATLAB);
}

void* ttGetData(char *nameOfTask) {  
  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttGetData: Non-existent task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return NULL;
  }
  UserTask* task = (UserTask*) dn->data; 
  if (task->data == NULL) {
    char buf[200];
    sprintf(buf, "ttGetData: Task '%s' doesn't have data!", nameOfTask);
    MEX_ERROR(buf);
    return NULL;
  }
  return task->data;
  
}
#endif

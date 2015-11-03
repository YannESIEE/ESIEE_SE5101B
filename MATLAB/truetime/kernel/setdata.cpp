/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef SET_DATA
#define SET_DATA

#include "getnode.cpp"

void ttSetDataAux(char *nameOfTask, const mxArray *data) {  
  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttSetData: Non-existent task '%s'!", nameOfTask);
    MEX_ERROR(buf);
    return;
  }
  UserTask* task = (UserTask*) dn->data; 
  if (task->dataMATLAB == NULL) {
    char buf[200];
    sprintf(buf, "ttSetData: Task '%s' doesn't have data!", nameOfTask);
    MEX_ERROR(buf);
    return;
  }
  mxDestroyArray(task->dataMATLAB);
  task->dataMATLAB = mxDuplicateArray(data);
  mexMakeArrayPersistent(task->dataMATLAB);
}

/*void* ttGetData(char *nameOfTask) {  
  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttGetPeriod: Non-existent task '%s'!", nameOfTask);
    MEX_ERROR(buf);
  }
  UserTask* task = (UserTask*) dn->data; 
  if (task->data == NULL) {
    char buf[200];
    sprintf(buf, "ttGetData: Task '%s' doesn't have data!", nameOfTask);
    MEX_ERROR(buf);
  }
  return task->data;
  
}*/
#endif

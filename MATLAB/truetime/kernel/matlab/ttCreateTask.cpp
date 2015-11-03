/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#define KERNEL_MATLAB
#include "../ttkernel.h" 

RTsys *rtsys;

#include "../createtask.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  rtsys = getrtsys() ; // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs < 4 || nrhs > 5) {
    MEX_ERROR("ttCreateTask: Wrong number of input arguments! \nUsage: ttCreateTask(name, deadline, priority, codefcn)\n       ttCreateTask(name, deadline, priority, codefcn, data)");
    return;
  }

  if (mxIsChar(prhs[0]) != 1) {
    MEX_ERROR("ttCreateTask: name must be a string");
    return;
  }
  if (!mxIsDoubleScalar(prhs[1])) {
    MEX_ERROR("ttCreateTask: deadline must be a double scalar");
    return;
  }
  if (!mxIsDoubleScalar(prhs[2])) {
    MEX_ERROR("ttCreateTask: priority must be a double scalar");
    return;
  }
  if (mxIsChar(prhs[3]) != 1 || mxGetM(prhs[3]) != 1) {
    MEX_ERROR("ttCreateTask: codeFcn must be a non-empty string");
    return;
  }
  
  char name[100];
  mxGetString(prhs[0], name, 100);
  
  double deadline = *mxGetPr(prhs[1]);
  if (deadline < -EPS) {
    deadline = 0.0;
    printf("ttCreateTask: negative deadline changed to zero\n");
  }

  double priority = *mxGetPr(prhs[2]);

  char codeFcn[100];
  mxGetString(prhs[3], codeFcn, 100);

  // Make sure that the code function exists in Matlab path
  // and that the code function is syntactically correct.
  mxArray *lhs[1];
  mxArray *rhs[1];
  rhs[0] = mxDuplicateArray(prhs[3]);
  mexCallMATLAB(1, lhs, 1, rhs, "exist");
  int number = (int) *mxGetPr(lhs[0]);
  if (number == 0) {
    char buf[200];
    sprintf(buf, "ttCreateTask: codeFcn '%s' not in path! Task '%s' not created!", codeFcn, name);
    MEX_ERROR(buf);
    return;
  }

  if (ttCreateTask(name, deadline, priority, NULL)) {
  
    // Add name of code function (m-file) and data variable
    DataNode *n = (DataNode*) rtsys->taskList->getLast();
    UserTask *usertask = (UserTask*) n->data;
    
    usertask->codeFcnMATLAB = new char[strlen(codeFcn)+1];
    strcpy(usertask->codeFcnMATLAB, codeFcn);
    
    mxArray* data;
    if (nrhs == 4) { // no data specified
      data = mxCreateDoubleMatrix(0, 0, mxREAL);
    } else {
      data = mxDuplicateArray((mxArray*)prhs[4]);
    }
    mexMakeArrayPersistent(data);
    usertask->dataMATLAB = data;
  }
}

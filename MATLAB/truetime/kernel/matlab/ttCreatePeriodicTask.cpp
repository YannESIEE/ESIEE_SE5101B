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

#include "../createpertask.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  rtsys = getrtsys() ; // Get pointer to rtsys 
  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs < 5 || nrhs > 6) {
    MEX_ERROR("ttCreatePeriodicTask: Wrong number of input arguments! \nUsage: ttCreatePeriodicTask(name, offset, period, priority, codefcn)\n       ttCreatePeriodicTask(name, offset, period, priority, codefcn, data)");
    return;
  }

  if (mxIsChar(prhs[0]) != 1) {
    MEX_ERROR("ttCreatePeriodicTask: name must be a string");
    return;
  }
  if (!mxIsDoubleScalar(prhs[1])) {
    MEX_ERROR("ttCreatePeriodicTask: offset must be a double scalar");
    return;
  }
  if (!mxIsDoubleScalar(prhs[2])) {
    MEX_ERROR("ttCreatePeriodicTask: period must be a double scalar");
    return;
  }
  if (!mxIsDoubleScalar(prhs[3])) {
    MEX_ERROR("ttCreatePeriodicTask: priority must be a double scalar");
    return;
  }
  if (mxIsChar(prhs[4]) != 1 || mxGetM(prhs[4]) != 1) {
    MEX_ERROR("ttCreatePeriodicTask: codeFcn must be a non-empty string");
    return;
  }
  
  char name[100];
  mxGetString(prhs[0], name, 100);

  double offset = *mxGetPr(prhs[1]);
  if (offset < -EPS) {
    offset = 0.0;
    printf("ttCreatePeriodicTask: negative offset changed to zero\n");
  }
  
  double period = *mxGetPr(prhs[2]);

  double priority = *mxGetPr(prhs[3]);

  char codeFcn[100];
  mxGetString(prhs[4], codeFcn, 100);

  // Make sure that the code function exists in Matlab path
  mxArray *lhs[1];
  mxArray *rhs[1];
  rhs[0] = mxDuplicateArray(prhs[4]);
  mexCallMATLAB(1, lhs, 1, rhs, "exist");
  int number = (int) *mxGetPr(lhs[0]);
  if (number == 0) {
    char buf[200];
    sprintf(buf, "ttCreatePeriodicTask: codeFcn '%s' not in path! Task '%s' not created!", codeFcn, name);
    MEX_ERROR(buf);
    return;
  }

  if (ttCreatePeriodicTask(name, offset, period, priority, NULL)) {

    // Add name of code function (m-file) and data variable
    DataNode *n = (DataNode*) rtsys->taskList->getLast();
    UserTask *usertask = (UserTask*) n->data;
    
    usertask->codeFcnMATLAB = new char[strlen(codeFcn)+1];
    strcpy(usertask->codeFcnMATLAB, codeFcn);
    
    mxArray* data;
    if (nrhs == 5) { // no data specified
      data = mxCreateDoubleMatrix(0, 0, mxREAL);
    } else {
      data = mxDuplicateArray((mxArray*)prhs[5]);
    }
    mexMakeArrayPersistent(data);
    usertask->dataMATLAB = data;
  }
}

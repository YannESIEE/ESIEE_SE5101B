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

#include "../createlog.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  rtsys = getrtsys() ; // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs != 4) {
    MEX_ERROR("ttCreateLog: Wrong number of input arguments!\nUsage: ttCreateLog(taskname, logtype, variable, size)");
    return;
  }

  if (mxIsChar(prhs[0]) != 1 || mxGetM(prhs[0]) != 1) {
    MEX_ERROR("ttCreateLog: taskname must be a non-empty string");
    return;
  }
  char task[100];
  mxGetString(prhs[0], task, 100);

  int type = 0;
  if (mxIsDoubleScalar(prhs[1])) {
    type = (int)*mxGetPr(prhs[1]);
  }
  if (type <= 0) {
    MEX_ERROR("ttCreateLog: type must be a positive number");
    return;
  }

  if (mxIsChar(prhs[2]) != 1 || mxGetM(prhs[2]) != 1) {
    MEX_ERROR("ttCreateLog: variable must be a non-empty string");
    return;
  }
  char variable[100];
  mxGetString(prhs[2], variable, 100);

  int size = 0;
  if (mxIsDoubleScalar(prhs[3])) {
    size = (int)*mxGetPr(prhs[3]);
  }
  if (size <= 0) {
    MEX_ERROR("ttCreateLog: size must be a positive number");
    return;
  }

  ttCreateLog(task,type,variable,size);
}

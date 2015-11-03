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

#include "../createtimer.cpp"
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
    MEX_ERROR("ttCreatePeriodicTimer: Wrong number of input arguments!\nUsage: ttCreatePeriodicTimer(timername, start, period, handlername)");
    return;
  }

  if (mxIsChar(prhs[0]) != 1) {
    MEX_ERROR("ttCreatePeriodicTimer: timername must be a string");
    return;
  }
  if (!mxIsDoubleScalar(prhs[1])) {
    MEX_ERROR("ttCreatePeriodicTimer: time must be a double scalar");
    return;
  }
  if (!mxIsDoubleScalar(prhs[2])) {
    MEX_ERROR("ttCreatePeriodicTimer: period must be a double scalar");
    return;
  }
  if (mxIsChar(prhs[3]) != 1) {
    MEX_ERROR("ttCreatePeriodicTimer: handlername must be a string");
    return;
  }
  
  char timername[100];
  mxGetString(prhs[0], timername, 100);

  double start = *mxGetPr(prhs[1]);

  double period = *mxGetPr(prhs[2]);
  
  char handlername[100];
  mxGetString(prhs[3], handlername, 100);

  ttCreatePeriodicTimer(timername, start, period, handlername);
}

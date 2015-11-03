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
  if (nrhs != 3) {
    MEX_ERROR("ttCreateTimer: Wrong number of input arguments!\nUsage: ttCreateTimer(timername, time, handlername)");
    return;
  }

  if (mxIsChar(prhs[0]) != 1) {
    MEX_ERROR("ttCreateTimer: timername must be a string");
    return;
  }
  if (!mxIsDoubleScalar(prhs[1])) {
    MEX_ERROR("ttCreateTimer: time must be a double scalar");
    return;
  }
  if (mxIsChar(prhs[2]) != 1) {
    MEX_ERROR("ttCreateTimer: handlername must be a string");
    return;
  }
  
  char timername[100];
  mxGetString(prhs[0], timername, 100);
  
  double time = *mxGetPr(prhs[1]);

  char handlername[100];
  mxGetString(prhs[2], handlername, 100);

  ttCreateTimer(timername, time, handlername);
}

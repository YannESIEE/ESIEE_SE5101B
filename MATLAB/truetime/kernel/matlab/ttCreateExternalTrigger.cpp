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

#include "../createtrigger.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  rtsys = getrtsys() ; // Get pointer to rtsys 
 
  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs != 2) {
    MEX_ERROR("ttCreateExternalTrigger: Wrong number of input arguments!\nUsage: ttCreateExternalTrigger(handlername, latency)");
    return;
  }

  if (mxIsChar(prhs[0]) != 1 || mxGetM(prhs[0]) != 1) {
    MEX_ERROR("ttCreateExternalTrigger: handlername must be a non-empty string");
    return;
  }
  if (!mxIsDoubleScalar(prhs[1])) {
    MEX_ERROR("ttCreateExternalTrigger: latency must be a double scalar");
    return;
  }
  
  char handlername[100];
  mxGetString(prhs[0], handlername, 100);
  
  double latency = *mxGetPr(prhs[1]);
  if (latency < -EPS) {
    printf("ttCreateExternalTrigger: negative latency, changed to zero\n");
    latency = 0.0;
  }

  ttCreateExternalTrigger(handlername, latency);
}

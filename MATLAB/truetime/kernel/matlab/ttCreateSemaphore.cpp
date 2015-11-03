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

#include "../createsemaphore.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  rtsys = getrtsys() ; // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs < 2 || nrhs > 3 ) {
    MEX_ERROR("ttCreateSemaphore: Wrong number of input arguments!\nUsage: ttCreateSemaphore(semaphorename, initval [, maxval])");
    return;
  }
  if (mxIsChar(prhs[0]) != 1) {
    MEX_ERROR("ttCreateSemaphore: semaphorename must be a string");
    return;
  }
  if (!mxIsDoubleScalar(prhs[1])) {
    MEX_ERROR("ttCreateSemaphore: initval must be an integer scalar");
    return;
  }

  if (nrhs == 3) {
    if (!mxIsDoubleScalar(prhs[2])) {
      MEX_ERROR("ttCreateSemaphore: maxval must be an integer scalar");
      return;
    }
  }

  char semaphorename[100];
  mxGetString(prhs[0], semaphorename, 100);
  int initval = (int) *mxGetPr(prhs[1]);
  if (nrhs == 2) {
    ttCreateSemaphore(semaphorename, initval);
  } else {
    int maxval = (int) *mxGetPr(prhs[2]);
    ttCreateSemaphore(semaphorename, initval, maxval);
  }
}

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

#include "../currenttime.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  rtsys = getrtsys() ; // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs > 1) {
    MEX_ERROR("ttCurrentTime: Wrong number of input arguments!\nUsage: ttCurrentTime or\n       ttCurrentTime(new time)");
    return;
  }

  if ( nrhs == 1 && !mxIsDouble(prhs[0])){
    MEX_ERROR("ttCurrentTime: new time must be a double");
    return;
  }

  double retval;
  if ( nrhs == 0){
    retval = ttCurrentTime();
  } else {
    double newTime = (double) *mxGetPr(prhs[0]);
    retval = ttCurrentTime(newTime);
  }

  plhs[0] = mxCreateDoubleScalar(retval);
}

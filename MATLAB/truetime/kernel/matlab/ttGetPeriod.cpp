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

#include "../getperiod.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  double retval; 
   
  rtsys = getrtsys() ; // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs > 1) {
    MEX_ERROR("ttGetPeriod: Wrong number of input arguments!\nUsage: ttGetPeriod or ttGetPeriod(taskname)");
    return;
  }

  if (nrhs == 0) {
    retval = ttGetPeriod(); 
    
    plhs[0] = mxCreateDoubleScalar(retval);

  } else {
    if (mxIsChar(prhs[0]) != 1 || mxGetM(prhs[0]) != 1) {
      MEX_ERROR("ttGetPeriod: taskname must be a string");
      return;
    }
    
    char taskname[100];
    mxGetString(prhs[0], taskname, 100);
    retval = ttGetPeriod(taskname); 
    
    plhs[0] = mxCreateDoubleScalar(retval);
    
  }

}

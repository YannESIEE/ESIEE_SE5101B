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

#include "../getabsdeadline.cpp"
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
    MEX_ERROR("ttGetAbsDeadline: Wrong number of input arguments!\nUsage: ttGetAbsDeadline or ttGetAbsDeadline(taskname)");
    return;
  }

  if (nrhs == 0) {
    retval = ttGetAbsDeadline(); 
    
    plhs[0] = mxCreateDoubleScalar(retval);

  } else {
    if (mxIsChar(prhs[0]) != 1 || mxGetM(prhs[0]) != 1) {
      MEX_ERROR("ttGetAbsDeadline: taskname must be a string");
      return;
    }
    
    char taskname[100];
    mxGetString(prhs[0], taskname, 100);
    retval = ttGetAbsDeadline(taskname); 
    
    plhs[0] = mxCreateDoubleScalar(retval);
    
  }

}

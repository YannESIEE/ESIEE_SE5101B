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

#include "../setwcet.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  double value; 
   
  rtsys = getrtsys() ; // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs == 0 || nrhs > 2) {
    MEX_ERROR("ttSetWCET: Wrong number of input arguments!\nUsage: ttSetWCET(value) or ttSetWCET(value, taskname)");
    return;
  }

  if (!mxIsDoubleScalar(prhs[0])) {
    MEX_ERROR("ttSetWCET: value must be a number");
    return;
  }
  
  if (nrhs == 1) {
    value = *mxGetPr(prhs[0]);
    ttSetWCET(value);
    
  } else {
    if (mxIsChar(prhs[1]) != 1 || mxGetM(prhs[1]) != 1) {
      MEX_ERROR("ttSetWCET: taskname must be a string");
      return;
    }
    
    value = *mxGetPr(prhs[0]);
    char taskname[100];
    mxGetString(prhs[1], taskname, 100);
    
    ttSetWCET(value, taskname);
  }

}

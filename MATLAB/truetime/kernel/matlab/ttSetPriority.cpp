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

#include "../setpriority.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  double prio; 

  rtsys = getrtsys() ; // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs == 0 || nrhs > 2) {
    MEX_ERROR("ttSetPriority: Wrong number of input argumenys!\nUsage: ttSetPriority(prio) or ttSetPriority(prio, taskname)");
    return;
  }

  if (!mxIsDoubleScalar(prhs[0])) {
    MEX_ERROR("ttSetPriority: prio must be a number");
    return;
  }
  
  if (nrhs == 1) {
    prio = *mxGetPr(prhs[0]);
    ttSetPriority(prio);
    
  } else {
    if (mxIsChar(prhs[1]) != 1 || mxGetM(prhs[1]) != 1) {
      MEX_ERROR("ttSetPriority: taskname must be a string");
      return;
    }
    
    prio = *mxGetPr(prhs[0]);
    char taskname[100];
    mxGetString(prhs[1], taskname, 100);
    
    ttSetPriority(prio, taskname);
  }
}

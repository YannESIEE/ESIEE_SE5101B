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

#include "../wait.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  rtsys = getrtsys() ; // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs != 1) {
    MEX_ERROR("ttWait: Wrong number of input arguments!\nUsage: ttWait(eventname)");
    return;
  }

  if (mxIsChar(prhs[0]) != 1) {
    MEX_ERROR("ttWait: eventname must be a string");
    return;
  }
  
  char eventname[100];
  mxGetString(prhs[0], eventname, 100);
  
  ttWait(eventname);

}

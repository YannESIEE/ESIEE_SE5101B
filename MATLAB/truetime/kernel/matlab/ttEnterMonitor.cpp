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

#include "../entermonitor.cpp"
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
    MEX_ERROR("ttEnterMonitor: Wrong number of input arguments!\nUsage: ttEnterMonitor(monitorname)");
    return;
  }

  if (mxIsChar(prhs[0]) != 1 || mxGetM(prhs[0]) != 1) {
    MEX_ERROR("ttEnterMonitor: monitorname must be a non-empty string");
    return;
  }
  
  char monitorname[100];
  mxGetString(prhs[0], monitorname, 100);
  
  ttEnterMonitor(monitorname);

}

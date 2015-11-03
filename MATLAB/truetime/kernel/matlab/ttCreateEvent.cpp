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

#include "../createevent.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  rtsys = getrtsys() ; // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs < 1 || nrhs > 2) {
    MEX_ERROR("ttCreateEvent: Wrong number of input arguments!\nUsage: ttCreateEvent(eventname) or ttCreateEvent(eventname, monitorname)");
    return;
  }

  if (mxIsChar(prhs[0]) != 1) {
    MEX_ERROR("ttCreateEvent: eventname must be a string");
    return;
  }
  
  if (nrhs == 2) {
    if (mxIsChar(prhs[1]) != 1) {
      MEX_ERROR("ttCreateEvent: monitorname must be a string");
      return;
    }  
  }

  char eventname[100];
  mxGetString(prhs[0], eventname, 100);

  if (nrhs == 1) {
    ttCreateEvent(eventname);
  } else {
    char monitorname[100];
    mxGetString(prhs[1], monitorname, 100);
    
    ttCreateEvent(eventname, monitorname);
  }
}


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

#include "../attachdlhandler.cpp"
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
    MEX_ERROR("ttAttachDLHandler: Wrong number of input arguments!\nUsage: ttAttachDLHandler(taskname, handlername)");
    return;
  }

  if (mxIsChar(prhs[0]) != 1 || mxGetM(prhs[0]) != 1) {
    MEX_ERROR("ttAttachDLHandler: taskname must be a non-empty string");
    return;
  }
  if (mxIsChar(prhs[1]) != 1 || mxGetM(prhs[1]) != 1) {
    MEX_ERROR("ttAttachDLHandler: handlername must be a non-empty string");
    return;
  }

  char taskname[100];
  char handlername[100];

  mxGetString(prhs[0], taskname, 100);
  mxGetString(prhs[1], handlername, 100);

  ttAttachDLHandler(taskname, handlername);
}

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

#include "../logstart.cpp"
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
    MEX_ERROR("ttLogStart: Wrong number of input arguments!\nUsage: ttLogStart(logID)");
    return;
  }

  int ID = 0;
  if (mxIsDoubleScalar(prhs[0])) {
    ID = (int)*mxGetPr(prhs[0]);
  }
  if (ID <= 0) {
    MEX_ERROR("ttLogStart: logID must be a positive number");
    return;
  }

  ttLogStart(ID);
}

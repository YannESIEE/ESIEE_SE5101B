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

#include "../analogout.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  rtsys = getrtsys(); // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments.
  if(nrhs != 2) {
    MEX_ERROR("ttAnalogOut: Wrong number of input arguments!\nUsage: ttAnalogOut(outpChan, value)");
    return;
  }

  if (!mxIsDoubleScalar(prhs[0])) {
    MEX_ERROR("ttAnalogOut: outpChan must be a number");
    return;
  }

  if (!mxIsDoubleScalar(prhs[1])) {
    MEX_ERROR("ttAnalogOut: value must be a number");
    return;
  }

  int outpChan = (int) *mxGetPr(prhs[0]);
  double value = *mxGetPr(prhs[1]);

  ttAnalogOut(outpChan, value);
}


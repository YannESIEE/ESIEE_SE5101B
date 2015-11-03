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

#include "../setkernelparameter.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  double value;
  char parametername[100];

  rtsys = getrtsys(); // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }
  // Check number and type of arguments. 
  if (nrhs < 2 || nrhs > 2) {
    MEX_ERROR("ttSetKernelParameter: Wrong number of input arguments!\nUsage: ttSetKernelParameter(parameter, value)");
    return;
  }
  if (mxIsChar(prhs[0]) != 1) {
    MEX_ERROR("ttSetKernelParameter: parameter name must be a string");
    return;
  }
  mxGetString(prhs[0], parametername, 100);

  if (!mxIsDouble(prhs[1])){
    MEX_ERROR("ttSetKernelParameter: value must be a double");
    return;
  }
  value = (double)*mxGetPr(prhs[1]);
  //printf("%s, %f\n", parametername, value);
  ttSetKernelParameter(parametername, value);
  //printf("%s:%d\n", __FILE__,__LINE__);
}

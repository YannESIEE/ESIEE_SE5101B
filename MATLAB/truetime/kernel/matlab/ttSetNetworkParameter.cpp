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

#include "../setnetworkparameter.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  int network;
  double value;
  char parametername[100];

  rtsys = getrtsys(); // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }
  // Check number and type of arguments. 
  if (nrhs < 2 || nrhs > 3) {
    MEX_ERROR("ttSetNetworkParameter: Wrong number of input arguments!\nUsage: ttSetNetworkParameter(parameter, value) or\n       ttSetNetworkParameter(network, parameter, value)");
    return;
  }
  if (mxIsChar(prhs[nrhs-2]) != 1) {
    MEX_ERROR("ttSetNetworkParameter: parameter name must be a string");
    return;
  }

  mxGetString(prhs[nrhs-2], parametername, 100);
  if (nrhs == 2){ // no network specified
    if (!mxIsDouble(prhs[1])){
      MEX_ERROR("ttSetNetworkParameter: value must be a double");
      return;
    }
    network = 1;
    value = (double)*mxGetPr(prhs[1]);
  } else { // the network is specified
    if (!mxIsDouble(prhs[0]) || !mxIsDouble(prhs[2])){
      MEX_ERROR("ttSetNetworkParameter: network must be a double\n                       value must be a double");
      return;
    }
    network = (int)*mxGetPr(prhs[0]);
    value = (double)*mxGetPr(prhs[2]);
  }
  //printf("%d, %s, %f\n", network, parametername, value);
  ttSetNetworkParameter(network, parametername, value);
  //printf("%s:%d\n", __FILE__,__LINE__);
}

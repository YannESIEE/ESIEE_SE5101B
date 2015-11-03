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

#include "../getmsg.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  mxArray* data;
  int network;

  rtsys = getrtsys(); // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs > 1) {
    MEX_ERROR("ttGetMsg: Wrong number of input arguments!\nUsage: ttGetMsg or\n       ttGetMsg(network)");
    return;
  }
  
  if (nrhs == 1) {
    if (!mxIsDoubleScalar(prhs[0])) {
      MEX_ERROR("ttGetMsg: network must be an integer scalar");
      return;
    }
    network = (int) *mxGetPr(prhs[0]);
  } else {
    network = 1;
  }

  double signalPower;
  data = ttGetMsgMATLAB(network, &signalPower);

  if ( data==NULL ){
    data = mxCreateDoubleMatrix(0,0,mxREAL); // Return empty matrix
  }

   plhs[0] = data;
   if ( nlhs>=2 ){
     plhs[1] = mxCreateDoubleScalar(signalPower);
   }
}

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

#include "../discardunsent.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  int network;

  rtsys = getrtsys(); // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number of arguments.
  if (nrhs > 1) {
    MEX_ERROR("ttDiscardUnsentMessages: Wrong number of input arguments!\nUsage: ttDiscardUnsentMessages() or\n       ttDiscardUnsentMessages(network)");
    return;
  }

  int nbr;

  if (nrhs > 0) {
    if (!mxIsDoubleScalar(prhs[0])) {
      MEX_ERROR("ttSendMsg: network must be a number");
      return;
    }
    network = (int)*mxGetPr(prhs[0]);
    nbr = ttDiscardUnsentMessages(network);
  }
  else
    // default network (1)
    nbr = ttDiscardUnsentMessages();


  //  if ( nlhs>=1 ){
    plhs[0] = mxCreateDoubleScalar(nbr);
    //}


}

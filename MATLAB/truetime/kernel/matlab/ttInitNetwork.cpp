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

#include "../initnetwork.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  int LAN, node;
  char nwhandler[100];

    rtsys = getrtsys(); // Get pointer to rtsys 

  // Check number and type of arguments. 
  if (nrhs < 2 || nrhs > 3) {
    mexErrMsgTxt("ttInitNetwork: Wrong number of input arguments!\nUsage: ttInitNetwork(network, node, nwhandler) or\n       ttInitNetwork(node, nwhandler)");
  }

  if (nrhs == 3) {
    
    if (!mxIsDoubleScalar(prhs[0])) {
      mexErrMsgTxt("ttInitNetwork: network must be an integer");
    }
    if (!mxIsDoubleScalar(prhs[1])) {
      mexErrMsgTxt("ttInitNetwork: node must be an integer");
    }
    if (mxIsChar(prhs[2]) != 1 || mxGetM(prhs[2]) != 1) {
      mexErrMsgTxt("ttInitNetwork: nwhandler must be a string");
    }
    
    LAN = (int) *mxGetPr(prhs[0]);
    node = (int) *mxGetPr(prhs[1]);
    
    mxGetString(prhs[2], nwhandler, 100);
    
    ttInitNetwork(LAN, node, nwhandler);
    
  } else {
    
    if (!mxIsDoubleScalar(prhs[0])) {
      mexErrMsgTxt("ttInitNetwork: node must be an integer");
    }
    if (mxIsChar(prhs[1]) != 1 || mxGetM(prhs[1]) != 1) {
      mexErrMsgTxt("ttInitNetwork: nwhandler must be a string");
    }
  
    node = (int) *mxGetPr(prhs[0]);
  
    mxGetString(prhs[1], nwhandler, 100);

    ttInitNetwork(node, nwhandler);
  }
}


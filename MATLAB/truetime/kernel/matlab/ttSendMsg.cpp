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

#include "../sendmsg.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  int network, receiver, length;
  mxArray *data;
  double priority = -INF;

  rtsys = getrtsys(); // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs < 3 || nrhs > 4) {
    MEX_ERROR("ttSendMsg: Wrong number of input arguments!\nUsage: ttSendMsg(receiver, data, length) or\n       ttSendMsg(receiver, data, length, priority) or\n       ttSendMsg([network receiver], data, length) or\n       ttSendMsg([network receiver], data, length, priority)");
    return;
  }

  if (mxIsDoubleScalar(prhs[0])) { // no network specified
    network = 1;
    receiver = (int)*mxGetPr(prhs[0]);
  } else if (mxIsDouble(prhs[0]) && !mxIsComplex(prhs[0]) && mxGetM(prhs[0])==1 && mxGetN(prhs[0])==2) {
    network = (int)*mxGetPr(prhs[0]);
    receiver = (int)*(mxGetPr(prhs[0])+1);
  } else {
    MEX_ERROR("ttSendMsg: receiver must be a number or a vector [network receiver]");
    return;
  }

  data = mxDuplicateArray(prhs[1]);
  mexMakeArrayPersistent(data);

  if (!mxIsDoubleScalar(prhs[2])) {
    MEX_ERROR("ttSendMsg: length must be a number");
    return;
  }
  length = (int)*mxGetPr(prhs[2]);
  if (nrhs == 4) {
    if(!mxIsDoubleScalar(prhs[3])) {
      MEX_ERROR("ttSendMsg: priority must be a number");
      return;
    }
    priority = *mxGetPr(prhs[3]);
  }
 
  if (priority == -INF) { 
    ttSendMsgMATLAB(network, receiver, length, data);
  } else {
    ttSendMsgMATLAB(network, receiver, length, data, priority);
  }
}

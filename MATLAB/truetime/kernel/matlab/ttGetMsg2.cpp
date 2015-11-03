/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#define KERNEL_MATLAB
#include "../ttnetwork.h" 

mxArray *ttGetMsgMATLAB(int networkNbr, int receiver, double* signalPower);

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  int network, receiver, trigger;

  // Check number and type of arguments. 
  if (nrhs != 3 ) {
    printf("ttGetMsg: Wrong number of input arguments!\nUsage: ttGetMsg(network, receiver, trigger)");
    return;
  }
  
  if (!mxIsDoubleScalar(prhs[2])) {
    printf("ttGetMsg: trigger must be an integer scalar");
    return;
  }
  trigger = (int) *mxGetPr(prhs[2]);
  // At time 0, the block is executed once to get an initial output signal.
  // But we do not want to send any messages at this time, unless the trigger
  // is really activated.
  if ( trigger==0 ){
    plhs[0] = mxCreateDoubleScalar(0);
    return;
  }

  if (!mxIsDoubleScalar(prhs[0])) {
    printf("ttGetMsg: network must be an integer scalar");
    return;
  }
  network = (int) *mxGetPr(prhs[0]);

  if (!mxIsDoubleScalar(prhs[1])) {
    printf("ttGetMsg: receiver must be an integer scalar");
    return;
  }
  receiver = (int) *mxGetPr(prhs[1]);

  double signalPower;
  plhs[0] = ttGetMsgMATLAB(network, receiver, &signalPower);

  if ( nlhs>=2 ){
    plhs[1] = mxCreateDoubleScalar(signalPower);
  }
}


// do the dirty work: poke around inside nwsys of network block 
NWmsg *nwGetMsg(RTnetwork* nwsys, int nodeNbr) {
  NWmsg *m;
  if ((m = (NWmsg *)nwsys->nwnodes[nodeNbr]->postprocQ->getFirst()) != NULL) {
    nwsys->nwnodes[nodeNbr]->postprocQ->removeNode(m);
  }
  return m;
}

// For use from Matlab mex function
//char *ttGetMsgMATLAB(int networkNbr)
mxArray *ttGetMsgMATLAB(int networkNbr, int receiver, double* signalPower)
{
  RTnetwork *nwsys;
  char nwsysbuf[100];

  sprintf(nwsysbuf, "_nwsys_%d", networkNbr);
  mxArray *var = (mxArray*)mexGetVariablePtr("global", nwsysbuf);
  if (var == NULL) {
    printf("_nwsys_%d not found!\n", networkNbr);
    return 0;
  }
  nwsys = (RTnetwork *)(*((long *)mxGetPr(var)));

  if (receiver < 0 || receiver > nwsys->nbrOfNodes) {
    printf("receiver number out of bounds.\n");
    return 0;
  }

  NWmsg *nwmsg = nwGetMsg(nwsys, receiver-1);
  if (nwmsg == NULL) {
    *signalPower = 0;
    //printf("Error in %s, line:%d\n", __FUNCTION__, __LINE__);
    return NULL;
  }
  mxArray* data = mxDuplicateArray(nwmsg->dataMATLAB);
  *signalPower = nwmsg->signalPower;

  // Delete message 
  mxDestroyArray(nwmsg->dataMATLAB);
  delete nwmsg;

  return data;
}

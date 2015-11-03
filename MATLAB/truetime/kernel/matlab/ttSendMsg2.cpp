/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#define KERNEL_MATLAB
#include "../ttnetwork.h"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  int networkNbr, sender, receiver, length, trigger;
  mxArray *data;
  double priority;
  RTnetwork *nwsys;

  // Check number and type of arguments. 
  if (nrhs != 7) {
    printf("ttSendMsg2(network, sender, receiver, data, length, priority, trigger)\n");
    return;
  }


  if (mxIsDoubleScalar(prhs[6])) {
    trigger = (int)*mxGetPr(prhs[6]);
  } else {
    printf("ttSendMsg: trigger must be a number.\n");
    return;
  }
  // At time 0, the block is executed once to get an initial output signal.
  // But we do not want to send any messages at this time, unless the trigger
  // is really activated.
  if ( trigger==0 ){
    plhs[0] = mxCreateDoubleScalar(0);
    return;
  }

  if (mxIsDoubleScalar(prhs[0])) {
    networkNbr = (int)*mxGetPr(prhs[0]);
  } else {
    printf("ttSendMsg: network must be a number.\n");
    return;
  }
  // Get the network pointer
  char nwsysbuf[100];
  sprintf(nwsysbuf, "_nwsys_%d", networkNbr);
  mxArray *var = (mxArray*)mexGetVariablePtr("global", nwsysbuf);
  if (var == NULL) {
    printf("_nwsys_%d not found!\n", networkNbr);
    return;
  }
  nwsys = (RTnetwork *)(*((long *)mxGetPr(var)));

  if (mxIsDoubleScalar(prhs[1])) {
    sender = (int)*mxGetPr(prhs[1]);
  } else {
    printf("ttSendMsg: sender must be a number.\n");
    return;
  }
  if (sender < 0 || sender > nwsys->nbrOfNodes) {
    printf("sender number out of bounds.\n");
    return;
  }

  if (mxIsDoubleScalar(prhs[2])) {
    receiver = (int)*mxGetPr(prhs[2]);
  } else {
    printf("ttSendMsg: receiver must be a number.\n");
    return;
  }
  if (receiver < -1 || receiver > nwsys->nbrOfNodes) {
    printf("receiver number out of bounds.\n");
    return;
  }

  data = mxDuplicateArray(prhs[3]);
  mexMakeArrayPersistent(data);
  //printf("%d\n",(int)*mxGetPr(prhs[3]));

  if (mxIsDoubleScalar(prhs[4])) {
    length = (int)*mxGetPr(prhs[4]);
  } else {
    printf("ttSendMsg: length must be a number.\n");
    return;
  }
  if (length <= 0) {
    printf("length must be a positive number.\n");
    return;
  }

  if (mxIsDoubleScalar(prhs[5])) {
    priority = *mxGetPr(prhs[5]);
  } else {
    printf("ttSendMsg: priority must be a number.\n");
    return;
  }

  NWmsg *nwmsg = new NWmsg();
  nwmsg->sender = sender-1;
  nwmsg->receiver = receiver-1;
  nwmsg->data = NULL;
  nwmsg->dataMATLAB = data;
  nwmsg->length = length;
  nwmsg->prio = priority;

  // do the dirty work: poke around inside nwsys of the network block 
  // set time when finished preprocessing
  nwmsg->waituntil = nwsys->time + nwsys->nwnodes[nwmsg->sender]->predelay;
  nwmsg->collided = 0; // This message has not collided (802.11)

  // enqueue message in preprocQ
  nwsys->nwnodes[nwmsg->sender]->preprocQ->appendNode(nwmsg);

  // Trigger the network block
  if (nwsys->inputs[sender-1] == 0.0) {
    plhs[0] = mxCreateDoubleScalar(1.0);
  } else {
    plhs[0] = mxCreateDoubleScalar(0.0);
  }
}

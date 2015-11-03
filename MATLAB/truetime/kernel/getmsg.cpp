/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef GET_MSG
#define GET_MSG

#include "getnetwork.cpp"

// do the dirty work: poke around inside nwsys of network block 
NWmsg *nwGetMsg(Network* net) {
  NWmsg *m;
  if ((m = (NWmsg *)net->nwsys->nwnodes[net->nodeNbr]->postprocQ->getFirst()) != NULL) {
    net->nwsys->nwnodes[net->nodeNbr]->postprocQ->removeNode(m);
  }
  return m;
}

void *ttGetMsg(int networkNbr, double* signalPower)
{
  void *data;
  Network* net = getNetwork(networkNbr);

  if (net == NULL) {
    // is there no network?
    char buf[200];
    sprintf(buf, "ttGetMsg: Network #%d not present!", networkNbr);
    MEX_ERROR(buf);
    return NULL;
  }

  NWmsg *nwmsg = nwGetMsg(net); 
  if (nwmsg == NULL) {
    *signalPower = 0;
    return NULL;
  }
  data = nwmsg->data;
  *signalPower = nwmsg->signalPower;

  // Delete message 
  delete nwmsg;

  return data;
}

void *ttGetMsg(int networkNbr) // returns data (void *) or NULL if no message
{
  void *data;
  Network* net = getNetwork(networkNbr);

  if (net == NULL) {
    // is there no network?
    char buf[200];
    sprintf(buf, "ttGetMsg: Network #%d not present!", networkNbr);
    MEX_ERROR(buf);
    return NULL;
  }

  NWmsg *nwmsg = nwGetMsg(net); 
  if (nwmsg == NULL) {
    return NULL;
  }
  data = nwmsg->data;
  // Delete message 
  delete nwmsg;

  return data;
}

void *ttGetMsg() {
  return ttGetMsg(1);
}

// For use from Matlab mex function
mxArray *ttGetMsgMATLAB(int networkNbr, double* signalPower)
{
  Network* net = getNetwork(networkNbr);
  
  if (net == NULL) {
    // is there no network?
    char buf[200];
    sprintf(buf, "ttGetMsg: Network #%d not present!", networkNbr);
    MEX_ERROR(buf);
    return NULL;
  }

  NWmsg *nwmsg = nwGetMsg(net);
  if (nwmsg == NULL) {
    *signalPower = 0;
    return NULL;
  }
  *signalPower = nwmsg->signalPower;
  mxArray *data = mxDuplicateArray(nwmsg->dataMATLAB);

  // Delete message
  mxDestroyArray(nwmsg->dataMATLAB);
  delete nwmsg;

  return data;
}



#endif

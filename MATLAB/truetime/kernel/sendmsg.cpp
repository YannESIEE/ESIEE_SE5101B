/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef SEND_MSG
#define SEND_MSG

#include "getnetwork.cpp"

// do the dirty work: poke around inside nwsys of the network block 
void nwSendMsg(NWmsg *nwmsg, RTnetwork *nwsys) {
  // set time when finished preprocessing
  nwmsg->waituntil = (rtsys->time- rtsys->clockOffset) / rtsys->clockDrift
    + nwsys->nwnodes[nwmsg->sender]->predelay;
  nwmsg->collided = 0; // This message has not collided (802.11)

  // enqueue message in preprocQ
  nwsys->nwnodes[nwmsg->sender]->preprocQ->appendNode(nwmsg);
}


void ttSendMsg(int receiver, void *data, int length);
void ttSendMsg(int receiver, void *data, int length, int priority);
void ttSendMsg(int networkNbr, int receiver, void *data, int length);
void ttSendMsg(int networkNbr, int receiver, void *data, int length, int priority);

void ttSendMsg(int receiver, void *data, int length)
{
  ttSendMsg(1, receiver, data, length);
}

void ttSendMsg(int networkNbr, int receiver, void *data, int length)
{
  Network* net = getNetwork(networkNbr);
  if (net == NULL) {
    char buf[200];
    sprintf(buf, "ttSendMsg: Network #%d not present!", networkNbr);
    MEX_ERROR(buf);
    return;
  }
  int priority = net->nodeNbr;
  ttSendMsg(networkNbr, receiver, data, length, priority);
}

void ttSendMsg(int receiver, void *data, int length, int priority)
{
  ttSendMsg(1, receiver, data, length, priority);
}

void ttSendMsg(int networkNbr, int receiver, void *data, int length, int priority)
{
  Network* net = getNetwork(networkNbr);
  if (net == NULL) {
    char buf[200];
    sprintf(buf, "ttSendMsg: Network #%d not present!", networkNbr);
    MEX_ERROR(buf);
    return;
  }
  
  if (receiver < 0 || receiver > net->nwsys->nbrOfNodes) {
    MEX_ERROR("ttSendMsg: receiver number out of bounds!");
    return;
  }

  NWmsg *nwmsg = new NWmsg();
  nwmsg->sender = net->nodeNbr;
  nwmsg->receiver = receiver-1;
  nwmsg->data = data;
  nwmsg->dataMATLAB = NULL;
  nwmsg->length = length;
  nwmsg->prio = priority;

  nwSendMsg(nwmsg, net->nwsys);
  if (rtsys->nwSnd[net->networkID - 1] == 0.0) {
    rtsys->nwSnd[net->networkID - 1] = 1.0; // trigger output
  } else {
    rtsys->nwSnd[net->networkID - 1] = 0.0; // trigger output
  }

}

// For use from Matlab mex function
void ttSendMsgMATLAB(int networkNbr, int receiver, int length, mxArray* dataMATLAB, double priority)
{
  Network* net = getNetwork(networkNbr);

  if (net == NULL) {
    // is there no network?
    char buf[200];
    sprintf(buf, "ttSendMsg: Network #%d not present!", networkNbr);
    MEX_ERROR(buf);
    return;
  }

  if (receiver < 0 || receiver > net->nwsys->nbrOfNodes) {
    MEX_ERROR("ttSendMsg: receiver number out of bounds");
    return;
  }

  NWmsg *nwmsg = new NWmsg();
  nwmsg->sender = net->nodeNbr;
  nwmsg->receiver = receiver-1;
  nwmsg->data = NULL;
  nwmsg->dataMATLAB = dataMATLAB;
  nwmsg->length = length;
  nwmsg->prio = priority;
  
  nwSendMsg(nwmsg, net->nwsys);
  if (rtsys->nwSnd[net->networkID - 1] == 0.0) {
    rtsys->nwSnd[net->networkID - 1] = 1.0; // trigger output
  } else {
    rtsys->nwSnd[net->networkID - 1] = 0.0; // trigger output
  }
}
 
void ttSendMsgMATLAB(int networkNbr, int receiver, int length, mxArray* dataMATLAB)
{
  Network* net = getNetwork(networkNbr);
  if (net == NULL) {
    // is there no network?
    char buf[200];
    sprintf(buf, "ttSendMsg: Network #%d not present!", networkNbr);
    MEX_ERROR(buf);
    return;
  }
  int priority = net->nodeNbr;
  ttSendMsgMATLAB(networkNbr, receiver, length, dataMATLAB, priority);
}

#endif

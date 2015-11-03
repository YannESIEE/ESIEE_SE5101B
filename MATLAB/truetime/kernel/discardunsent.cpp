/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef DISCARD_UNSENT
#define DISCARD_UNSENT

#include "getnetwork.cpp"

// discard messages from queue, beginning from given message
int deleteMessages(NWmsg *msg) {
  NWmsg *next;
  int nbr = 0;

  while( msg ){
    next = (NWmsg *)msg->getNext();
    msg->remove();

#ifndef KERNEL_C
    mxDestroyArray(msg->dataMATLAB);
#endif
    delete(msg);

    nbr++;
    msg = next;
  }
  return nbr;
}

// declare two versions
int ttDiscardUnsentMessages();
int ttDiscardUnsentMessages(int networkNbr);

int ttDiscardUnsentMessages()
{
  Network* net = getNetwork(1);
  if (net == NULL) {
    MEX_ERROR("ttDiscardUnsentMessages: Network not present!");
    return -1;
  }
  return ttDiscardUnsentMessages(1);
}

int ttDiscardUnsentMessages(int networkNbr)
{
  Network* net = getNetwork(networkNbr);
  if (net == NULL) {
    char buf[200];
    sprintf(buf, "ttSendMsg: Network #%d not present!", networkNbr);
    MEX_ERROR(buf);
    return -1;
  }

  RTnetwork *nwsys = net->nwsys;
  // get own network node handle
  NWnode *sender = nwsys->nwnodes[net->nodeNbr];

  int nbr;
  // discard all messages from preprocQ
  nbr = deleteMessages((NWmsg *)sender->preprocQ->getFirst());

  // The wireless network does not use the inputQ in the same way as the wired.
  if ( nwsys->type == CSMACD || nwsys->type == CSMAAMP || nwsys->type == RR ||
       nwsys->type == FDMA || nwsys->type == TDMA || nwsys->type == SFDSE){
    NWmsg *msg = (NWmsg *)sender->inputQ->getFirst();
    // is the first inputQ message just being sent?
    if (nwsys->sending == net->nodeNbr) {
      // yes, discard all but the first
      if (msg != NULL)
	nbr += deleteMessages((NWmsg *)msg->getNext());
    }
    else
      // no, discard whole inputQ
      nbr += deleteMessages(msg);
  }
  return nbr;
}

#endif

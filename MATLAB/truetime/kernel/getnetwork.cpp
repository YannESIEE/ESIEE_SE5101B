/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef GET_NETWORK
#define GET_NETWORK

Network *getNetwork(int networkNbr) {

  InterruptHandler* hdl;

  DataNode* dn = (DataNode*) rtsys->networkList->getFirst();
  while (dn != NULL) {
    hdl = (InterruptHandler*) dn->data;
    if (hdl->network->networkNbr == networkNbr) {
      break;
    }
    dn = (DataNode*) dn->getNext();
  }
  if (dn == NULL)
    return NULL;
  else {
    return hdl->network;
  }
}

#endif

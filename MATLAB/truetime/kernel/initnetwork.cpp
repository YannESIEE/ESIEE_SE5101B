/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef INIT_NETWORK
#define INIT_NETWORK 

#include "getnode.cpp"
#include "hdlerror.cpp"

// Register that there is a network but delay some initialization
void ttInitNetwork(int LAN, int node, char *nwhandler) {

  DataNode* dn = getNode(nwhandler, rtsys->handlerList);
  if (dn==NULL) {
    char buf[200];
    sprintf(buf, "ttInitNetwork: Non-existent interrupt handler '%s'! Can't init network!", nwhandler);
    MEX_ERROR(buf);
    return;
  }
  if (LAN < 1) {
    MEX_ERROR("ttInitNetwork: Network number must be positive!");
    return;
  }
  if (node < 1) {
    MEX_ERROR("ttInitNetwork: Node number must be positive!");
    return;
  }

  InterruptHandler* hdl = (InterruptHandler*) dn->data;

  if (hdl->type == UNUSED) {
    Network* network = new Network;
    network->networkNbr = LAN; // there is a network
    network->networkID = rtsys->nbrOfNetworks + 1;
    network->nodeNbr = node-1;
    
    hdl->network = network;
    hdl->type = NETWORK;
  
    rtsys->networkList->appendNode(new DataNode(hdl, ""));
    rtsys->nbrOfNetworks++;
  } else {
    // Handler already assigned, display error message
    handler_error(hdl, "ttInitNetwork:");
  }
}

// If there is only one network
void ttInitNetwork(int node, char *nwhandler)
{
  ttInitNetwork(1, node, nwhandler);
}

#endif

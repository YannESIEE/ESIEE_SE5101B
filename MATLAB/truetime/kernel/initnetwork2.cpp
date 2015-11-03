/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef INIT_NETWORK2
#define INIT_NETWORK2

// returns 0 if successful, -1 otherwise
int ttInitNetwork2()
{
  int i;
  char buf[100];
  mxArray *lhs1[1];
  mxArray *lhs2[1];
  mxArray *rhs[11];
  void *nwsysp;
  
  DataNode *dn;
  InterruptHandler* hdl;
  Network* network;
  
  mexCallMATLAB(1, lhs1, 0, NULL, "bdroot"); // Get model name

  // find an S-function block with the specified name
  rhs[0] = lhs1[0];
  rhs[1] = mxCreateString("LookUnderMasks");
  rhs[2] = mxCreateString("all");
  rhs[3] = mxCreateString("FollowLinks");
  rhs[4] = mxCreateString("on");
  rhs[5] = mxCreateString("RegExp");
  rhs[6] = mxCreateString("on");
  rhs[7] = mxCreateString("Name");  
  rhs[8] = mxCreateString("ttw*network");
  rhs[9] = mxCreateString("BlockType");
  rhs[10] = mxCreateString("S-Function");
  mexCallMATLAB(1, lhs1, 11, rhs, "find_system");
  
  if (mxIsEmpty(lhs1[0])) {
    MEX_ERROR("ttInitNetwork: Can't find any network blocks!");
    return -1;
  }

  dn = (DataNode*) rtsys->networkList->getFirst();
  while (dn!=NULL) {
    hdl = (InterruptHandler*) dn->data;
    network = hdl->network;
    // Assign TrueTime network pointer 
    i = 0;
    bool found = false;
    while (!found) {
      // Go through the network blocks
      rhs[0] = mxGetCell(lhs1[0], i);
      rhs[1] = mxCreateString("UserData");
      mexCallMATLAB(1, lhs2, 2, rhs, "get_param");
      if (mxIsChar(lhs2[0]) != 1 || mxGetM(lhs2[0]) != 1) {
	MEX_ERROR("UserData is not a string.");
	return -1;
      }
      mxGetString(lhs2[0], buf, 100);
      sscanf(buf, "%p", &nwsysp);
      
      if (nwsysp == NULL) {
	MEX_ERROR("ttInitNetwork: UserData of network block is not a pointer\n");
	return -1;
      }
      
      RTnetwork *tt_network = (RTnetwork*) nwsysp;
      if (tt_network->networkNbr == network->networkNbr) {
	network->nwsys = tt_network;
	// if ttSetNetworkParameter() was called from the init_function
	if (network->transmitpower != -INF) {
	  network->nwsys->nwnodes[network->nodeNbr]->transmitPower = 
	    network->transmitpower;
	  //printf("transmitpower is set to %.2f mW in node %d from the initfunction\n",
	  // network->nwsys->nwnodes[network->nodeNbr]->transmitPower*1000,
	  // network->nodeNbr+1);
	}
	if (network->predelay != -INF) {
	  network->nwsys->nwnodes[network->nodeNbr]->predelay = 
	    network->predelay;
	}
	if (network->postdelay != -INF) {
	  network->nwsys->nwnodes[network->nodeNbr]->postdelay = 
	    network->postdelay;
	}
	found = true;
      }
      i++; // next network string
    }
    
    dn = (DataNode*) dn->getNext();
  }
  return 0; // Everything is OK
}

#endif



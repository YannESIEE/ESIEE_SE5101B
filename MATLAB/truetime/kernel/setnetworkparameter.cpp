/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef SET_NETWORK_PARAMETER
#define SET_NETWORK_PARAMETER

#include "getnetwork.cpp"

// do the dirty work: poke around inside nwsys of the network block 
/**
 * Note, network->nwsys is set up in initnetwork2, which is run in mdlStart
 * of the kernel block. Therefore, if nwsys is not set up we can not set the
 * values but must instead save them so that they can be set correctly when
 * initnetwork2 is called. What is stated above is only necessary when calling
 * ttSetNetworkParameter from the initfunction, not the code function.
 */
void nwSetNetworkParameter(Network* network, char* parameter, double value) 
{
  if (strcmp(parameter, "transmitpower")==0){
    // Convert dbm to Watt
    double Watt = pow(10.0, value/10)/1000;
    if (network->nwsys == 0){        // initnetwork2 has not run yet
      network->transmitpower = Watt; // saved temporary, set in initnetwork2
    } else {
      // internal representation is in Watt
      network->nwsys->nwnodes[network->nodeNbr]->transmitPower = Watt;
      //printf("transmitpower is set to %.2f dbm <==> %.2e mW in node %d\n", 
      //     value, 
      //     network->nwsys->nwnodes[network->nodeNbr]->transmitPower*1000, 
      //     network->nodeNbr+1);
    }
  } else if ( strcmp(parameter, "predelay") == 0 ){
    if (network->nwsys == 0){    // initnetwork2 has not run yet
      network->predelay = value; // saved temporary, set in initnetwork2
    } else {
      network->nwsys->nwnodes[network->nodeNbr]->predelay = value;
    }
  } else if (strcmp(parameter, "postdelay")==0){
    if (network->nwsys == 0){     // initnetwork2 has not run yet
      network->postdelay = value; // saved temporary, set in initnetwork2
    } else {
      network->nwsys->nwnodes[network->nodeNbr]->postdelay = value;
    }
  } else {
    printf("Configuration of parameter %s is not implemented\n", parameter);
  }
}

void ttSetNetworkParameter(int networkNbr, char* parameter, double value)
{
  Network* net = getNetwork(networkNbr);
  if (net == NULL) {
    char buf[200];
    sprintf(buf, "ttSendMsg: Network #%d not present!", networkNbr);
    MEX_ERROR(buf);
    return;
  }
  //printf("%d\n",(int)net);
  nwSetNetworkParameter(net, parameter, value);
}

void ttSetNetworkParameter(char* parameter, double value)
{
  ttSetNetworkParameter(1, parameter, value);
}

#endif

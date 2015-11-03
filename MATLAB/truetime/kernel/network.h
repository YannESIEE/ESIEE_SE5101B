/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef NETWORK_H
#define NETWORK_H

/**
 * There is one "Network" for every network interface
 */
class Network {
 public:
  int networkNbr; // global network identifier
  int networkID;  // network ID (used for the input 
                  // and output trigger signals)
  int nodeNbr;    // node identifier in the specific network
  RTnetwork *nwsys;
  double transmitpower; // used to buffer the value in ttSetNetworkParameter(
                        // until the network is set up properly
  double predelay;      // Same as above
  double postdelay;      // Same as above
  Network();
};

Network::Network() {

	nwsys = 0; // So that we later on can be sure if initnetwork2 
	           //has been run or not
	transmitpower = -INF; // temporary variable, set in initnetwork2
	predelay = -INF;      // temporary variable, set in initnetwork2
	postdelay = -INF;     // temporary variable, set in initnetwork2
}

#endif

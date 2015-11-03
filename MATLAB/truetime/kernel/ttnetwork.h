/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#include "mex.h"
#include <string.h>
#include "mexhelp.h"
#include "linkedlist.cpp"

#include <math.h>

#define max(A,B) ((A) > (B) ? (A) : (B))
#define min(A,B) ((A) < (B) ? (A) : (B))

// When adding a new network protocol, also add the correct code to
// discardunsent.cpp
enum { CSMACD, CSMAAMP, RR, FDMA, TDMA, SFDSE, _802_11, _802_15_4};

enum { COMMONBUF, OUTPUTBUF };

enum { BUFFULLRETRY, BUFFULLDROP };

#define COLLISION_WINDOW 0.000001  // 1 us
#define ETHERNET_MINFRAMESIZE 64

// 802.11
enum { W_IDLE, W_SENDING, W_WAITING };
enum { UNICAST, BROADCAST };
#define SLOTTIME_802_11 0.00002 // 20 us
//#define SIFS_802_11 0.00001 // 10 us
//#define PIFS_802_11 0.00003 // 30 us
#define DIFS_802_11 0.00005 // 50 us
//31=2^5-1
#define CWMIN_802_11 5
#define CWMAX_802_11 1023
#define BACKGROUND_NOISE 8.9e-14


class NWmsg : public Node {
 public:
  int sender;
  int receiver;
  void *data;       // message content
  mxArray* dataMATLAB;
  int length;       // in bits - determines the transmission time
  double prio;
  double waituntil;    // when wait in pre/postprocQ has finished
  double remaining;    // remaining nbr of bits to be transmitted
  double collided;     // (802.11), 0 if not collided else 1
  double maximum_disturbance; // (802.11) also includes the own sending power
  int type;            // (802.11), set to BROADCAST for broadcast messages
  double signalPower;  // (802.11) what was the received signal power
  int nbrOfBackoffs; // (802.15.4)
  void print();
  NWmsg(); // constructor
};

NWmsg::NWmsg() {
  type = UNICAST;
  nbrOfBackoffs = -1; // only used in (802.15.4)
}

void NWmsg::print() {
  printf("msg: sender=%d, receiver=%d, length=%d ", sender, receiver, length);
}


class NWnode {
 public:
  List *preprocQ;
  List *inputQ;
  List *postprocQ;
  List *outputQ;
  List *switchQ;
  int state;
  int nbrcollisions;
  double waituntil;
  int swstate;   // state of switch output
  int switchmem; // output buffer memory
  double lastused; // (802.11) time when the signal level was last over the threshold
  double *signallevels; // (802.11) array of nodes signallevels in this node
  double backoff;       // (802.11)
  double lastbackoffcount; // (802.11)
  double xCoordinate;      // (802.11)
  double yCoordinate;      // (802.11)
  double total_received_power;   // (802.11) SNR stuff 

  // To be able to set some parameters individually
  double transmitPower;     //802.11
  double predelay;     /* preprocessing delay (s) */
  double postdelay;    /* postprocessing delay (s) */

  NWnode(); // constructor
};

NWnode::NWnode() {
  preprocQ = new List("preprocQ",0);
  inputQ = new List("inputQ",0);
  outputQ = new List("outputQ",0);
  postprocQ = new List("postprocQ",0);
  switchQ = new List("switchQ",0);

  state = 0; // idle
  nbrcollisions = 0;
  waituntil = 0.0;
  swstate = 0; // idle

  lastused = 0;         // (802.11)
  signallevels = NULL;  // (802.11)
  backoff = 0;          // (802.11)
  lastbackoffcount = 0; // (802.11)
  xCoordinate = 0;      // (802.11)
  yCoordinate = 0;      // (802.11)
  total_received_power = BACKGROUND_NOISE; // (802.11)

  predelay = 0;
  postdelay = 0;
}


class RTnetwork {
 public:
  int type;
  int networkNbr;
  int nbrOfNodes;
  double datarate;     /* bits/s */
  int minsize;         /* minimum frame size (bits) */
  double lossprob;     /* probability of a packet getting lost */
  double *bandwidths;  /* FDMA only: vector of bandwidth factors */
  double slottime;     /* TDMA only: size of a slot in seconds */
  int schedsize;       /* TDMA only: the length of the cyclic schedule */
  int *schedule;       /* TDMA only: vector of senders */
  int memsize;         /* SFDSE only: switch memory size */
  int buftype;         /* SFDSE only: switch buffer type */
  int overflow;        /* SFDSE only: switch overflow behavior */

  NWnode **nwnodes;    /* vector of network nodes */

  double time;      // Current time in simulation
  double prevHit;   // Previous invocation of kernel
  double nextHit;   // Next invocation of kernel

  double *inputs;
  double *oldinputs;
  double *outputs;      // trigger outputs
  double *sendschedule; // schedule outputs
  double *energyconsumption; // energy output to the battery

  int sending;          // nbr of the sending node, -1 if none (not FDMA)
  double pathloss;          //802.11
  double receiverThreshold; //802.11
  double acktimeout;        //802.11
  double retrylimit;        //802.11
  double error_threshold;   //802.11

  double waituntil;     // the network is idle until this time (CSMA)
  double lasttime;      // last time a frame started to be sent (CSMA)

  int rrturn;           // Round Robin turn (0 - nbrOfNodes-1) (RR)

  double currslottime;  // time current slot started (TDMA)
  int slotcount;        // current slot in the schedule (TDMA)

  int switchmem;        // global switch memory

  double reach;         // reach radius (Ultrasound network only)
  double pinglength;    // ping duration (Ultrasound network only)
  double speedofsound;  // speed of sound (Ultrasound network only)

  mxArray *nbrOfTransmissions;        // For logging
  mxArray *nbrOfSuccessfulReceptions; // For logging

  RTnetwork(); // constructor
};

RTnetwork::RTnetwork() {
  bandwidths = NULL;
  schedule = NULL;
  inputs = NULL;
  oldinputs = NULL;
  outputs = NULL;
  sendschedule = NULL;
  nbrOfTransmissions = NULL;
}

// Distributed control system: interference node
//
// Generates disturbing network traffic.

#define S_FUNCTION_NAME interference_init

#include "ttkernel.cpp"

// code function
double sender_code(int seg, void *data)
{
  double BWshare = 0.0; // Fraction of the network bandwidth
  if ((double)rand()/(double)RAND_MAX < BWshare) {
    ttSendMsg(1, NULL, 80);  // send 80 bits to myself (no data)
  }
  return FINISHED;
}

double msgRcvhandler(int seg, void *data)
{
  void *msg;
  msg = ttGetMsg();
  return FINISHED;
}

void init()
{
  // Initialize TrueTime kernel
  ttInitKernel(0, 0, prioFP); // nbrOfInputs, nbrOfOutputs, fixed priority

  // Sender task
  double offset = 0;
  double period = 0.001;
  double prio = 1.0;
  ttCreatePeriodicTask("sender", offset, period, prio, sender_code);

  // Initialize network
  ttCreateInterruptHandler("msgRcv", prio, msgRcvhandler);
  ttInitNetwork(1, "msgRcv"); // node #1 in the network
}

void cleanup() {
}

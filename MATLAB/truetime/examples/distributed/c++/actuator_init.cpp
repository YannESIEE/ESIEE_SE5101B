// Distributed control system: actuator node
//
// Receives messages from the controller and actuates 
// the plant.

#define S_FUNCTION_NAME actuator_init

#include "ttkernel.cpp"

// code function
double act_code(int seg, void *data) {
  
  static double *u;

  switch (seg) {
  case 1:
    ttTake("sem");  // wait for a message
    return 0.0;
  case 2:
    return 0.0005;
  case 3:
    u = (double *)ttGetMsg(); // Receive message
    if (u != NULL) {
      ttAnalogOut(1, *u);
      delete u; // delete message
    }
    ttSetNextSegment(1); // loop and wait for new packet  
    return 0.0;
  }

  return FINISHED; // to supress compilation warnings
}

double msgRcvhandler(int seg, void *data)
{
  ttGive("sem");  // signal that there is a message
  return FINISHED;
}


void init()
{
  // Initialize TrueTime kernel
  ttInitKernel(0, 1, prioFP); // nbrOfInputs, nbrOfOutputs, fixed priority

  // Actuator task
  double deadline = 100.0;
  double prio = 1.0;
  ttCreateTask("act_task", deadline, prio, act_code);
  ttCreateJob("act_task");

  // Initialize network
  ttCreateInterruptHandler("msgRcv", prio, msgRcvhandler);
  ttInitNetwork(2, "msgRcv"); // node #2 in the network

  ttCreateSemaphore("sem", 0);
}


void cleanup() {
}

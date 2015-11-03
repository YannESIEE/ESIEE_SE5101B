// Distributed control system: sensor node
//
// Samples the plant periodically and sends the samples to the 
// controller node.

#define S_FUNCTION_NAME sensor_init

#include "ttkernel.cpp"

// code function
double sensor_code(int seg, void *data) {
  static double *y;
  switch (seg) {
  case 1:
    y = new double;
    *y = ttAnalogIn(1);
    return 0.0005;
  case 2:
    ttSendMsg(3, y, 80); // Send measurement (80 bits) to node 3 (controller)
    return 0.0004;
  case 3:
    return FINISHED;
  }

  return FINISHED; // to supress compilation warnings
}

double msgRcvhandler(int seg, void *data)
{
  printf("ERROR: sensor received a message!\n");
  return FINISHED;
}

void init()
{
  // Initialize TrueTime kernel
  ttInitKernel(1, 0, prioFP); // nbrOfInputs, nbrOfOutputs, fixed priority

  // Sensor task
  double offset = 0.0;
  double period = 0.010;
  double prio = 1.0;
  ttCreatePeriodicTask("sensor_task", offset, period, prio, sensor_code);

  // Initialize network
  ttCreateInterruptHandler("msgRcv", prio, msgRcvhandler);
  ttInitNetwork(4, "msgRcv"); // node #4 in the network
}


void cleanup() {
}

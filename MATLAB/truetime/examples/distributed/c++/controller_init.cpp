// Distributed control system: controller node
//
// Receives messages from the sensor node, computes control signal
// and sends it to the actuator node. Also contains a high-priority
// disturbing task.

#define S_FUNCTION_NAME controller_init

#include "ttkernel.cpp"

// PID data structure
class PD_Data {
public:
  // ctrl params
  double K, Td, N, h, ad, bd;
  
  // ctrl states
  double yold, Dold, u;
};

// controller code function
double ctrl_code(int seg, void *data)
{
  double *m;
  double r, y, P, D;
  PD_Data* d = (PD_Data*) data;

  switch(seg) {
  case 1:
    ttTake("sem"); // wait for a message
    return 0.0;

  case 2:
    m = (double*) ttGetMsg(); // get sensor value
    y = *m;
    delete m; // delete message
    r = ttAnalogIn(1);
    
    P = d->K*(r-y);
    D = d->ad*d->Dold + d->bd*(d->yold-y);
    d->u = P + D;
    d->Dold = D;
    d->yold = y;
    return 0.0005;

  case 3:
    m = new double;
    *m = d->u;
    ttSendMsg(2, m, 80); // Send 80 bits to node 2 (actuator)
    ttSetNextSegment(1); // loop and wait for new packet
    return 0.0;
  }

  return FINISHED; // to supress compilation warnings
}

// interfering task code function
double dummy_code(int seg, void *data)
{
  switch (seg) {
  case 1:
    return 0.004;
  case 2:
    return FINISHED;
  }

  return FINISHED; // to supress compilation warnings
}

double msgRcvhandler(int seg, void *data)
{
  ttGive("sem");  // signal that there is a message
  return FINISHED;
}


PD_Data* data;

void init() {

  // Initialize TrueTime kernel  
  ttInitKernel(1, 0, prioFP); // nbrOfInputs, nbrOfOutputs, fixed priority
  
  // Create task data (local memory)
  data = new PD_Data;
  data->K = 1.5;
  data->Td = 0.035;
  data->N = 100000.0;
  data->h = 0.010;
  data->ad = data->Td/(data->N*data->h+data->Td);
  data->bd = data->N*data->K*data->Td/(data->N*data->h+data->Td);
  data->yold = 0.0;
  data->Dold = 0.0;
  data->u = 0.0;

  // Controller task
  double deadline = 0.010;
  double prio = 2.0;
  ttCreateTask("pid_task", deadline, prio, ctrl_code, data);
  ttCreateJob("pid_task");

  // Disturbance task (uncomment to add disturbance task)
  // double offset = 0.0002;
  // double period = 0.007;
  // prio = 1.0;
  // ttCreatePeriodicTask("dummy", offset, period, prio, dummy_code);

  // Initialize network
  ttCreateInterruptHandler("msgRcv", prio, msgRcvhandler);
  ttInitNetwork(3, "msgRcv"); // node #3 in the network
  
  ttCreateSemaphore("sem", 0);
}

void cleanup() {

  delete data;
}

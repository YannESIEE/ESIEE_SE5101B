// PID-control of a DC servo process.
//
// This example shows four ways to implement a periodic controller
// activity in TrueTime. The task implements a standard
// PID-controller to control a DC-servo process (2nd order system). 

#define S_FUNCTION_NAME servo_init

#include "ttkernel.cpp"

// PID data structure used in Implementations 1a, 2, and 3 below.
class PID_Data {
public:
  struct { // states
    double u, Iold, Dold, yold, t; // t used only in Implementation 2 below 
  } s;
  
  struct { // params
    double K, Ti, Td, beta, N, h;
    int rChan, yChan, uChan; 
  } p;
};

// calculate PID control signal and update states
void pidcalc(PID_Data* d, double r, double y) {
  double P = d->p.K*(d->p.beta*r-y);
  double I = d->s.Iold;
  double D = d->p.Td/(d->p.N*d->p.h+d->p.Td)*d->s.Dold+d->p.N*d->p.K*d->p.Td/(d->p.N*d->p.h+d->p.Td)*(d->s.yold-y); 

  d->s.u = P + I + D;
  d->s.Iold = d->s.Iold + d->p.K*d->p.h/d->p.Ti*(r-y);
  d->s.Dold = D;
  d->s.yold = y;
};

// Variables used in Implementation 1b below.
const int nInp = 2;                 // nbr of inputs to controller block 
const int nOutp = 2;                // nbr of outputs to controller block 
static double inp[] = {0.0, 0.0};   // block inputs 
static double outp[] = {0.0, 0.0};  // block outputs 


// ---- PID code function for Implementation 1 ----
double pidcode1(int seg, void* data) {

  double r, y;
  PID_Data* d = (PID_Data*) data;

  switch (seg) {
  case 1:  
    r = ttAnalogIn(d->p.rChan);
    y = ttAnalogIn(d->p.yChan);
    pidcalc(d, r, y); 
    return 0.002;
  case 2:    
    ttAnalogOut(d->p.uChan, d->s.u);
    return FINISHED;
  }

  return FINISHED; // to supress compilation warnings
}

// ---- PID code function for Implementation 2 ----
double pidcode2(int seg, void* data) {

  switch (seg) {
  case 1: 
    inp[0] = ttAnalogIn(1);
    inp[1] = ttAnalogIn(2);

    ttCallBlockSystem(nOutp, outp, nInp, inp, "controller");

    return outp[1];     // execution time returned from block 
  case 2:    
    ttAnalogOut(1, outp[0]);
    return FINISHED;
  }

  return FINISHED; // to supress compilation warnings
}

// ---- PID code function for Implementation 3 ----
double pidcode3(int seg, void* data) {

  double r, y;
  PID_Data* d = (PID_Data*) data;

  switch (seg) {
  case 1:
    d->s.t = ttCurrentTime();
    return 0.0;
  case 2:  
    r = ttAnalogIn(d->p.rChan);
    y = ttAnalogIn(d->p.yChan);
    pidcalc(d, r, y); 
    return 0.002;
  case 3:    
    ttAnalogOut(d->p.uChan, d->s.u);
    // Sleep
    d->s.t += d->p.h;
    ttSleepUntil(d->s.t);
    return 0.0;
  case 4:
    ttSetNextSegment(2); // loop
    return 0.0;
  }

  return FINISHED; // to supress compilation warnings
}

// ---- PID code function for Implementation 4 ----
double pidcode4(int seg, void* data) {

  double r;
  double *y;
  PID_Data* d = (PID_Data*) data;

  switch (seg) {
  case 1:  
    r = ttAnalogIn(d->p.rChan);
    y = (double*) ttTryFetch("Samples");
    pidcalc(d, r, *y); 
    delete y;
    return 0.0018;
  case 2:    
    ttAnalogOut(d->p.uChan, d->s.u);
    return FINISHED;
  }

  return FINISHED; // to supress compilation warnings
}

// ---- Sampler code function for Implementation 4 ----
double samplercode(int seg, void* data) {

  double y;
  int* d = (int*) data;

  switch (seg) {
  case 1:  
    y = ttAnalogIn(*d);
    ttTryPost("Samples", new double(y)); // put sample in mailbox
    ttCreateJob("pid_task");  // trigger task job
    return 0.0002;
  case 2:    
    return FINISHED;
  }

  return FINISHED; // to supress compilation warnings
}

#define NBROFINPUTS 2
#define NBROFOUTPUTS 1
#define SCHEDULER prioFP

PID_Data *d;
double *d2 = NULL;
int *hdl_data = NULL;

void init() {
  
  // Initialize TrueTime kernel
  ttInitKernel(NBROFINPUTS, NBROFOUTPUTS, SCHEDULER);

  // Task attributes
  double period = 0.006;
  double offset = 0.0; // start of first task instance
  double prio = 1.0;
  double deadline = period;

  // Create task data (local memory)
  d = new PID_Data;
  d->p.K = 0.96;
  d->p.Ti = 0.12;
  d->p.Td = 0.049;
  d->p.beta = 0.5;
  d->p.N = 10.0;
  d->p.h = period;
  d->s.u = 0.0;
  d->s.t = 0.0; // only used in Implementation 2 below
  d->s.Iold = 0.0;
  d->s.Dold = 0.0;
  d->s.yold = 0.0;
  d->p.rChan = 1;
  d->p.yChan = 2;
  d->p.uChan = 1;

  // Read the input argument from the block dialogue
  mxArray *initarg = ttGetInitArg();
  if (!mxIsDoubleScalar(initarg)) {
    MEX_ERROR("The init argument must be a number!");
    return;
  }

  int implementation = (int)mxGetPr(initarg)[0];

  switch (implementation) {
    
  case 1:
    // IMPLEMENTATION 1: using the built-in support for periodic tasks
    
    ttCreatePeriodicTask("pid_task", offset, period, prio, pidcode1, d);
    break;

  case 2:
    // IMPLEMENTATION 2: calling Simulink block within code function
    
    d2 = new double(0.0); // Only the control signal needs to be 
                          // stored between segments. Controller 
                          // states are stored internally by TrueTime.
    ttCreatePeriodicTask("pid_task", offset, period, prio, pidcode2, d2);
    break;

  case 3:
    // IMPLEMENTATION 3: sleepUntil and loop back
  
    ttCreateTask("pid_task", deadline, prio, pidcode3, d);
    ttCreateJob("pid_task");
    break;
    
  case 4:
    // IMPLEMENTATION 4: sampling in timer handler, triggers task job
  
    hdl_data = new int(2); // y_chan for reading samples
    ttCreateInterruptHandler("timer_handler", prio, samplercode, hdl_data);
    ttCreatePeriodicTimer("timer", offset, period, "timer_handler");
    ttCreateMailbox("Samples", 10);
    ttCreateTask("pid_task", deadline, prio, pidcode4, d);
    break;

  }
}

void cleanup() {
  // This is called also in the case of an error

  delete d;
  if (d2) delete d2;
  if (hdl_data) delete hdl_data;
}

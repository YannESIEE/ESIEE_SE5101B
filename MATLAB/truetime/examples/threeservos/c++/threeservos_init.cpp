// Task scheduling and control.
//
// This example extends the simple PID control example (located in
// $DIR/examples/simple_pid) to the case of three PID-tasks running
// concurrently on the same CPU controlling three different servo
// systems. The effect of the scheduling policy on the global control
// performance is demonstrated.

#define S_FUNCTION_NAME threeservos_init

#include "ttkernel.cpp"

// PID data structure
class PID_Data {
public:
  struct { // states
    double u, Iold, Dold, yold; 
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


// --------- Generic code function ----------
double pidcode(int seg, void* data) {

  PID_Data* d = (PID_Data*) data;

  switch (seg) {
  case 1:  
    pidcalc(d, ttAnalogIn(d->p.rChan), ttAnalogIn(d->p.yChan)); 
    return 0.002;
  case 2:    
    ttAnalogOut(d->p.uChan, d->s.u);
    return FINISHED;
  }

  return FINISHED; // to supress compilation warnings
}

#define NBROFINPUTS 6
#define NBROFOUTPUTS 3
#define SCHEDULER prioRM

// Task parameters 
double periods[] = {0.006, 0.005, 0.004};
char* names[] = {"pid_task1", "pid_task2", "pid_task3"};
int rChans[] = {1, 3, 5};
int yChans[] = {2, 4, 6};
int uChans[] = {1, 2, 3};

PID_Data *d[3];

void init() {

  // Initialize TrueTime kernel
  ttInitKernel(NBROFINPUTS, NBROFOUTPUTS, SCHEDULER);

  // Create the three tasks
  for (int i = 0; i < 3; i++) {
    d[i] = new PID_Data;
    d[i]->p.K = 0.96;
    d[i]->p.Ti = 0.12;
    d[i]->p.Td = 0.049;
    d[i]->p.beta = 0.5;
    d[i]->p.N = 10;
    d[i]->p.h = periods[i];
    d[i]->s.u = 0.0;
    d[i]->s.Iold = 0.0;
    d[i]->s.Dold = 0.0;
    d[i]->s.yold = 0.0;
    d[i]->p.rChan = rChans[i];
    d[i]->p.yChan = yChans[i];
    d[i]->p.uChan = uChans[i];

    // Offset=0 and prio=1 for all tasks
    ttCreatePeriodicTask(names[i], 0.0, periods[i], 1.0, pidcode, d[i]);
  }
}

void cleanup() {

  for (int i = 0; i < 3; i++) {
    delete d[i];
  }
}

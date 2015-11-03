/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef __TT_KERNEL_H__
#define __TT_KERNEL_H__

#include "mex.h"
#include <string.h>
#include <math.h>

#include "mexhelp.h"
#include "linkedlist.cpp"
#include "datanode.h"

#define EPS 1.0E-15  // Timing precision
#define INF 1.0E+15  // Maximum simulation time
#define FINISHED -1.0
    
enum { FP, RM, DM, EDF }; // supported scheduling policies
enum { ARRIVAL, RELEASE, START, SUSPEND, RESUME, FINISH }; // hooks
enum { IDLE, READY, RUNNING, WAITING, SLEEPING, SUSPENDED };   // usertask states
enum { UNUSED, OVERRUN, TIMER, NETWORK, EXTERNAL }; // handler types 

#define NBRLOGS 10 // Maximum number of log entries for each task
// Log types (5 pre-defined)
enum { RESPONSETIMELOG=1, RELEASELATENCYLOG, STARTLATENCYLOG, EXECTIMELOG, CONTEXTRESTORELOG, USERLOG }; 
#define NBRUSERLOGS (NBRLOGS - 5) // Number of user-defined log types

#include "ttnetwork.h"

#include "task.h"
#include "log.h"
#include "blockdata.h"
#include "usertask.h"

#include "timer.h"
#include "trigger.h"
#include "network.h"
#include "handler.h"

#include "monitor.h"
#include "event.h"
#include "mailbox.h"
#include "semaphore.h"

void init(void);
void cleanup(void);

class RTsys {
 public:
  char* name;

  bool init_phase;    // false when the simulation is running
  bool initialized;   // true if ttInitKernel has been called
  bool error;         // true if simulation should stop
  bool started;       // true after time zero
  bool mdlzerocalled; // true if mdlZeroCrossings has been called
  
  int nbrOfInputs;
  int nbrOfOutputs;
  int nbrOfTasks;
  int nbrOfHandlers;
  int nbrOfMonitors;
  int nbrOfTriggers;
  int nbrOfSchedTasks;
  int nbrOfSchedHandlers;
  int nbrOfSchedMonitors;
  
  double time;      // Current time in simulation
  double prevHit;   // Previous invocation of kernel
  double nextHit;   // Next invocation of kernel

  double *inputs;          // Vector of input port (analogin) values
  double *outputs;         // Vector of output port (analogout) values
  double *interruptinputs;    // External interrupt trigger signals
  double *oldinterruptinputs;

  double *taskSched;      // Vector of values for the usertask schedule
  double *handlerSched;   // Vector of values for the handler schedule
  double *monitorGraph;   // Vector of values for the monitor graph

  Task* running;          // Currently running task

  List* readyQ;   // usertasks and handlers ready for execution, prio-sorted
  List* timeQ;    // usertasks and handlers waiting for release, time-sorted

  List *taskList;      // List of datanodes with pointers to created tasks
  List *handlerList;   // List of datanodes with pointers to created handlers
  List *timerList;     // List of datanodes with pointers to timer handlers
  List *monitorList;   // List of datanodes with pointers to created monitors
  List *eventList;     // List of datanodes with pointers to created event
  List *triggerList;   // List of datanodes with pointers to ext. interrupt handlers
  List *mailboxList;   // List of datanodes with pointers to created mailboxes
  List *semaphoreList; // List of datanodes with pointers to created semaphores

  mxArray *initarg;                // Pointer to Kernel block init arg

  double (*prioFcn)(UserTask*);    // Priority function (see priofunctions.cpp)

  double contextSwitchTime;         // Time for a full context save/restore
  InterruptHandler* kernelHandler;  // Handler simulating context switches
  double contextSimTime;            // Execution time of handler code function
  UserTask* suspended;              // Last suspended usertask (context switch
                                    // if another task is resumed or started)

  // NETWORK 
  int nbrOfNetworks;         // Number of TrueTime networks to which the
                             // kernel (node) is connected
  double *nwSnd;             // Send output 
  double *networkinputs;     // Network interrupt trigger signals
  double *oldnetworkinputs;
  List *networkList;         // List of datanodes with pointers to network interrupt 
                             // handlers. One handler for each network to which the 
                             // kernel (node) is connected. 

  // Function pointers
  double (*contextSwitchCode)(int, void*); // Code function for context switch handler
  double (*periodicTaskHandlerCode)(int, void*);  // Code function for handler 
                                                  // generating periodic task jobs. 

  int (*prioSort)(Node* , Node*);  // Sorting function for priority-sorted lists
  int (*timeSort)(Node* , Node*);  // Sorting function for time-sorted lists
                                   // (see compfunctions.cpp)

  void (*default_arrival)(UserTask*);  // Default kernel hooks
  void (*default_release)(UserTask*);  // (see defaulthooks.cpp)
  void (*default_start)(UserTask*);
  void (*default_suspend)(UserTask*);
  void (*default_resume)(UserTask*);
  void (*default_finish)(UserTask*);

  double (*prioFP)(UserTask*);         // Standard priority functions
  double (*prioRM)(UserTask*);         // (see priofunctions.cpp)
  double (*prioEDF)(UserTask*);
  double (*prioDM)(UserTask*);

  double energyLevel;       // Input from the battery
  double energyConsumption; // How much we consume
  double cpuScaling;        // How fast we run
  double clockDrift;        // 
  double clockOffset;       // Offset from the nominal time

#ifdef KERNEL_MATLAB
  mxArray* rtsysptr;        // Pointer to global variable "_rtsys"
#endif

  RTsys();  // constructor
  ~RTsys(); // deconstructor
};


/**
 * RTsys Constructor 
 */
RTsys::RTsys() {

  name = "NoName";

  init_phase = true;
  initialized = false;
  error = false;
  started = false;
  mdlzerocalled = false;

  nbrOfInputs = 0;
  nbrOfOutputs = 0;
  nbrOfTasks = 0;
  nbrOfHandlers = 0;
  nbrOfMonitors = 0;
  nbrOfTriggers = 0;
  nbrOfSchedTasks = 0;
  nbrOfSchedHandlers = 0;
  nbrOfSchedMonitors = 0;
  
  time = 0.0; 
  prevHit = 0.0;
  nextHit = 0.0;

  inputs = NULL;   
  outputs = NULL;  
  interruptinputs = NULL; 
  oldinterruptinputs = NULL;

  taskSched = NULL;
  handlerSched = NULL;
  monitorGraph = NULL;

  running = NULL;

  readyQ = NULL;
  timeQ = NULL;

  taskList = NULL;
  handlerList = NULL;
  timerList = NULL;
  monitorList = NULL;
  eventList = NULL;
  triggerList = NULL;
  mailboxList = NULL;
  semaphoreList = NULL;

  initarg = NULL;

  prioFcn = NULL;

  contextSwitchTime = 0.0;
  contextSimTime = 0.0;
  kernelHandler = NULL;
  suspended = NULL;
  
  nbrOfNetworks = 0;
  nwSnd = NULL;  
  networkinputs = NULL;
  oldnetworkinputs = NULL;
  networkList = NULL;

  energyLevel = 1;       // Energy level from the battery
  energyConsumption = 0; // How much we consume
  cpuScaling = 1;        // No scaling as default
  clockDrift = 1;        // No drift as default
  clockOffset = 0;       // No offset as default

#ifdef KERNEL_MATLAB
  rtsysptr = NULL;
#endif
}

/**
 * RTsys Destructor 
 */
RTsys::~RTsys() {
  DataNode *dn, *tmp;

  if (inputs) delete[] inputs;
  if (outputs) delete[] outputs;
  if (interruptinputs) delete[] interruptinputs;
  if (oldinterruptinputs) delete[] oldinterruptinputs;
  if (taskSched) delete[] taskSched;
  if (handlerSched) delete[] handlerSched;
  if (monitorGraph) delete[] monitorGraph;
  if (nwSnd) delete[] nwSnd;
  if (networkinputs) delete[] networkinputs;
  if (oldnetworkinputs) delete[] oldnetworkinputs;

  if (readyQ) delete readyQ;
  if (timeQ) delete timeQ;
  
  // Delete all tasks and the tasklist
  if (taskList) {
    dn = (DataNode*) taskList->getFirst();
    while (dn != NULL) {
      UserTask* task = (UserTask*) dn->data; 
      delete task;
      tmp = dn;
      dn = (DataNode*) dn->getNext();
      delete tmp;
    }
    delete taskList;
  }
  // Delete all handlers and the handlerlist
  if (handlerList) {
    dn = (DataNode*) handlerList->getFirst();
    while (dn != NULL) {
      InterruptHandler* hdl = (InterruptHandler*) dn->data;
      delete hdl;
      tmp = dn;
      dn = (DataNode*) dn->getNext();
      delete tmp;
    }
    delete handlerList;
  }
  // Delete the timerlist
  if (timerList) {
    dn = (DataNode*) timerList->getFirst();
    while (dn != NULL) {
      tmp = dn;
      dn = (DataNode*) dn->getNext();
      delete tmp;
    }
    delete timerList;
  }  
  // Delete all monitors and the monitorlist
  if (monitorList) {
    dn = (DataNode*) monitorList->getFirst();
    while (dn != NULL) {
      Monitor* mon = (Monitor*) dn->data;
      delete mon;
      tmp = dn;
      dn = (DataNode*) dn->getNext();
      delete tmp;
    }
    delete monitorList;
  }
  // Delete all events and the eventlist
  if (eventList) {
    dn = (DataNode*) eventList->getFirst();
    while (dn != NULL) {
      Event* ev = (Event*) dn->data;
      delete ev;
      tmp = dn;
      dn = (DataNode*) dn->getNext();
      delete tmp;
    }
    delete eventList;
  }
  // Delete triggerlist
  if (triggerList) {
    dn = (DataNode*) triggerList->getFirst();
    while (dn != NULL) {
      tmp = dn;
      dn = (DataNode*) dn->getNext();
      delete tmp;
    }
    delete triggerList;
  }  
  // Delete all mailboxes and the mailboxlist
  if (mailboxList) {
    dn = (DataNode*) mailboxList->getFirst();
    while (dn != NULL) {
      Mailbox* mb = (Mailbox*) dn->data;
      delete mb;
      tmp = dn;
      dn = (DataNode*) dn->getNext();
      delete tmp;
    }
    delete mailboxList;
  } 
  // Delete all semaphores and the semaphorelist
  if (semaphoreList) {
    dn = (DataNode*) semaphoreList->getFirst();
    while (dn != NULL) {
      Semaphore* sem = (Semaphore*) dn->data;
      delete sem;
      tmp = dn;
      dn = (DataNode*) dn->getNext();
      delete tmp;
    }
    delete semaphoreList;
  } 
  // Delete networklist
  if (networkList) {
    dn = (DataNode*) networkList->getFirst();
    while (dn != NULL) {
      tmp = dn;
      dn = (DataNode*) dn->getNext();
      delete tmp;
    }
    delete networkList;
  }

}
  

#endif // __TT_KERNEL_H__




/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#include "ttkernel.h"
// ----- Main data structure ------

RTsys *rtsys; // global variable used by all instances of ttkernel

// ------- Internal functions used by kernel ------- 

#include "compfunctions.cpp"
#include "codefunctions.cpp"
#include "priofunctions.cpp"
#include "defaulthooks.cpp"
#include "initnetwork2.cpp"

#ifndef KERNEL_MATLAB

#define KERNEL_C

// --- Initialization and creation ----

#include "initkernel.cpp"
#include "createtask.cpp"
#include "createpertask.cpp"
#include "createhandler.cpp"
#include "createtrigger.cpp"
#include "initnetwork.cpp"
#include "createmonitor.cpp"
#include "createevent.cpp"
#include "createmailbox.cpp"
#include "createsemaphore.cpp"
#include "createlog.cpp"
#include "attachdlhandler.cpp"
#include "attachwcethandler.cpp"
#include "attachpriofcn.cpp"
#include "attachhook.cpp"
#include "noschedule.cpp"
#include "nonpreemptable.cpp"
#include "getinitarg.cpp"

// ------- Real-time primitives ------- 

#include "createjob.cpp"
#include "killjob.cpp"
#include "createtimer.cpp"
#include "removetimer.cpp"
#include "analogin.cpp"
#include "analogout.cpp"
#include "sleep.cpp"
#include "entermonitor.cpp"
#include "exitmonitor.cpp"
#include "wait.cpp"
#include "notify.cpp"
#include "tryfetch.cpp"
#include "trypost.cpp"
#include "fetch.cpp"
#include "post.cpp"
#include "retrieve.cpp"
#include "take.cpp"
#include "give.cpp"
#include "lognow.cpp"
#include "logstart.cpp"
#include "logstop.cpp"
#include "currenttime.cpp"
#include "invokingtask.cpp"
#include "setnextsegment.cpp"
#include "callblocksystem.cpp"
#include "sendmsg.cpp"
#include "getmsg.cpp"
#include "setnetworkparameter.cpp"
#include "abortsimulation.cpp"
#include "discardunsent.cpp"

// ---------- Sets and Gets ------------

#include "setdeadline.cpp"
#include "setabsdeadline.cpp"
#include "setpriority.cpp"
#include "setperiod.cpp"
#include "setbudget.cpp"
#include "setwcet.cpp"
#include "setdata.cpp"
#include "getrelease.cpp"
#include "getdeadline.cpp"
#include "getabsdeadline.cpp"
#include "getpriority.cpp"
#include "getperiod.cpp"
#include "getbudget.cpp"
#include "getwcet.cpp"
#include "getdata.cpp"

#endif

#ifdef KERNEL_MATLAB

// ----------------------------------------------
// ------ Executes an m-file code function ------
// ----------------------------------------------

// mxArray used to pass the segment to the code function
mxArray *segArray;
bool destroyed;

double executeCode(char *codeName, int seg, Task *task) {
  double retval;
  mxArray *lhs[2];
  mxArray *rhs[2];

  *mxGetPr(segArray) = (double) seg;

  rhs[0] = segArray;
  if (task->dataMATLAB) {
    //rhs[1] = mxDuplicateArray(task->dataMATLAB);
    rhs[1] = task->dataMATLAB;
  } else {
    rhs[1] = mxCreateDoubleMatrix(1, 1, mxREAL);
    *mxGetPr(rhs[1]) = 0.0;
  }

  mexSetTrapFlag(1); // return control to the MEX file after an error
  lhs[0] = NULL;     // needed not to crash Matlab after an error
  lhs[1] = NULL;     // needed not to crash Matlab after an error
  if (mexCallMATLAB(2, lhs, 2, rhs, codeName) != 0) {
    rtsys->error = true;
    return 0.0;
  }

  if (mxGetClassID(lhs[0]) == mxUNKNOWN_CLASS) {
    printf("??? executeCode: execution time not assigned\n\n");
    printf("Error in ==> code function '%s', segment %d\n", codeName, seg);
    rtsys->error = true;
    return 0.0;
  }

  if (!mxIsDoubleScalar(lhs[0])) {
    printf("??? executeCode: illegal execution time\n\n");
    printf("Error in ==> code function '%s', segment %d\n", codeName, seg);
    rtsys->error = true;
    return 0.0;
  }

  if (mxGetClassID(lhs[1]) == mxUNKNOWN_CLASS) {
    printf("??? executeCode: data not assigned\n\n");
    printf("Error in ==> code function '%s', segment %d\n", codeName, seg);
    rtsys->error = true;
    return 0.0;
  }

  //if ( task->dataMATLAB ) {
  if ( task->dataMATLAB != lhs[1] ) {
    mxDestroyArray(task->dataMATLAB);
    //task->dataMATLAB = mxDuplicateArray(lhs[1]);
    task->dataMATLAB = lhs[1];
    mexMakeArrayPersistent(task->dataMATLAB);
  }
  
  retval = *mxGetPr(lhs[0]);

  //mxDestroyArray(rhs[1]);
  mxDestroyArray(lhs[0]);
  //mxDestroyArray(lhs[1]);

  return retval;
}
#endif

// ----------------------------------------------
// -- Determines time for next clock interrupt --
// ----- used in the kernel function below ------
// ----------------------------------------------

double getNextInvocation() {

  double compTime;
  double nextHit = INF;
  
  // Next release from timeQ
  if (rtsys->timeQ->getFirst() != NULL) {
    Task* t = (Task*) rtsys->timeQ->getFirst();
    nextHit = t->wakeupTime() - rtsys->time;
  }

  // Remaining execution time of running task
  if (rtsys->running != NULL) {
    compTime = rtsys->running->execTime / rtsys->cpuScaling;
    nextHit = (nextHit < compTime) ? nextHit : compTime;
  } 
  
  return nextHit;
}


// -------------------------------------------------------------
// --------------------- Kernel Function -----------------------
// ----  Called from the Simulink callback functions during ----
// -- simulation and returns the time for its next invocation --
// -------------------------------------------------------------

double runKernel(double externalTime) {
  
  double nextHit, timeElapsed; 

  Task *task, *temp, *oldrunning, *newrunning;
  UserTask *usertask;
  InterruptHandler *hdl;
  DataNode* dn;

  // If no energy, then we can not run
  if (rtsys->energyLevel <= 0) {
    //printf("Energy is out at time: %f\n", rtsys->time);
    return INF;
  }

  
  timeElapsed = externalTime - rtsys->prevHit; // time since last invocation
  rtsys->prevHit = externalTime;  // update previous invocation time
  nextHit = 0.0;

  //printf("runkernel at %f\n", rtsys->time);

#ifdef KERNEL_MATLAB
    /* Write rtsys pointer to global workspace */
  *((long *)mxGetPr(rtsys->rtsysptr)) = (long)rtsys;
#endif

  while (nextHit < EPS) {

    // Count down execution time for current task (usertask or handler)
    // and check if it has finished its execution 

    task = rtsys->running;
    if (task != NULL) {
      // Count down execution time 
      task->execTime -= timeElapsed * rtsys->cpuScaling;
      if (task->execTime < EPS) {
	// Execute next segment 
	task->segment++;

	if (task->isUserTask()) {
	  usertask = (UserTask*) task;
	  // Update budget and lastStart variable at segment change
	  usertask->budget -= (rtsys->time - usertask->lastStart);
	  usertask->lastStart = rtsys->time;
	}

	// Execute next segment of the code function

#ifndef KERNEL_MATLAB
	task->execTime = task->codeFcn(task->segment, task->data);
	if (rtsys->error) {
	  printf("Error in ==> task '%s', segment %d\n", task->name, task->segment);
	  return 0.0;
	}
#else
	if (task->codeFcnMATLAB == NULL) {
	  task->execTime = task->codeFcn(task->segment, task->data);
	} else {
	  task->execTime = executeCode(task->codeFcnMATLAB, task->segment, task);
	}
	if (rtsys->error) {
	  printf("Error in ==> task '%s', segment %d\n", task->name, task->segment);
	  return 0.0;
	}
#endif

	if (task->execTime < 0.0) { 
	  // Negative execution time = task finished
	  task->execTime = 0.0;
	  task->segment = 0;

	  if (task->myList == rtsys->readyQ) {
	    // Remove task from readyQ
	    task->remove();
	  }
	  
	  if (!(task->isUserTask())) {
	    hdl = (InterruptHandler*) task;

	    if (hdl->type == TIMER) {
	      if (hdl->timer->isPeriodic) {
		// if periodic timer put back in timeQ
		hdl->timer->time += hdl->timer->period;
		hdl->moveToList(rtsys->timeQ);
	      } else {
		// Remove timer and free up handler
		dn = getNode(hdl->timer->name, rtsys->timerList);
		rtsys->timerList->deleteNode(dn);
		delete hdl->timer;
		hdl->timer = NULL;
		hdl->type = UNUSED;
	      }
	    }
	    if (hdl->type == EXTERNAL) {
	      if (hdl->pending > 0) {
		// new external interrupt occured before handler finished
		hdl->pending--;
		hdl->moveToList(rtsys->readyQ);
	      }
	    }

	  } else { // the finished task is a usertask
	    usertask = (UserTask*) task;

	    // Execute finish-hook 
	    usertask->finish_hook(usertask);
	    usertask->state = IDLE;

	    // Release next job if any
	    usertask->nbrJobs--;
	    if (usertask->nbrJobs > 0) {
	      // next pending release
	      dn = (DataNode*) usertask->pending->getFirst();
	      double* release = (double*) dn->data;
	      usertask->release = *release;
	      usertask->absDeadline = *release + usertask->deadline;
	      usertask->moveToList(rtsys->timeQ);
	      usertask->pending->deleteNode(dn);
	      delete release;
	      // Execute release-hook 
	      usertask->release_hook(usertask);
	      usertask->state = SLEEPING;
	    
	    }
	  }
	}
      }
    } // end: counting down execution time of running task


    // Check time queue for possible releases

    task = (Task*) rtsys->timeQ->getFirst();
    while (task != NULL) {
      if ((task->wakeupTime() - rtsys->time) < EPS) {
	
	// Task to be released 
	temp = task;
	task = (Task*) task->getNext();
	temp->moveToList(rtsys->readyQ);
	
	if (temp->isUserTask()) {
	  usertask = (UserTask*) temp;
	  usertask->state = READY;

	}
      } else {
	break;
      }
    } // end: checking timeQ for releases


    // Determine task with highest priority and make it running task

    newrunning = (Task*) rtsys->readyQ->getFirst();
    oldrunning = rtsys->running;

    if (newrunning != NULL) {
      
      // Check for suspend- and resume-hooks
      
      if (oldrunning != NULL) {

	// Is oldrunning being suspended?
	if (oldrunning->isUserTask()) {
	  if (newrunning != oldrunning && ((UserTask*) oldrunning)->state == RUNNING) {
	    usertask = (UserTask*) oldrunning;
	    usertask->state = SUSPENDED;
	    usertask->suspend_hook(usertask);
	  }
	}
      }

      // invocation of hooks may have triggered kernelHandler
      newrunning = (Task*) rtsys->readyQ->getFirst();
      
      // Is newrunning being resumed?
      if (newrunning->isUserTask()) {
	if ( (((UserTask*) newrunning)->state == READY) || 
	     (((UserTask*) newrunning)->state == SUSPENDED) ) {
	  // newrunning is being resumed or started
	  usertask = (UserTask*) newrunning;
	  usertask->state = RUNNING;
	  if (usertask->segment == 0) {
	    usertask->start_hook(usertask);
	  } else {
	    usertask->resume_hook(usertask);
	  }
	}
      }

      // invocation of hooks may have triggered kernelHandler
      rtsys->running = (Task*) rtsys->readyQ->getFirst();
      
    } else { // No tasks in readyQ
      
      rtsys->running = NULL;
      
    } // end: task dispatching
    

    // Determine next invocation of kernel
    nextHit = getNextInvocation();
    timeElapsed = 0.0;
    
  } // end: loop while nextHit < EPS

  return nextHit;
}


// ------- Simulink callback functions ------- 

#ifdef __cplusplus
extern "C" { // use the C fcn-call standard for all functions  
#endif       // defined within this scope   

#define S_FUNCTION_LEVEL 2
  
#include "simstruc.h"


static void mdlInitializeSizes(SimStruct *S)
{
  static int printed = 0;
  if (!printed) {
    printed = 1;
    printf("-------------------------------------------------------\n");
    printf("                 TrueTime, Version 1.5\n");            
    printf("                   Copyright (c) 2007\n");
    printf("     Martin Ohlin, Dan Henriksson and Anton Cervin\n");
    printf("          Department of Automatic Control LTH\n");        
    printf("                Lund University, Sweden\n");
    printf("-------------------------------------------------------\n");  
  }

#ifdef KERNEL_MATLAB
  char initfun[100];
  static mxArray *lhs[1]; // warning: used multiple times
  static mxArray *rhs[3]; // warning: used multiple times
  
  segArray = mxCreateDoubleScalar(0.0);
  destroyed = false;
  mexMakeArrayPersistent(segArray);
#endif
  
  ssSetNumSFcnParams(S, 4);  /* Number of expected parameters */
  if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
    return; /* Parameter mismatch will be reported by Simulink */
  }
  
#ifdef KERNEL_MATLAB  
  mxGetString(ssGetSFcnParam(S, 0), initfun, 100);
#endif

  rtsys = new RTsys;

  /* Assign function pointers */
  rtsys->contextSwitchCode = contextSwitchCode;
  rtsys->periodicTaskHandlerCode = periodicTaskHandlerCode;

  rtsys->timeSort = timeSort;
  rtsys->prioSort = prioSort;

  rtsys->default_arrival = default_arrival;
  rtsys->default_release = default_release;
  rtsys->default_start = default_start;
  rtsys->default_suspend = default_suspend;
  rtsys->default_resume = default_resume;
  rtsys->default_finish = default_finish;

  rtsys->prioFP = prioFP;
  rtsys->prioRM = prioRM;
  rtsys->prioEDF = prioEDF;
  rtsys->prioDM = prioDM;

#ifdef KERNEL_MATLAB
  mexSetTrapFlag(1); // return control to the MEX file after an error

  /* Write rtsys pointer to global workspace */
  char rtsysbuf[100];
  sprintf(rtsysbuf, "%p", rtsys);
  mxArray* var = mxCreateDoubleScalar(0.0);
  mexMakeArrayPersistent(var);
  *((long *)mxGetPr(var)) = (long)rtsys;
  mexPutVariable("global", "_rtsys", var);
  rtsys->rtsysptr = (mxArray*)mexGetVariablePtr("global", "_rtsys");

  /* Evaluating user-defined init function (MATLAB) */
  rhs[0] = mxCreateString(initfun);
  if (mexCallMATLAB(1, lhs, 1, rhs, "nargin") != 0) {
    printf("Call to init function failed!\n");
    ssSetErrorStatus(S, "Call to init function failed!");
    return;
  } else {
    if (*mxGetPr(lhs[0]) == 0) {
      if (mexCallMATLAB(0, NULL, 0, NULL, initfun) != 0) {
	printf("Call to init function failed!\n");
	ssSetErrorStatus(S, "Call to init function failed!");
	return;
      } else {
	rtsys->init_phase = false;
      }
    } else if (*mxGetPr(lhs[0]) == 1) {
      rhs[0] = (mxArray *)ssGetSFcnParam(S, 1);
      if (mexCallMATLAB(0, NULL, 1, rhs, initfun) != 0) {
	printf("Call to init function failed!\n");
	ssSetErrorStatus(S, "Call to init function failed!");
	return;
      } else {
	rtsys->init_phase = false;
      }
    } else {
      printf("Init function takes wrong number (> 1) of arguments!\n");
      ssSetErrorStatus(S, "Init function takes wrong number (> 1) of arguments!");
      return;
    }
  }

#else
  /* Save pointer to init args */
  rtsys->initarg = (mxArray *)ssGetSFcnParam(S, 1);
  /* Evaluating user-defined init function (C++) */
  init();
  rtsys->init_phase = false;
#endif

  
  if (!rtsys->initialized) {
    printf("ttInitKernel was not called in init function!\n");
    ssSetErrorStatus(S, "ttInitKernel was not called in init function!");
    return;
  }

  // Clock drift parameters
  const mxArray *arg;
  arg = ssGetSFcnParam(S, 2);
  if (mxIsDoubleScalar(arg)) {
    rtsys->clockDrift = *mxGetPr(arg) + 1;
  }
  arg = ssGetSFcnParam(S, 3);
  if (mxIsDoubleScalar(arg)) {
    rtsys->clockOffset = *mxGetPr(arg);
  }
  //printf("drift: %f, offset:%f\n", rtsys->clockDrift, rtsys->clockOffset);



  if (!ssSetNumInputPorts(S, 4)) return;
  ssSetInputPortDirectFeedThrough(S, 0, 0);
  ssSetInputPortDirectFeedThrough(S, 1, 0);
  ssSetInputPortDirectFeedThrough(S, 2, 0);
  ssSetInputPortDirectFeedThrough(S, 3, 0);
  if (!ssSetNumOutputPorts(S, 5)) return;

  /* Input Ports */

  if (rtsys->nbrOfInputs > 0) 
    ssSetInputPortWidth(S, 0, rtsys->nbrOfInputs);
  else
    ssSetInputPortWidth(S, 0, 1);
  
  if (rtsys->nbrOfTriggers > 0) 
    ssSetInputPortWidth(S, 1, rtsys->nbrOfTriggers);
  else
    ssSetInputPortWidth(S, 1, 1);

  if (rtsys->nbrOfNetworks > 0) 
    ssSetInputPortWidth(S, 2, rtsys->nbrOfNetworks); /* Network receive */
  else
    ssSetInputPortWidth(S, 2, 1);

  ssSetInputPortWidth(S, 3, 1); //battery

  /* Output Ports */

  if (rtsys->nbrOfOutputs > 0) 
    ssSetOutputPortWidth(S, 0, rtsys->nbrOfOutputs);
  else
    ssSetOutputPortWidth(S, 0, 1);

  if (rtsys->nbrOfNetworks > 0) 
    ssSetOutputPortWidth(S, 1, (rtsys->nbrOfNetworks)); /* Network send */
  else
    ssSetOutputPortWidth(S, 1, 1);

  if (rtsys->nbrOfSchedTasks+rtsys->nbrOfSchedHandlers > 0) 
    ssSetOutputPortWidth(S, 2, rtsys->nbrOfSchedTasks+rtsys->nbrOfSchedHandlers);
  else
    ssSetOutputPortWidth(S, 2, 1);

  if (rtsys->nbrOfSchedMonitors > 0) 
    ssSetOutputPortWidth(S, 3, rtsys->nbrOfSchedMonitors*rtsys->nbrOfTasks);
  else
    ssSetOutputPortWidth(S, 3, 1);

  ssSetOutputPortWidth(S, 4, 1); //Energy consumption

  ssSetNumContStates(S, 0);
  ssSetNumDiscStates(S, 0);
  
  ssSetNumSampleTimes(S, 1);
    
  ssSetNumRWork(S, 0);
  ssSetNumIWork(S, 0);
  ssSetNumPWork(S, 0); 
  ssSetNumModes(S, 0);
  ssSetNumNonsampledZCs(S, 1);

  ssSetUserData(S, rtsys);
  
  ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE | SS_OPTION_CALL_TERMINATE_ON_EXIT); 
}


static void mdlInitializeSampleTimes(SimStruct *S)
{
  //printf("mdlInitSample\n");
  ssSetSampleTime(S, 0, CONTINUOUS_SAMPLE_TIME);
  ssSetOffsetTime(S, 0, FIXED_IN_MINOR_STEP_OFFSET);
}


#define MDL_START
static void mdlStart(SimStruct *S)
{
  //printf("mdlStart\n");
  rtsys = (RTsys*) ssGetUserData(S);

  if (rtsys->init_phase) {
    /* Failure during initialization */
    return;
  } 

  /* DATA ALLOCATION */  
   
  if (rtsys->nbrOfTriggers > 0) {
    rtsys->interruptinputs = new double[rtsys->nbrOfTriggers];
    rtsys->oldinterruptinputs = new double[rtsys->nbrOfTriggers];
  }
  
  if (rtsys->nbrOfTasks > 0) {
    rtsys->taskSched = new double[rtsys->nbrOfTasks];
  }
  
  if (rtsys->nbrOfHandlers > 0) {
    rtsys->handlerSched = new double[rtsys->nbrOfHandlers];
  }

  if (rtsys->nbrOfMonitors > 0) {
    rtsys->monitorGraph = new double[rtsys->nbrOfTasks];
  }
  if (rtsys->nbrOfNetworks > 0) {
    rtsys->nwSnd = new double[rtsys->nbrOfNetworks];
    rtsys->networkinputs = new double[rtsys->nbrOfNetworks];
    rtsys->oldnetworkinputs = new double[rtsys->nbrOfNetworks];
  }
}


#define MDL_INITIALIZE_CONDITIONS
static void mdlInitializeConditions(SimStruct *S)
{
  //printf("mdlInit\n");
  int i;
  rtsys = (RTsys*) ssGetUserData(S);

  if (rtsys->init_phase) {
    /* Failure during initialization */
    return;
  }
  
  for (i=0; i<rtsys->nbrOfInputs; i++) 
    rtsys->inputs[i] = *ssGetInputPortRealSignalPtrs(S,0)[i];
  
  for (i=0; i<rtsys->nbrOfTriggers; i++) {
    rtsys->interruptinputs[i] = 0.0;
    rtsys->oldinterruptinputs[i] = 0.0;
  }
  
  /* NETWORK */
  for (i=0; i<rtsys->nbrOfNetworks; i++) {
    rtsys->nwSnd[i] = 0.0;
    rtsys->networkinputs[i] = 0.0;
    rtsys->oldnetworkinputs[i] = 0.0;
  }
  if (rtsys->nbrOfNetworks > 0) {
    ttInitNetwork2(); /* do the rest of the network initialization */
  }
}


static void mdlOutputs(SimStruct *S, int_T tid)
{
  //printf("mdlOutputs at %g\n", ssGetT(S));
  rtsys = (RTsys*) ssGetUserData(S);

  if (rtsys->init_phase) {
    /* Failure during initialization */
    return;
  }

  real_T *y = ssGetOutputPortRealSignal(S,0);
  real_T *n = ssGetOutputPortRealSignal(S,1);
  real_T *s = ssGetOutputPortRealSignal(S,2);
  real_T *m = ssGetOutputPortRealSignal(S,3);
  real_T *energyConsumption = ssGetOutputPortRealSignal(S,4);
  int i, j, k, detected;
  double dTime; 
 
  DataNode *dn;
  Task* task;
  UserTask* t;
  InterruptHandler* hdl;
  Monitor *mon;

  if (!rtsys->started && ssGetT(S) == 0.0) {
    rtsys->started = true;
    return;
  }

  if (!rtsys->mdlzerocalled) {
    printf("Zero crossing detection must be turned on in order to run TrueTime!\n");
    ssSetErrorStatus(S, "Zero crossing detection must be turned on in order to run TrueTime!");
    return;
  }
  
  /* Storing the time */

  rtsys->time = ssGetT(S) * rtsys->clockDrift + rtsys->clockOffset;
  
  detected = 0;



  /* Check interrupts */
  
  i = 0;
  dn = (DataNode*) rtsys->triggerList->getFirst();
  while (dn != NULL) {
    if (fabs(rtsys->interruptinputs[i]-rtsys->oldinterruptinputs[i]) > 0.1) {
      hdl = (InterruptHandler*) dn->data;
      Trigger* trig = hdl->trigger;
      if (rtsys->time - trig->prevHit > trig->latency) { 
	// Trigger interrupt handler
	if (hdl->myList == rtsys->readyQ) {
	  // handler serving older interrupts
	  hdl->pending++;
	} else {
	  hdl->moveToList(rtsys->readyQ);
	  detected = 1;
	}
 	trig->prevHit = rtsys->time;
      } else { 
	//printf("Call to interrupt handler %s ignored at time %f. Within interrupt latency!\n", hdl->name, rtsys->time);
      }
      rtsys->oldinterruptinputs[i] = rtsys->interruptinputs[i];
    }
    i++;
    dn = (DataNode*) dn->getNext();
  }
  
  /* Check network */
  
  dn = (DataNode*) rtsys->networkList->getFirst();
  while (dn != NULL) {
    hdl = (InterruptHandler*) dn->data;
    Network* network = hdl->network;
    i = network->networkID - 1;
    //printf("mdlOutputs: checking network #%d inp: %d oldinp: %d\n",i,rtsys->networkinputs[i],rtsys->oldnetworkinputs[i]);
    if (fabs(rtsys->networkinputs[i] - rtsys->oldnetworkinputs[i]) > 0.1) {
      hdl->moveToList(rtsys->readyQ);
      detected = 1;
      rtsys->oldnetworkinputs[i] = rtsys->networkinputs[i];
    }
    dn = (DataNode*) dn->getNext();
  }
  
  /* Run kernel? */

  double externTime =  (rtsys->time- rtsys->clockOffset) / rtsys->clockDrift;
  if ((externTime >= rtsys->nextHit) || (detected > 0)) {
    dTime = runKernel(ssGetT(S));
    if (rtsys->error) {
      // Something went wrong executing a code function
      mxArray *bn[1];
      mexCallMATLAB(1, bn, 0, 0, "gcs"); // get current system
      char buf[200];
      mxGetString(bn[0], buf, 200);
      for (unsigned int i=0; i<strlen(buf); i++) if (buf[i]=='\n') buf[i]=' '; 
      printf("In block ==> '%s'\nSimulation aborted!\n", buf); 
      ssSetStopRequested(S, 1);
    } else {
      rtsys->nextHit = (rtsys->time + dTime - rtsys->clockOffset) / rtsys->clockDrift;
    }
  }

  
  /* Outputs */

  for (i=0; i<rtsys->nbrOfOutputs; i++) {
    y[i] = rtsys->outputs[i];
  }
  
  
  /* Network send */

  for (i=0; i<rtsys->nbrOfNetworks; i++) {
    n[i] = rtsys->nwSnd[i];
  }
  
  /* Task schedule */
  
  i = 0;
  j = 0;
  dn = (DataNode*) rtsys->taskList->getFirst();
  while (dn != NULL) {
    t = (UserTask*) dn->data;
    rtsys->taskSched[i] = (double) (j+1);
    if (t->display) j++;
    dn = (DataNode*) dn->getNext();
    i++;
  }
  
  task = (Task*) rtsys->readyQ->getFirst();
  while (task != NULL) {
    if (task->isUserTask()) {
      t = (UserTask*) task;
      rtsys->taskSched[t->taskID - 1] += 0.25;
    }
    task = (Task*) task->getNext();
  }
  
  if ((rtsys->running != NULL) && (rtsys->running->isUserTask())) {
    t = (UserTask*) rtsys->running;
    rtsys->taskSched[t->taskID - 1] += 0.25;
  }    

  i = 0;
  j = 0;
  dn = (DataNode*) rtsys->taskList->getFirst();
  while (dn != NULL) {
    t = (UserTask*) dn->data;
    if (t->display) {
      s[j] = rtsys->taskSched[i];
      j++;
    }
    dn = (DataNode*) dn->getNext();
    i++;
  }
  

  /* Handler schedule */
  
  i = 0;
  j = 0;
  dn = (DataNode*) rtsys->handlerList->getFirst();
  while (dn != NULL) {
    rtsys->handlerSched[i] = (double) (j+rtsys->nbrOfSchedTasks+2);
    if (i==0 && rtsys->contextSwitchTime > EPS) {
      // Context switch schedule, move graph down to task level
      rtsys->handlerSched[i] = rtsys->handlerSched[i] - 1;
    }
    hdl = (InterruptHandler*) dn->data;
    if (hdl->display) j++;
    dn = (DataNode*) dn->getNext();
    i++;
  }

  task = (Task*) rtsys->readyQ->getFirst();
  while (task != NULL) {
    if (!(task->isUserTask())) {
      hdl = (InterruptHandler*) task;
      rtsys->handlerSched[hdl->handlerID - 1] += 0.25;
    }
    task = (Task*) task->getNext();
  }

  if ((rtsys->running != NULL) && (!(rtsys->running->isUserTask()))) {
    hdl = (InterruptHandler*) rtsys->running;
    rtsys->handlerSched[hdl->handlerID - 1] += 0.25;
  }

  i = 0;
  j = 0;
  dn = (DataNode*) rtsys->handlerList->getFirst();
  while (dn != NULL) {
    hdl = (InterruptHandler*) dn->data;
    if (hdl->display) {
      s[j+rtsys->nbrOfSchedTasks] = rtsys->handlerSched[i];
      j++;
    }
    dn = (DataNode*) dn->getNext();
    i++;
  }
  
  /* Monitor graph */
  
  k = 0;
  dn = (DataNode*) rtsys->monitorList->getFirst();
  while (dn != NULL) {
    mon = (Monitor*) dn->data;
    
    for (j=0; j<rtsys->nbrOfTasks; j++)
      rtsys->monitorGraph[j] = (double) (j+1+k*(1+rtsys->nbrOfTasks));
    
    t = (UserTask*) mon->waitingQ->getFirst();
    while (t != NULL) {
      i = t->taskID;
      rtsys->monitorGraph[i-1] += 0.25;
      t = (UserTask*) t->getNext();
    }
    if (mon->heldBy != NULL) {
      i = mon->heldBy->taskID;
      rtsys->monitorGraph[i-1] += 0.5;
    }
    if (mon->display) {
      for (j=0; j<rtsys->nbrOfTasks; j++)
 	m[j+k*rtsys->nbrOfTasks] = rtsys->monitorGraph[j];
      k++;
    }
    dn = (DataNode*) dn->getNext();
  }

  /* Energy consumption */
  energyConsumption[0] = rtsys->energyConsumption;
} 


#define MDL_ZERO_CROSSINGS

static void mdlZeroCrossings(SimStruct *S)
{
  //printf("mdlZeroCrossings at %g\n", ssGetT(S));
  if (!rtsys->mdlzerocalled) {
    rtsys->mdlzerocalled = true;
  }

  int i;
  rtsys = (RTsys*) ssGetUserData(S);
  
  if (rtsys->init_phase) {
    /* Failure during initialization */
    return;
  }

  /* Copy analog inputs */
  InputRealPtrsType inputs = ssGetInputPortRealSignalPtrs(S,0);
  for (i=0; i<rtsys->nbrOfInputs; i++) {
    rtsys->inputs[i] = *inputs[i];
  }

  /* Copy interrupt inputs, check for events */
  inputs = ssGetInputPortRealSignalPtrs(S,1);
  for (i=0; i<rtsys->nbrOfTriggers; i++) {
    if (fabs(*inputs[i]-rtsys->interruptinputs[i]) > 0.1) {
      if (ssGetT(S) < rtsys->nextHit) {
	rtsys->nextHit = ssGetT(S);
      } 
      //printf("mdlZeroCrossings: interrupt detected at %2.20g\n", ssGetT(S));
    }
    rtsys->interruptinputs[i] = *inputs[i];
  }


  /* Copy network input, check for event */
  inputs = ssGetInputPortRealSignalPtrs(S,2);
  for (i=0; i<rtsys->nbrOfNetworks; i++) {
    if (fabs(*inputs[i]-rtsys->networkinputs[i]) > 0.1) {
      if (ssGetT(S) < rtsys->nextHit) {
        rtsys->nextHit = ssGetT(S);
      } 
    }
    rtsys->networkinputs[i] = *inputs[i];
  }

  /* Check the energy level */
  rtsys->energyLevel = *ssGetInputPortRealSignalPtrs(S,3)[0];

  ssGetNonsampledZCs(S)[0] = rtsys->nextHit - ssGetT(S);
}


static void mdlTerminate(SimStruct *S)
{   
  rtsys = (RTsys*) ssGetUserData(S);
  
  if (rtsys == NULL) {
    return;
  }

  if (rtsys->taskList != NULL) {
    // write logs to the MATLAB workspace
    DataNode *dn = (DataNode*) rtsys->taskList->getFirst();
    while (dn != NULL) {
      UserTask *task = (UserTask*) dn->data;
      for (int j=0; j<NBRLOGS; j++) {
	Log *log = task->logs[j];
	if (log) {
	  // printf("Dumping log %d for task %s\n", j, task->name);
	  mxArray *ptr = mxCreateDoubleMatrix(log->entries, 1, mxREAL); 
	  for (int n=0; n<log->entries; n++) {
	    mxGetPr(ptr)[n] = log->vals[n];
	  }
	  mexMakeArrayPersistent(ptr);
	  mexPutVariable("base",log->variable,ptr);
	}
      }
      dn = (DataNode*) dn->getNext();
    }
  }

#ifdef KERNEL_MATLAB  
  if (!destroyed) {
    mxDestroyArray(segArray);
    destroyed = true;
  }

#else
  cleanup();
#endif

  // Delete rtsys and all data structures within
  delete rtsys;

#ifdef KERNEL_MATLAB  
  mxArray* rhs[2];
  rhs[0] = mxCreateString("global");
  rhs[1] = mxCreateString("_rtsys");
  mexCallMATLAB(0, NULL, 2, rhs, "clear"); 
#endif
}


#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif

#ifdef __cplusplus
} // end of extern "C" scope
#endif

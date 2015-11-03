/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef INIT_KERNEL
#define INIT_KERNEL

void ttInitKernel(int nbrOfInputs, int nbrOfOutputs, double (*prioFcn)(UserTask*)) {
  int i;

  if (nbrOfInputs < 0 || nbrOfOutputs < 0) {
    MEX_ERROR("ttInitKernel: invalid number of inputs or outputs!");
    return;
  }

  rtsys->nbrOfInputs = nbrOfInputs;
  rtsys->nbrOfOutputs = nbrOfOutputs;

  if (rtsys->nbrOfInputs > 0) {
    rtsys->inputs = new double[rtsys->nbrOfInputs];
  }

  for (i=0; i<rtsys->nbrOfInputs; i++) {
    rtsys->inputs[i] = 0.0;
  }

  if (rtsys->nbrOfOutputs > 0) {
    rtsys->outputs = new double[rtsys->nbrOfOutputs];
  }
  
  for (i=0; i<rtsys->nbrOfOutputs; i++) {
    rtsys->outputs[i] = 0.0;
  }

  rtsys->prioFcn = prioFcn;

  rtsys->readyQ = new List("ReadyQ", rtsys->prioSort);
  rtsys->timeQ = new List("TimeQ", rtsys->timeSort);

  rtsys->taskList = new List("TaskList", NULL);
  rtsys->handlerList = new List("HandlerList", NULL);
  rtsys->timerList = new List("TimerList", NULL);
  rtsys->monitorList = new List("MonitorList", NULL);
  rtsys->eventList = new List("EventList", NULL);
  rtsys->triggerList = new List("TriggerList", NULL);
  rtsys->mailboxList = new List("MailboxList", NULL);
  rtsys->semaphoreList = new List("SemaphoreList", NULL);
  rtsys->networkList = new List("NetworkList", NULL);
  
  rtsys->initialized = true;
}


void ttInitKernel(int nbrOfInputs, int nbrOfOutputs, int dispatch) {
  int i;

  if (nbrOfInputs < 0 || nbrOfOutputs < 0) {
    MEX_ERROR("ttInitKernel: invalid number of inputs or outputs!");
    return;
  }

  rtsys->nbrOfInputs = nbrOfInputs;
  rtsys->nbrOfOutputs = nbrOfOutputs;

  if (rtsys->nbrOfInputs > 0) {
    rtsys->inputs = new double[rtsys->nbrOfInputs];
  }

  for (i=0; i<rtsys->nbrOfInputs; i++) {
    rtsys->inputs[i] = 0.0;
  }

  if (rtsys->nbrOfOutputs > 0) {
    rtsys->outputs = new double[rtsys->nbrOfOutputs];
  }
  
  for (i=0; i<rtsys->nbrOfOutputs; i++) {
    rtsys->outputs[i] = 0.0;
  }

  switch (dispatch) {
  case FP:
    rtsys->prioFcn = rtsys->prioFP;
    break;
  case RM:
    rtsys->prioFcn = rtsys->prioRM;
    break;
  case EDF:
    rtsys->prioFcn = rtsys->prioEDF;
    break;
  case DM:
    rtsys->prioFcn = rtsys->prioDM;
    break;
  default:
    MEX_ERROR("ttInitKernel: Invalid priority function!");
    return;
  }

  rtsys->readyQ = new List("ReadyQ", rtsys->prioSort);
  rtsys->timeQ = new List("TimeQ", rtsys->timeSort);

  rtsys->taskList = new List("TaskList", NULL);
  rtsys->handlerList = new List("HandlerList", NULL);
  rtsys->timerList = new List("TimerList", NULL);
  rtsys->monitorList = new List("MonitorList", NULL);
  rtsys->eventList = new List("EventList", NULL);
  rtsys->triggerList = new List("TriggerList", NULL);
  rtsys->mailboxList = new List("MailboxList", NULL);
  rtsys->semaphoreList = new List("SemaphoreList", NULL);
  rtsys->networkList = new List("NetworkList", NULL);
  
  rtsys->initialized = true;

}

// Creates an interrupt handler to simulate context switches
InterruptHandler *createCShandler(void) {

  InterruptHandler* hdl = new InterruptHandler("CShandler");
  
  hdl->handlerID = rtsys->nbrOfHandlers + 1;
  hdl->codeFcn = rtsys->contextSwitchCode;

  hdl->handlerID = rtsys->nbrOfHandlers + 1;
  hdl->priority = -2000.0;
  hdl->display = true;

  rtsys->handlerList->appendNode(new DataNode(hdl, NULL));
  
  rtsys->nbrOfSchedHandlers++;
  rtsys->nbrOfHandlers++;
  
  return hdl;
}

void ttInitKernel(int nbrOfInputs, int nbrOfOutputs, double (*prioFcn)(UserTask*), double contextSwitchOH) {  

  ttInitKernel(nbrOfInputs, nbrOfOutputs, prioFcn);

  if (contextSwitchOH < 0.0) {
    MEX_ERROR("ttInitKernel: Invalid context switch overhead!");
    return;
  }

  if (contextSwitchOH > EPS) {
    rtsys->contextSwitchTime = contextSwitchOH;
    rtsys->kernelHandler = createCShandler();
  }
}

void ttInitKernel(int nbrOfInputs, int nbrOfOutputs, int dispatch, double contextSwitchOH) {  

  ttInitKernel(nbrOfInputs, nbrOfOutputs, dispatch);

  if (contextSwitchOH < 0.0) {
    MEX_ERROR("ttInitKernel: Invalid context switch overhead!");
    return;
  }

  if (contextSwitchOH > EPS) {
    rtsys->contextSwitchTime = contextSwitchOH;
    rtsys->kernelHandler = createCShandler();
  }

}

#endif

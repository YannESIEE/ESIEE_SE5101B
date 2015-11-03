/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef PRIO_FUNCTIONS
#define PRIO_FUNCTIONS

double prioFP(UserTask* t) {

  return t->priority;
}

double prioRM(UserTask* t) {
  
  if (t->periodichandler != NULL) {
    return t->periodichandler->timer->period;
  } else {
    printf("Rate-monotonic scheduling not applicable to non-periodic tasks. Priority value for task '%s' set to 1.\n",t->name);
    return 1.0;
  }
}

double prioDM(UserTask* t) {

  return t->deadline;
}

double prioEDF(UserTask* t) {
  
  return t->absDeadline;
}

#endif

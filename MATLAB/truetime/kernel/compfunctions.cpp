/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef COMP_FUNCTIONS
#define COMP_FUNCTIONS

// Sorts tasks according to their wakeup time
int timeSort(Node* n1, Node* n2) {
  
  double cmp1, cmp2;
  Task* t1 = (Task*) n1;
  Task* t2 = (Task*) n2;

  cmp1 = t1->wakeupTime();
  cmp2 = t2->wakeupTime();

  return (cmp1 < cmp2) ? 1 : -1;
}

// Sorts tasks according to the selected priority function
// see priofunctions.cpp for standard scheduling policies
int prioSort(Node* n1, Node* n2) {

  double cmp1, cmp2;
  Task* t1 = (Task*) n1;
  Task* t2 = (Task*) n2;
  UserTask* u1;
  UserTask* u2;

  if (t1->isUserTask()) {
    if (t2->isUserTask()) {
      u1 = (UserTask*) t1;
      u2 = (UserTask*) t2;
      if ((u1->state == RUNNING || (u1->state == SUSPENDED))
	  && !u1->isPreemptable) {
	return 1;
      }
      if ((u2->state == RUNNING || (u2->state == SUSPENDED))
	  && !u2->isPreemptable) {
	return -1;
      }
      cmp1 = (u1->prioRaised) ? u1->tempPrio : rtsys->prioFcn(u1);
      cmp2 = (u2->prioRaised) ? u2->tempPrio : rtsys->prioFcn(u2);
    } else {
      return -1;
    }
  } else {
    // t1 is a handler
    if (t2->isUserTask()) {
      return 1;
    } else {
      cmp1 = ((InterruptHandler*) n1)->priority;
      cmp2 = ((InterruptHandler*) n2)->priority;
    }
  }
  
  return (cmp1 < cmp2) ? 1 : -1;
  
}

#endif

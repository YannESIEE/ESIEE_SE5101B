/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef NOTIFY_ALL
#define NOTIFY_ALL

#include "getnode.cpp"

void ttNotify(char *nameOfEvent) {
  
  DataNode* dn;
  Event* ev;
  List* newQ;
  int newState;
  UserTask* task;

  dn = getNode(nameOfEvent, rtsys->eventList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttNotify: Non-existent event '%s'!", nameOfEvent);
    MEX_ERROR(buf);
    return;
  }

  // Event exists 
  ev = (Event*) dn->data;
  
  // move FIRST waiting task to readyQ (if free event)
  // or to waitingQ of associated monitor
  if (ev->isFree) {
    newQ = rtsys->readyQ;
    newState = READY;
  } else {
    newQ = ev->mon->waitingQ;
    newState = WAITING;
  }

  task = (UserTask*) ev->waitingQ->getFirst();
  if (task != NULL) {
    task->moveToList(newQ);
    task->state = newState;
  }
}

void ttNotifyAll(char *nameOfEvent) {
  
  DataNode* dn;
  Event* ev;
  Node *n, *tmp;
  List* newQ;
  int newState;
  UserTask* task;

  dn = getNode(nameOfEvent, rtsys->eventList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttNotifyAll: Non-existent event '%s'!", nameOfEvent);
    MEX_ERROR(buf);
    return;
  }

  // Event exists 
  ev = (Event*) dn->data;
  
  // move ALL waiting tasks to readyQ (if free event)
  // or to waitingQ of associated monitor
  if (ev->isFree) {
    newQ = rtsys->readyQ;
    newState = READY;
  } else {
    newQ = ev->mon->waitingQ;
    newState = WAITING;
  }

  n = ev->waitingQ->getFirst();
  while (n != NULL) {
    tmp = n;
    n = n->getNext();
    tmp->moveToList(newQ);
    task = (UserTask*) tmp;
    task->state = newState;
  }
}

#endif

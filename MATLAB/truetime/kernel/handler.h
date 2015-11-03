/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef HANDLER_H
#define HANDLER_H

/**
 * InterruptHandler class, inherits from Task
 */
class InterruptHandler : public Task {
 public:
  double priority;
  
  int handlerID;   // Used for schedule 
  bool display;
  
  // ----------------------
  int type;           // interrupt identifier

  UserTask *usertask; // if overrun handler to task

  Timer* timer;       // if associated with timer interrupt 
                      // (user-defined or overrun timer)

  Network* network;   // if associated with network receive interrupt

  Trigger* trigger;   // if associated with external interrupt
  int pending;        // list of pending invocations, if new external
                      // interrupt occurs before the old is served
  // ----------------------

  bool isUserTask();
  double wakeupTime();
  void print();

  InterruptHandler(char *n);
  ~InterruptHandler();
};

/**
 * InterruptHandler Constructor
 */
InterruptHandler::InterruptHandler(char *n) 
  : Task(n)
{
  type = UNUSED;
  usertask = NULL;
  timer = NULL;
  network = NULL;
  trigger = NULL;
  pending = 0;
}

/**
 * InterruptHandler Destructor
 */
InterruptHandler::~InterruptHandler() {
  if (timer) {
    delete timer;
  }
  if (network) {
    delete network;
  }
  if (trigger) {
    delete trigger;
  }
}

bool InterruptHandler::isUserTask() {
  return false;
}

double InterruptHandler::wakeupTime() {
  if (timer != NULL) {
    return timer->time;
  } else {
    return 0.0;
  }
}

void InterruptHandler::print() {
  printf("HANDLER (name: %s #%d prio: %f time: %f)", name,handlerID,priority,wakeupTime());
}

#endif

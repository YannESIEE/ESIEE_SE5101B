/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef EVENT_H
#define EVENT_H

class Event {
 public:
  char* name;
  bool isFree;
  Monitor* mon;
  List* waitingQ;

  Event(char *n, bool free, Monitor* monitor);
  ~Event();
};

/**
 * Event Constructor 
 */
Event::Event(char *n, bool free, Monitor* monitor) {
  if (n==NULL) {
    name = NULL;
  } else {
    name = new char[strlen(n)+1];
    strcpy(name, n);
  }
  isFree = free;
  mon = monitor;
  waitingQ = NULL;
}

/**
 * Event Destructor 
 */
Event::~Event() {
  if (name) {
    delete[] name;
  }
  if (waitingQ) delete waitingQ;
}

#endif

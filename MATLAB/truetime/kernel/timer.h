/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef TIMER_H
#define TIMER_H

class Timer {
 public:
  char* name;
  double time;
  double period;
  bool isPeriodic;
  
  Timer(char *n);
  ~Timer();
};

/**
 * Timer Constructor 
 */
Timer::Timer(char *n) {
  if (n==NULL) {
    name = NULL;
  } else {
    name = new char[strlen(n)+1];
    strcpy(name, n);
  }
}

/**
 * Timer Destructor 
 */
Timer::~Timer() {
  if (name)
    delete[] name;
}

#endif




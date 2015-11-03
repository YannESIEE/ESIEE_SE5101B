/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

class Semaphore {
 public:
  char* name;
  int value;
  int maxval;
  List *waitingQ;

  Semaphore(char *n);
  ~Semaphore();
};

/**
 * Semaphore Constructor 
 */
Semaphore::Semaphore(char *n) {
  if (n==NULL) {
    name = NULL;
  } else {
    name = new char[strlen(n)+1];
    strcpy(name, n);
  }
  value = 0;
  maxval = 0;
  waitingQ = NULL;
}

/**
 * Semaphore Destructor 
 */
Semaphore::~Semaphore() {
  if (name) {
    delete[] name;
  }
  if (waitingQ) delete waitingQ;
}

#endif

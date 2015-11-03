/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef LOG_H
#define LOG_H

class Log {
 public:
  char *variable;
  int size; 
  int entries;
  int tempIndex;
  double *vals;
  double temp;

  Log(char *var, int s);
  ~Log();
};

/**
 * Log Constructor 
 */
Log::Log(char *var, int s) {
  if (var==NULL) {
    variable = NULL;
  } else {
    variable = new char[strlen(var)+1];
    strcpy(variable, var);
  }
  size = s; 
  entries = 0;
  tempIndex = 0; 
  vals = new double[size];
  for (int i=0; i<size; i++) {
    vals[i] = 0.0;
  }
  temp = 0.0;
}

/**
 * Log Destructor 
 */
Log::~Log() {
  if (variable) {
    delete[] variable;
  }
  if (vals) delete[] vals;
}

#endif

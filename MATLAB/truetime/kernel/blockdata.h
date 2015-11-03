/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef BLOCKDATA_H
#define BLOCKDATA_H

class Blockdata {
 public:
  char *blockName;
  mxArray *options;

  Blockdata(char *n);
  ~Blockdata();
};

/**
 * Blockdata Constructor 
 */
Blockdata::Blockdata(char *n) {
  if (n==NULL) {
    blockName = NULL;
  } else {
    blockName = new char[strlen(n)+1];
    strcpy(blockName, n);
  }
  options = NULL;
}

/**
 * Blockdata Destructor 
 */
Blockdata::~Blockdata() {
  if (blockName) {
    delete[] blockName;
  }
  mxDestroyArray(options);
}

#endif

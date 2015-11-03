/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef ANALOG_OUT
#define ANALOG_OUT

void ttAnalogOut(int outputChan, double value) {
  
  if (outputChan < 1 || outputChan > rtsys->nbrOfOutputs) {
    MEX_ERROR("ttAnalogOut: outpChan out of bounds");
    return;
  }

  rtsys->outputs[outputChan-1] = value;
}

#endif

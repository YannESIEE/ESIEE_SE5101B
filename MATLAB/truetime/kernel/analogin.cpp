/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef ANALOG_IN
#define ANALOG_IN

double ttAnalogIn(int inputNbr) {

  if (inputNbr < 1 || inputNbr > rtsys->nbrOfInputs) {
    MEX_ERROR("ttAnalogIn: inpChan out of bounds");
    return 0.0;
  }

  return rtsys->inputs[inputNbr-1];
}

#endif

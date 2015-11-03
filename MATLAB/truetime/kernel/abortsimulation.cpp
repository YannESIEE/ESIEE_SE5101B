/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef ABORT_SIMULATION
#define ABORT_SIMULATION

void ttAbortSimulation() {

  rtsys->error = true;  // set the error flag so TrueTime will stop

}

#endif

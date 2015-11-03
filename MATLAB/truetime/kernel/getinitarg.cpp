/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef GET_INIT_ARG
#define GET_INIT_ARG

mxArray *ttGetInitArg() {

  return rtsys->initarg;

}

#endif

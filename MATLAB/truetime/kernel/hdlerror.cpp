/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef HDL_ERROR
#define HDL_ERROR

// Display error message when trying to use already assigned
// interrupt handler
void handler_error(InterruptHandler* hdl, char* pre) {

  char buf[200];

  switch (hdl->type) {
  case OVERRUN:
    sprintf(buf, "%s handler '%s' already used as overrun handler for task '%s'!", pre, hdl->name, hdl->usertask->name);
    MEX_ERROR(buf);
    return;
  case TIMER:
    sprintf(buf, "%s handler '%s' already associated with timer '%s'!", pre, hdl->name, hdl->timer->name);
    MEX_ERROR(buf);
    return;
  case NETWORK:
    sprintf(buf, "%s handler '%s' already used as network handler!", pre, hdl->name);
    MEX_ERROR(buf);
    return;
  case EXTERNAL:
    sprintf(buf, "%s handler '%s' already associated with external interrupt!",pre, hdl->name);
    MEX_ERROR(buf);
    return;
  }
}

#endif

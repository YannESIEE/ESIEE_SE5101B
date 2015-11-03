/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef ATTACH_PRIO_FCN
#define ATTACH_PRIO_FCN

void ttAttachPrioFcn(double (*prioFcn)(UserTask*)) {

  printf("Warning: ttAttachPrioFcn has been deprecated. Use ttInitKernel instead.\n");

  rtsys->prioFcn = prioFcn;

}

#endif

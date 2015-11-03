/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

extern RTsys* rtsys;

RTsys *getrtsys()
{

  RTsys *rtsys;

  mxArray *var = (mxArray*)mexGetVariablePtr("global", "_rtsys");
  if (var == NULL) {
    printf("_rtsys not found!\nKernel primitives cannot be called from the command line!\n");
    return NULL;
  }

  rtsys = (RTsys *)(*((long *)mxGetPr(var)));

  return rtsys;
}

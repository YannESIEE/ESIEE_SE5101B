/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#define KERNEL_MATLAB
#include "../ttkernel.h" 

RTsys *rtsys;

#include "../invokingtask.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  rtsys = getrtsys() ; // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs != 0) {
    printf("ttInvokingTask: Input arguments ignored!\nUsage: ttInvokingTask\n");
  }

  char* retval = ttInvokingTask();

  if (retval == NULL) {
    plhs[0] = mxCreateDoubleMatrix(0,0,mxREAL); // Return empty matrix
  } else {
    plhs[0] = mxCreateString(retval);
  }

}

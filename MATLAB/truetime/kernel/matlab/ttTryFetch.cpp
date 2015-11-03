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

#include "../tryfetch.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  rtsys = getrtsys() ; // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs != 1) {
    MEX_ERROR("ttTryFetch: Wrong number of input arguments!\nUsage: ttTryFetch(mailboxname)");
    return;
  }

  if (mxIsChar(prhs[0]) != 1) {
    MEX_ERROR("ttTryFetch: mailboxname must be a string");
    return;
  }
  
  char mailboxname[100];
  mxGetString(prhs[0], mailboxname, 100);
  mxArray* tmp = (mxArray *) ttTryFetch(mailboxname);

  if (tmp == NULL) {
    plhs[0] = mxCreateDoubleMatrix(0,0,mxREAL); // Return empty matrix
  } else {
    mxArray *msg = mxDuplicateArray(tmp);
    mxDestroyArray(tmp);
    plhs[0] = msg;
  }
}

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

#include "../post.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  rtsys = getrtsys() ; // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs != 2) {
    MEX_ERROR("ttPost: Wrong number of input arguments!\nUsage: ttPost(mailboxname, msg)");
    return;
  }

  if (mxIsChar(prhs[0]) != 1) {
    MEX_ERROR("ttPost: mailboxname must be a string");
    return;
  }

  char mailboxname[100];
  mxGetString(prhs[0], mailboxname, 100);

  mxArray *data = mxDuplicateArray(prhs[1]);
  mexMakeArrayPersistent(data);

  ttPost(mailboxname, (void *)data);
}

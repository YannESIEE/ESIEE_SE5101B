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

#include "../retrieve.cpp"
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
    MEX_ERROR("ttRetrieve: Wrong number of input arguments!\nUsage: ttRetrieve(mailboxname)");
    return;
  }

  if (mxIsChar(prhs[0]) != 1) {
    MEX_ERROR("ttRetrieve: mailboxname must be a string");
    return;
  }
  
  char mailboxname[100];
  mxGetString(prhs[0], mailboxname, 100);
  
  mxArray* data;
  mxArray* msg = (mxArray *) ttRetrieve(mailboxname);
  if ( msg!=NULL ){
    data = mxDuplicateArray((mxArray *) msg);
    plhs[0] = data;
    // Delete message
    mxDestroyArray((mxArray *) msg); 
  } else {
    // A message was already fetched but not retrieved!
    MEX_ERROR("ttRetrieve: No message to retrieve!\n");
    plhs[0] = mxCreateDoubleMatrix(0,0,mxREAL); // Return empty matrix (error)
  }

}

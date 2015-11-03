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

#include "../createhandler.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  rtsys = getrtsys() ; // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs < 3 || nrhs > 4) {
    MEX_ERROR("ttCreateInterruptHandler: Wrong number of input arguments!\nUsage: ttCreateInterruptHandler(name, priority, codefcn) \n       ttCreateInterruptHandler(name, priority, codefcn, data)");
    return;
  }

  if (mxIsChar(prhs[0]) != 1) {
    MEX_ERROR("ttCreateInterruptHandler: name must be a string");
    return;
  }
  if (!mxIsDouble(prhs[1])) {
    MEX_ERROR("ttCreateInterruptHandler: priority must be a number");
    return;
  }
  if (mxIsChar(prhs[2]) != 1 || mxGetM(prhs[2]) != 1) {
    MEX_ERROR("ttCreateInterruptHandler: codeFcn must be a non-empty string");
    return;
  }
  
  char name[100];
  mxGetString(prhs[0], name, 100);
  
  double priority = *mxGetPr(prhs[1]);

  char codeFcn[100];
  mxGetString(prhs[2], codeFcn, 100);

  // Make sure that the code function exists in Matlab path
  // and that the code function is syntactically correct.
  mxArray *lhs[1];
  mxArray *rhs[1];
  rhs[0] = mxDuplicateArray(prhs[2]);
  mexCallMATLAB(1, lhs, 1, rhs, "exist");
  int number = (int) *mxGetPr(lhs[0]);
  if (number == 0) {
    char buf[200];
    sprintf(buf, "ttCreateInterruptHandler: codeFcn '%s' not in path! Interrupt handler '%s' not created!\n", codeFcn, name);
    MEX_ERROR(buf);
    return;
  }

  // Create handler
  if (ttCreateInterruptHandler(name, priority, NULL)) {

    // Add name of code function (m-file) and data variable
    DataNode *n = (DataNode*) rtsys->handlerList->getLast();
    InterruptHandler* hdl = (InterruptHandler*) n->data;

    hdl->codeFcnMATLAB = new char[strlen(codeFcn)+1];
    strcpy(hdl->codeFcnMATLAB, codeFcn);
     
    if (nrhs == 4) { // data specified
      mxArray* data = mxDuplicateArray(prhs[3]);
      mexMakeArrayPersistent(data);
      hdl->dataMATLAB = data;
    }
  }
}

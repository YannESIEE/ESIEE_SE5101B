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

#include "../initkernel.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  rtsys = getrtsys(); // Get pointer to rtsys 
  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs < 3 || nrhs > 4) {
    MEX_ERROR("ttInitKernel: Wrong number of input arguments! \nUsage: ttInitKernel(nbrInputs, nbrOutputs, prioFcn) or\n       ttInitKernel(nbrInputs, nbrOutputs, prioFcn, contextSwitchOH)");
    return;
  }
  if (!mxIsDoubleScalar(prhs[0])) {
    MEX_ERROR("ttInitKernel: nbrInputs must be an integer");
    return;
  }
  if (!mxIsDoubleScalar(prhs[1])) {
    MEX_ERROR("ttInitKernel: nbrOutputs must be an integer");
    return;
  }
  if (mxIsChar(prhs[2]) != 1 || mxGetM(prhs[2]) != 1) {
    MEX_ERROR("ttInitKernel: prioFcn must be a string");
    return;
  }
  if (nrhs == 4) {
    if (!mxIsDoubleScalar(prhs[3])) {
      MEX_ERROR("ttInitKernel: contextSwitchOH must be a number");
      return;
    }
  }

  int nbrInp = (int) *mxGetPr(prhs[0]);
  int nbrOutp = (int) *mxGetPr(prhs[1]);
  
  if ( nbrInp < 0 ) {
    MEX_ERROR("ttInitKernel: nbrInputs must be positive or zero");
    return;
  }
  if ( nbrOutp < 0 ) {
    MEX_ERROR("ttInitKernel: nbrOutputs must be positive or zero");
    return;
  }

  char buf[20];
  mxGetString(prhs[2], buf, 20);

  int dispatch;
  if (strcmp(buf, "prioFP") == 0) {
    dispatch = FP;
  } else if (strcmp(buf, "prioRM") == 0) {
    dispatch = RM;
  } else if (strcmp(buf, "prioDM") == 0){
    dispatch = DM;
  } else if (strcmp(buf, "prioEDF") == 0) {
    dispatch = EDF;
  } else {
    printf("ttInitKernel: Unknown priority function '%s', using fixed priority scheduling!\n",buf);
    dispatch = FP; // default 
  }
  
  if (nrhs == 3) {
    ttInitKernel(nbrInp, nbrOutp, dispatch);
  } else {
    double contextSwitchOH = *mxGetPr(prhs[3]);
    ttInitKernel(nbrInp, nbrOutp, dispatch, contextSwitchOH);
  }
}


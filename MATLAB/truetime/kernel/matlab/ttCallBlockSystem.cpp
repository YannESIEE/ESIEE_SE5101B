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

#include "../callblocksystem.cpp"
#include "getrtsys.cpp"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  rtsys = getrtsys() ; // Get pointer to rtsys 

  if (rtsys==NULL) {
    return;
  }

  // Check number and type of arguments. 
  if (nrhs != 3) {
    MEX_ERROR("ttCallBlockSystem: Wrong number of input arguments!\nUsage: ttCallBlockSystem(nbrOutp, inpVec, blockname)");
    return;
  }

  if (!mxIsDoubleScalar(prhs[0])) {
    MEX_ERROR("ttCallBlockSystem: nbrOutp must be a number");
    return;
  }
  if (!mxIsDouble(prhs[1])) {
    MEX_ERROR("ttCallBlockSystem: inpVec must contain numbers");
    return;
  }
  if (mxIsChar(prhs[2]) != 1 || mxGetM(prhs[2]) != 1) {
    MEX_ERROR("ttCallBlockSystem: blockname must be a non-empty string");
    return;
  }

  int nbrOutp = (int) *mxGetPr(prhs[0]);
  double* outpVec = new double[nbrOutp];

  int nbrInp = mxGetNumberOfElements(prhs[1]);
  double* inpVec = mxGetPr(prhs[1]);
  
  char blockname[100];
  mxGetString(prhs[2], blockname, 100);

  ttCallBlockSystem(nbrOutp, outpVec, nbrInp, inpVec, blockname);

  plhs[0] = mxCreateDoubleMatrix(1, nbrOutp, mxREAL);
  // Copy values (will be zeros if call failed)
  for (int i=0; i<nbrOutp; i++) {
    mxGetPr(plhs[0])[i] = outpVec[i];
  }
  
  delete[] outpVec;

}


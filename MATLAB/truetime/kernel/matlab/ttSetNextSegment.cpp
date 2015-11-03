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

#include "../setnextsegment.cpp"
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
    MEX_ERROR("ttSetNextSegment: Wrong number of input arguments!\nUsage: ttSetNextSegment(segment)");
    return;
  }
  
  if (!mxIsDoubleScalar(prhs[0])) {
    MEX_ERROR("ttSetNextSegment: segment must be an integer");
    return;
  }

  int segment = (int) *mxGetPr(prhs[0]);

  ttSetNextSegment(segment);

}

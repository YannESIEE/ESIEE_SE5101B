/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef __MEX_HELP_H__
#define __MEX_HELP_H__

// This function should exist...
#define mxIsDoubleScalar(M) (mxIsDouble(M) && !mxIsComplex(M) && mxGetM(M)==1 && mxGetN(M)==1)

// Handle errors in kernel primitives
#define MEX_ERROR(e) rtsys->error = true; printf("??? %s\n\n", e)

// Uncomment lines below for backwards compatibility with R12.1
//#define mexPutVariable(w,n,d) mxSetName(d,n); mexPutArray(d,w)
//#define mexGetVariable(w,n) mexGetArray(n,w)
//#define mexGetVariablePtr(w,n) mexGetArrayPtr(n,w)

#endif

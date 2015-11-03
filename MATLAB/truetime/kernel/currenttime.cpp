/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef CURRENT_TIME
#define CURRENT_TIME

double ttCurrentTime(void) 
{
  if (rtsys->time > EPS) 
    return rtsys->time;
  else
    return 0.0;
}

/**
 * Set the new time and return the old one
 */
double ttCurrentTime(double newTime) 
{
  double oldTime = rtsys->time;

  rtsys->clockOffset = rtsys->clockOffset + newTime - rtsys->time;
  //printf("New clockOffset: %f\n", rtsys->clockOffset);
  rtsys->time = newTime;

  return oldTime;
}

#endif

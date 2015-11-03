/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef SET_KERNEL_PARAMETER
#define SET_KERNEL_PARAMETER

void ttSetKernelParameter(char* parameter, double value)
{
  if (strcmp(parameter, "energyconsumption")==0){
    rtsys->energyConsumption = value;
  } else if (strcmp(parameter, "cpuscaling")==0){
    rtsys->cpuScaling = value;
  } else {
    printf("Configuration of parameter %s is not implemented\n", parameter);
  }
}

#endif

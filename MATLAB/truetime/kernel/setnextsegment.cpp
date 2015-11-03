/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef SET_NEXT_SEGMENT
#define SET_NEXT_SEGMENT

void ttSetNextSegment(int seg) {

  rtsys->running->segment = seg-1;
}

#endif

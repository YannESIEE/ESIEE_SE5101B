/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef TRIGGER_H
#define TRIGGER_H

class Trigger {
 public:
  double latency;
  double prevHit;

  Trigger(double lat);
};

/**
 * Trigger Constructor 
 */
Trigger::Trigger(double lat) {

  latency = lat;
  prevHit = -1.0;
}

#endif

/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef DATANODE_H
#define DATANODE_H

/**
 * Generic node class containing arbitrary data
 * Used for all object lists in the kernel.
 */

class DataNode : public Node {
public:
  void* data;
  char* name;

  void print();
  DataNode(void *d, char* n);
  DataNode(double n);
};

DataNode::DataNode(void* d, char* n) {
  data = d;
  name = n; // pointer to task name, timer name etc
}

void DataNode::print() {
  printf(" %s", name);
  return;
}

#endif

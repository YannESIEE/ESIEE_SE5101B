/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef TASK_H
#define TASK_H

/**
 * General Task class, inherits from Node
 * super-class to UserTask and InterruptHandler
 */

class Task : public Node {
 public:
  char* name;
  double execTime;
  int segment;
  
  void *data;       // task data in C++ case
  mxArray * dataMATLAB; // variable for task data
  void *mb_data;    // temporary mailbox data (void* (C++) or char* (Matlab))
  double (*codeFcn)(int, void*); // Code function written in C++
  char* codeFcnMATLAB;  // Name of m-file code function
  
  List *blockList; // To store options mexArrays for block system calls 

  virtual bool isUserTask() = 0;
  virtual double wakeupTime() = 0;
  virtual void print() = 0;
  Task(char *n);
  virtual ~Task();
};

/**
 * Task Constructor 
 */
Task::Task(char *n) {
  if (n==NULL) {
    name = "NoName";
  } else {
    name = new char[strlen(n)+1];
    strcpy(name, n);
  }

  execTime = 0.0;
  segment = 0;
  data = NULL;
  dataMATLAB = NULL;
  codeFcn = NULL;
  codeFcnMATLAB = NULL;
  mb_data = NULL;
  blockList = new List("BlockList", NULL);
}

/**
 * Task Destructor 
 */
Task::~Task() {
  if (name) delete[] name;
  if (codeFcnMATLAB) delete[] codeFcnMATLAB;
  if (dataMATLAB) {
    mxDestroyArray(dataMATLAB);
  }

#ifdef KERNEL_MATLAB
if (mb_data) {
	mxDestroyArray((mxArray *)mb_data);
  }
#endif

}

#endif

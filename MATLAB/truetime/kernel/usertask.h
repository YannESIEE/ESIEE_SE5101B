/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef USER_TASK_H
#define USER_TASK_H

class InterruptHandler;

/**
 * UserTask class, inherits from Task
 */
class UserTask : public Task {
public:
  double priority; 
  double wcExecTime;
  double deadline;

  int state;  // Task state (IDLE; WAITING; SLEEPING; READY; RUNNING)
  int taskID; // Used for schedule
  bool isPreemptable;
  bool display; 
  
  double lastStart; // last time the task started to execute
  double absDeadline; 
  double release;   // task release time if in timeQ
  double budget;

  bool prioRaised;  // Implementing priority inheritance
  double tempPrio;

  int nbrJobs;
  List *pending; // list of pending releases

  InterruptHandler* periodichandler;  // timer handler generating periodic jobs
  InterruptHandler* deadlineORhandler; 
  InterruptHandler* exectimeORhandler; 
  
  int nbrOfUserLogs;  // Number of user-created log entries 
  Log* logs[NBRLOGS];
  
  void (*arrival_hook)(UserTask*);  
  void (*release_hook)(UserTask*);
  void (*start_hook)(UserTask*);
  void (*suspend_hook)(UserTask*);
  void (*resume_hook)(UserTask*);
  void (*finish_hook)(UserTask*);
 
  bool isUserTask();
  double wakeupTime();
  void print();

  UserTask(char *n);
  ~UserTask();
};

/**
 * UserTask Constructor 
 */
UserTask::UserTask(char *n) 
  : Task(n)
{
  nbrJobs = 0;
  prioRaised = false;
  tempPrio = 0.0;
  pending = new List("Pending", NULL);
  periodichandler   = NULL;
  deadlineORhandler = NULL;
  exectimeORhandler = NULL;
  nbrOfUserLogs = 0;
  for (int i=0; i < NBRLOGS; i++) {
    logs[i] = NULL;
  }
}

/**
 * UserTask Destructor 
 */
UserTask::~UserTask() {
  DataNode *dn, *tmp;

  dn = (DataNode*) pending->getFirst();
  while (dn != NULL) {
    double* r = (double*) dn->data;
    delete r;
    tmp = dn;
    dn = (DataNode*) dn->getNext();
    delete tmp;
  }
  delete pending;

  dn = (DataNode*) blockList->getFirst();
  while (dn != NULL) {
    Blockdata* bd = (Blockdata*) dn->data;
    delete bd;
    tmp = dn;
    dn = (DataNode*) dn->getNext();
    delete tmp;
  }
  delete blockList;
  for (int i=0; i < NBRLOGS; i++) {
    if (logs[i] != NULL) {
      delete logs[i];
    }
  }
}

bool UserTask::isUserTask() {
  return true;
}

double UserTask::wakeupTime() {
  return release;
}

void UserTask::print() {
  printf("USERTASK (name: %s prio: %f dl: %f rel.: %f raised prio: %f)", name,priority,deadline,release,tempPrio);
}

#endif

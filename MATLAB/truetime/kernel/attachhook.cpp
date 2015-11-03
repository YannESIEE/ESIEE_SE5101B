/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef ATTACH_HOOK
#define ATTACH_HOOK

#include "getnode.cpp"

void ttAttachHook(char* nameOfTask, int ID, void (*hook)(UserTask*)) {

  DataNode* dn = getNode(nameOfTask, rtsys->taskList);
  UserTask* task = (UserTask*) dn->data; 

  if (task == NULL)
    printf("ttAttachHook: Non-existent task '%s'\n", nameOfTask);
  else {
    switch (ID) {
    case ARRIVAL:
      task->arrival_hook = hook;
      break;
    case RELEASE:
      task->release_hook = hook;
      break;
    case START:
      task->start_hook = hook;
      break;
    case SUSPEND:
      task->suspend_hook = hook;
      break;
    case RESUME:
      task->resume_hook = hook;
      break;
    case FINISH:
      task->finish_hook = hook;
      break;
    default:
      printf("ttAttachHook: Unknown identifier, no hook attached!\n");
    }
  }
}


#endif

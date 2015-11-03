/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef RETRIEVE
#define RETRIEVE

#include "getnode.cpp"

void* ttRetrieve(char* mailbox) {

  DataNode* dn;
  void* msg;

  dn = getNode(mailbox, rtsys->mailboxList);
  if (dn == NULL) {
    // Mailbox does not exist
    char buf[200];
    sprintf(buf, "ttRetrieve: Non-existent mailbox '%s'\n", mailbox);
    MEX_ERROR(buf);
    return NULL;
  }

  UserTask* task = (UserTask*) rtsys->running;
  msg = task->mb_data;   // NULL is a valid message
  task->mb_data = NULL;

  return msg;
}
#endif


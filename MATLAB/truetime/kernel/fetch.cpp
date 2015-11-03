/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef FETCH
#define FETCH

#include "getnode.cpp"

void ttFetch(char* mailbox) {

  DataNode* dn = getNode(mailbox, rtsys->mailboxList);
  if (dn == NULL) {
    // Mailbox does not exist 
    char buf[200];
    sprintf(buf, "ttFetch: Non-existent mailbox '%s'\n", mailbox);
    MEX_ERROR(buf);
    return;
  }

  UserTask* task = (UserTask*) rtsys->running;
  if (task->mb_data != NULL) {
    // A message was already fetched but not retrieved!
    MEX_ERROR("ttFetch: Previously fetched message was not retrieved!\n");
    return;
  }

  Mailbox* m = (Mailbox*) dn->data;
  if (m->count == 0) {
    //printf("ttFetch: Mailbox '%s' is empty, blocking\n", mailbox);
    // block the fetching task
    task->moveToList(m->readerQ);
    task->state = WAITING;
    // Execute suspend hook
    task->suspend_hook(task);
  } else {
  
    dn = (DataNode*) m->buffer->getFirst();
    task->mb_data = (void*)dn->data;  // save it for later (ttRetrieve)
    m->buffer->deleteNode(dn);
    m->count--;
  }
  
  // release first waiting writer, if any
  UserTask* task2 = (UserTask*) m->writerQ->getFirst();
  if (task2 != NULL) {
    m->buffer->appendNode(new DataNode(task2->mb_data, NULL));
    m->count++;   // number of messages
    task2->moveToList(rtsys->readyQ);
    task2->state = READY;
  }
}

#endif

/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef POST
#define POST

#include "getnode.cpp"

void ttPost(char* mailbox, void* msg) {

  DataNode* dn = getNode(mailbox, rtsys->mailboxList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttPost: Non-existent mailbox '%s'!", mailbox);
    MEX_ERROR(buf);
    return;
  }

  Mailbox* m = (Mailbox*) dn->data;

  UserTask* task = (UserTask*) rtsys->running;

  if (m->count == m->maxSize) {
    //printf("ttPost: Mailbox '%s' is full, blocking\n", mailbox);
    // block the posting task
    task->moveToList(m->writerQ);
    task->state = WAITING;
    // Execute suspend hook
    task->suspend_hook(task);
    // Store the msg pointer in the task struct
    task->mb_data = msg;

  } else {

    m->buffer->appendNode(new DataNode(msg, NULL));
    m->count++;   // number of messages

    // release first waiting reader, if any
    task = (UserTask*) m->readerQ->getFirst();
    if (task != NULL) {

      dn = (DataNode*) m->buffer->getFirst();
      task->mb_data = (void*)dn->data;  // save it for later (ttRetrieve)
      m->buffer->deleteNode(dn);
      m->count--;

      task->moveToList(rtsys->readyQ);
      task->state = READY;

    }
  }
}
#endif

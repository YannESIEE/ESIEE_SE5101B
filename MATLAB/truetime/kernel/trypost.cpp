/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef TRY_POST
#define TRY_POST

#include "getnode.cpp"

bool ttTryPost(char* mailbox, void* msg) {

  DataNode* dn;
  Mailbox* m;

  dn = getNode(mailbox, rtsys->mailboxList);
  if (dn == NULL) {
    char buf[200];
    sprintf(buf, "ttTryPost: Non-existent mailbox '%s'!", mailbox);
    MEX_ERROR(buf);
    return false;
  }

  m = (Mailbox*) dn->data;

  if (m->count == m->maxSize) {
    //printf("ttTryPost: Mailbox '%s' is full\n", mailbox);
    return false;
  } else {
    m->buffer->appendNode(new DataNode(msg, NULL));
    m->count++;
  }

  // release first waiting reader, if any
  UserTask* task = (UserTask*) m->readerQ->getFirst();
  if (task != NULL) {

    dn = (DataNode*) m->buffer->getFirst();
    task->mb_data = (void*)dn->data;  // save it for later (ttRetrieve)
    m->buffer->deleteNode(dn);
    m->count--;

    task->moveToList(rtsys->readyQ);
    task->state = READY;
  }

  return true;
}
#endif

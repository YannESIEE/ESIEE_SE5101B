/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef TRY_FETCH
#define TRY_FETCH

#include "getnode.cpp"

void* ttTryFetch(char* mailbox) {

  DataNode* dn;
  Mailbox* m;
  void* msg;

  dn = getNode(mailbox, rtsys->mailboxList);
  if (dn == NULL) {
    // Mailbox does not exist
    char buf[200];
    sprintf(buf, "ttTryFetch: Non-existent mailbox '%s'\n", mailbox);
    MEX_ERROR(buf);
    return NULL;
  }

  m = (Mailbox*) dn->data;
  if (m->count == 0) {
    //printf("ttTryFetch: Mailbox '%s' is empty\n", mailbox);
    return NULL;
  } else {
    dn = (DataNode*) m->buffer->getFirst();
    msg = dn->data;
    m->buffer->deleteNode(dn);
    m->count--;

    // release first waiting writer, if any
    UserTask* task2 = (UserTask*) m->writerQ->getFirst();
    if (task2 != NULL) {
      m->buffer->appendNode(new DataNode(task2->mb_data, NULL));
      m->count++;   // number of messages
      task2->moveToList(rtsys->readyQ);
      task2->state = READY;
    }
  }
  return msg;
}

#endif

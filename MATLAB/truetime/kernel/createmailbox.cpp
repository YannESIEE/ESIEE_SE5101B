/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef CREATE_MAILBOX
#define CREATE_MAILBOX

#include "getnode.cpp"

void ttCreateMailbox(char *nameOfMailbox, int maxSize) {

  DataNode* dn;
  Mailbox* m;
  
  if (strcmp(nameOfMailbox,"") == 0) {
    MEX_ERROR("ttCreateMailbox: Name should be a non-empty string!");
    return;
  }
  if (rtsys->prioFcn == NULL) {
    MEX_ERROR("ttCreateMailbox: Kernel must be initialized before creation of mailboxes!");
    return;
  } 
  if (maxSize < 1) {
    MEX_ERROR("ttCreateMailbox: Size of mailbox must be greater than zero!");
    return;
  }
  dn = getNode(nameOfMailbox, rtsys->mailboxList);
  if (dn != NULL) { 
    MEX_ERROR("ttCreateMailbox: Name of mailbox not unique!");
    return;
  }

  m = new Mailbox(nameOfMailbox, maxSize);
  m->readerQ = new List("ReaderQ", NULL); // waiting reader tasks in FIFO order
  m->writerQ = new List("WriterQ", NULL); // waiting writer tasks in FIFO order

  rtsys->mailboxList->appendNode(new DataNode(m, m->name));
}


void ttCreateMailbox(char *nameOfMailbox) {

  ttCreateMailbox(nameOfMailbox, INT_MAX);

}

#endif

/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef MAILBOX_H
#define MAILBOX_H

class Mailbox {
 public:
  char* name;
  List *buffer;
  int maxSize;
  int count, counter;
  List *readerQ;
  List *writerQ;

  Mailbox(char *n, int size);
  ~Mailbox();
};

/**
 * Mailbox Constructor 
 */
Mailbox::Mailbox(char *n, int size) {
  if (n==NULL) {
    name = NULL;
  } else {
    name = new char[strlen(n)+1];
    strcpy(name, n);
  }
  maxSize = size;
  buffer = new List("MailboxBuffer", NULL);
  count = 0;
  counter = 0;
  readerQ = NULL;
  writerQ = NULL;
}

/**
 * Mailbox Destructor 
 */
Mailbox::~Mailbox() {
  DataNode *dn, *tmp;

  if (name != NULL) {
    delete[] name;
  }

  dn = (DataNode*) buffer->getFirst();
  while (dn != NULL) {
    tmp = dn;
    dn = (DataNode*) dn->getNext();

#ifdef KERNEL_MATLAB
    mxDestroyArray((mxArray *)tmp->data);
#endif

    delete tmp;
  }
  delete buffer;

  if (readerQ) delete readerQ;
  if (writerQ) delete writerQ;

}

#endif

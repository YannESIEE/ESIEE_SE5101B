/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef GET_NODE
#define GET_NODE

DataNode *getNode(char *name, List* list) {

  DataNode* n;

  if (strcmp(name, "")==0) {
    return NULL;
  }

  n = (DataNode*) list->getFirst();
  while (n != NULL) {
    if (n->name != NULL) {
      if (strcmp(n->name, name) == 0)
	break;
    }
    n = (DataNode*) n->getNext();
  }
  if (n == NULL) {
    return NULL;
  } else {
    return n;
  }
}

#endif




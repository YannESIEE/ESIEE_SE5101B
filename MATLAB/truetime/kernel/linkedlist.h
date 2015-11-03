/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

class Node;

/**
 * List class
 */
class List {
  char *name;
  Node *pHead, *pTail;
  int (*compFcn)(Node *, Node *);
public:
  Node *getFirst();
  Node *getLast();
  void appendNode(Node *pNode);
  void insertAfter(Node *pNode, Node *pAfter);
  void insertBefore(Node *pNode, Node *pBefore);
  void insertSorted(Node *pNode);
  void removeNode(Node *pNode);
  void deleteNode(Node *pNode);
  int length();
  void print();

  List(char *n, int (*compFcn)(Node *, Node *));
  ~List();
};

/**
 * Node class
 */
class Node {
public:
  Node* pNext;
  Node* pPrev;
  List* myList;
  
  Node *getNext();
  Node *getPrev();
  void remove();
  void moveToList(List* newlist);
  
  virtual void print() = 0;
  Node(); // constructor
  virtual ~Node(){}
};

#endif

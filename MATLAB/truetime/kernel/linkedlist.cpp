/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#ifndef LINKEDLIST
#define LINKEDLIST

#include "linkedlist.h"

/** 
 * Node Constructor 
 */
Node::Node() {
  pNext = NULL;
  pPrev = NULL;
  myList = NULL;
}

/** 
 * Returns a pointer to the node after pNode 
 * NULL if pNode is last node 
 */
Node *Node::getNext() {
  
  return pNext;
}

/** 
 * Returns a pointer to the node before pNode,
 * NULL if pNode is first node 
 */
Node *Node::getPrev() {
  
  return pPrev;
}

/** 
 * Removes the node from its current list
 */
void Node::remove() {
  if (myList != NULL) {
    myList->removeNode(this);
  }
}

/** 
 * Removes the node from its current list
 * and inserts it sorted in newList
 */
void Node::moveToList(List* newList) {

  remove();
  newList->insertSorted(this);
}


/**
 * List Constructor 
 */
List::List(char *n, int (*cf)(Node *, Node *)) {
  name = new char[strlen(n)+1];
  strcpy(name, n);
  compFcn = cf;
  pHead = NULL;
  pTail = NULL;
}

List::~List() {
  delete[] name;
}

/* Returns a pointer to the first node in the list */
Node *List::getFirst() {
  return pHead;
}

/* Returns a pointer to the last node in the list */
Node *List::getLast() {
  return pTail;
}

/* Appends a node to the end of the list */
void List::appendNode(Node *pNode)
{
  if (pHead == NULL) {
    pHead = pNode;
    pNode->pPrev = NULL;
  }
  else {
    pTail->pNext = pNode;
    pNode->pPrev = pTail;
  }
  pTail = pNode;
  pNode->pNext = NULL;

  pNode->myList = this;
}

/* Inserts a node into the list after pAfter */
void List::insertAfter(Node *pNode, Node *pAfter)
{
  pNode->pNext = pAfter->pNext;
  pNode->pPrev = pAfter;
  if (pAfter->pNext != NULL)
    pAfter->pNext->pPrev = pNode;
  else
    pTail = pNode;
  pAfter->pNext = pNode;

  pNode->myList = this;
}

/* Inserts a node into the list before pBefore */
void List::insertBefore(Node *pNode, Node *pBefore)
{
  pNode->pPrev = pBefore->pPrev;
  pNode->pNext = pBefore;
  if (pBefore->pPrev != NULL)
    pBefore->pPrev->pNext = pNode;
  else
    pHead = pNode;
  pBefore->pPrev = pNode;

  pNode->myList = this;
}

/** 
 * Inserts a node in the list, sorted using compFcn
 * returns true if the node is inserted last in the list
 */
void List::insertSorted(Node *pNode)
{
  Node *p;

  if (compFcn == NULL) {
    appendNode(pNode);
    return;
  }

  p = pHead;
  if (p==NULL) {
    appendNode(pNode);
    return;
  }
  else {
    while (p!=NULL) {
      if (compFcn(pNode,p) >= 0) {
	insertBefore(pNode,p);
	return;
      } else {
	p = p->pNext;
      }
    }
    if (p==NULL) {
      appendNode(pNode);
    }
    return;
  }
}

/* Removes the specified node from the list */
void List::removeNode(Node *pNode)
{
  if (pNode->pPrev == NULL)
    pHead = pNode->pNext;
  else
    pNode->pPrev->pNext = pNode->pNext;

  if (pNode->pNext == NULL)
    pTail = pNode->pPrev;
  else
    pNode->pNext->pPrev = pNode->pPrev;

  pNode->myList = NULL;
}

void List::deleteNode(Node *pNode)
{
  removeNode(pNode);
  delete pNode;
}

int List::length() {
  int len = 0;
  Node *n = pHead;
  while (n != NULL) {
    len++;
    n = n->pNext;
  }
  return len;
}

void List::print() {
  int i = 0;

  printf("%s:", name);
  Node *n = pHead;
  if (n == NULL) printf(" Empty");
  while (n != NULL) {
    printf(" %d.",++i);
    n->print();
    n = n->pNext;
  }
  printf("\n");
}

#endif

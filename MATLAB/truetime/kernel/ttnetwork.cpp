/*
 * TrueTime, Version 1.5
 * Copyright (c) 2007
 * Martin Ohlin, Dan Henriksson and Anton Cervin
 * Department of Automatic Control LTH
 * Lund University, Sweden
 */

#include "ttnetwork.h"

#define EPS 0.0000001  // 0.1 us
#define INF 1000000.0

// ----- Main data structure ------

RTnetwork *nwsys;  

// ------- Network functions -------

// returns a uniformly distributed random number between a and b (inclusive)
// (assumes b >= a and a >= 0)
int urand(int a, int b) {
  return a + rand() % (b-a+1);
}

// computes the remaining transmission time of the current frame
double remxtime(NWmsg *m) {
  switch (nwsys->type) {
  case FDMA:
    return m->remaining / (nwsys->bandwidths[m->sender] * nwsys->datarate + EPS);
  default:
    return m->remaining / nwsys->datarate;
  }
}

// allocates switch memory, if possible
bool switchmalloc(int sender, int receiver, int length) {
  int i;
  if (nwsys->buftype == OUTPUTBUF) {
    if (receiver == -1) { // broadcast
      // make sure there is memory in each buffer first
      bool ok = true;
      for (i=0; i<nwsys->nbrOfNodes; i++) {
	if (sender != i) {
	  if (nwsys->nwnodes[i]->switchmem < length) {
	    ok = false;
	    break;
	  }
	}
      }
      if (!ok) return false;
      // allocate memory
      for (i=0; i<nwsys->nbrOfNodes; i++) {
	if (sender != i) {
	  nwsys->nwnodes[i]->switchmem -= length;
	  //printf("Broadcast: %d bits allocated in output buffer %d, remaining %d\n", length, i, nwsys->nwnodes[i]->switchmem);
	}
      }
      return true;
    } else {
      if (nwsys->nwnodes[receiver]->switchmem < length) return false;
      nwsys->nwnodes[receiver]->switchmem -= length;
      //printf("%d bits allocated in output buffer %d, remaining %d\n", length, receiver, nwsys->nwnodes[receiver]->switchmem);
      return true;
    }
  } else {
    if (receiver == -1) { // broadcast
      if (nwsys->switchmem < (nwsys->nbrOfNodes-1)*length) return false;
      nwsys->switchmem -= (nwsys->nbrOfNodes-1)*length;
      //printf("Broadcast: %d bits allocated in common buffer, remaining %d\n", (nwsys->nbrOfNodes-1)*length, nwsys->switchmem);
      return true;
    } else {
      if (nwsys->switchmem < length) return false;
      nwsys->switchmem -= length;
      //printf("%d bits allocated in common buffer, remaining %d\n", length, nwsys->switchmem);      
      return true;
    }
  }
}

// frees switch memory
void switchfree(int receiver, int length) {
  if (nwsys->buftype == OUTPUTBUF) {
    nwsys->nwnodes[receiver]->switchmem += length;
    //printf("%d freed in output buffer %d, remaining %d\n", length, receiver, nwsys->nwnodes[receiver]->switchmem);    
  } else {
    nwsys->switchmem += length;
    //printf("%d bits freed in common buffer, remaining %d\n", length, nwsys->switchmem);      
   }
}


/**
 * Returns the smallest postdelay != 0
 */
double broadcast(NWmsg *m, int whichqueue) {
  int j;
  NWmsg *m2;
  mxArray* data = NULL;
  char* node = new char[3];	
  double temp = INF;

  if (m->dataMATLAB != NULL) {
    data = m->dataMATLAB;
  }

  for (j=0; j<nwsys->nbrOfNodes; j++) {
    if (j != m->sender) {
      // Duplicate message
      m2 = new NWmsg();
      *m2 = *m;
      if (data) {
	m2->dataMATLAB = mxDuplicateArray(data);
	mexMakeArrayPersistent(m2->dataMATLAB);
      }
      if (whichqueue == 1) {
	nwsys->nwnodes[j]->outputQ->appendNode(m2);
      } else if (whichqueue == 2) {
	nwsys->nwnodes[j]->switchQ->appendNode(m2);
      }
      
      m2->waituntil = nwsys->time + nwsys->nwnodes[j]->postdelay;
      // How far from now should we run _next_ time?
      if (nwsys->nwnodes[j]->postdelay != 0.0 && 
	  temp > nwsys->nwnodes[j]->postdelay ){
	temp = nwsys->nwnodes[j]->postdelay;
      }
    }
  }
#ifndef KERNEL_C
  mxDestroyArray(m->dataMATLAB);
#endif
  
  delete m;
  delete[] node;

  return temp;
}  

// Returns the time of the next invocation (nextHit)
double runNetwork() {
  int i;
  NWmsg *m, *m2;  // m is our message, m2 is contending message
  NWmsg *next;
  double timeElapsed;
  double nextHit = INF;
  double waittime;
  double waituntil;

  timeElapsed = nwsys->time - nwsys->prevHit; // time since last invocation
  nwsys->prevHit = nwsys->time;

  //printf("Running network at %f\n", nwsys->time);

  // Check if messages have finished waiting in the preprocQ's

  for (i=0; i<nwsys->nbrOfNodes; i++) {
    m = (NWmsg *)nwsys->nwnodes[i]->preprocQ->getFirst();
    while (m != NULL) {
      if (m->waituntil - nwsys->time < EPS) {
	//printf("moving message from preprocQ to inputQ at %f\n", nwsys->time);
	m->remaining = max(m->length, nwsys->minsize);
	nwsys->nwnodes[i]->preprocQ->removeNode(m);
	nwsys->nwnodes[i]->inputQ->appendNode(m);
      } else {
	// update nextHit?
	if (m->waituntil < nextHit) {
	  nextHit = m->waituntil;
	}
      }
      m = (NWmsg *)m->getNext();
    }
  }
  
  // Do the main processing of different protocols
  switch (nwsys->type) {

  case CSMACD:
    if (nwsys->sending != -1) { // not been idle
      //printf("Node %d has been sending\n", nwsys->sending);
	m = (NWmsg *)nwsys->nwnodes[nwsys->sending]->inputQ->getFirst();
	// decrease remaining number of bits in current frame
	m->remaining -= (nwsys->datarate * timeElapsed);
	// frame finished?
	if (m->remaining < EPS) {
	  nwsys->nwnodes[nwsys->sending]->nbrcollisions = 0; // reset coll. counter
	  // transmission is finished, move to outputQ
	  //printf("Transmission finished\n");
	  nwsys->nwnodes[nwsys->sending]->inputQ->removeNode(m);
	  if (m->receiver == -1) {
	    // Broadcast
	    // Duplicate message and append to outputQ's of all nodes
	    waituntil = nwsys->time + broadcast(m, 1);
	  } else {
	    nwsys->nwnodes[m->receiver]->outputQ->appendNode(m);
	    m->waituntil = nwsys->time + nwsys->nwnodes[m->receiver]->postdelay;
	    waituntil = m->waituntil;
	  }
	  // update nextHit?
	  if ( waituntil != nwsys->time && waituntil < nextHit ) {
	    nextHit = waituntil;
	  }
	
	  nwsys->nwnodes[nwsys->sending]->state = 0; // idle
	  nwsys->sending = -1;

	} else {
	  if (nwsys->time + remxtime(m) < nextHit) {
	    nextHit = nwsys->time + remxtime(m);
	  }	  
	}
    }
    
    // check if some node has finished waiting after a collision
    for (i=0; i<nwsys->nbrOfNodes; i++) {
      if (nwsys->nwnodes[i]->state == 3) {
	if (nwsys->time >= nwsys->nwnodes[i]->waituntil) {
	  //printf("Node %d has finished waiting\n", i);
	  nwsys->nwnodes[i]->state = 0; // idle again
	} else if (nwsys->nwnodes[i]->waituntil < nextHit) {
	  nextHit = nwsys->nwnodes[i]->waituntil;
	}
      }
    }

    // if network appears idle, check if any new nodes want to transmit
    if (nwsys->sending == -1 || nwsys->time < nwsys->lasttime + COLLISION_WINDOW) {
      //printf("The network *appears* to be idle\n");
      // do any nodes want to transmit?
      for (i=0; i<nwsys->nbrOfNodes; i++) {
	// not sending already and anything to send?
	if (nwsys->nwnodes[i]->state == 0 && (m = (NWmsg *)nwsys->nwnodes[i]->inputQ->getFirst()) != NULL) {
	  //printf("Node %d wants to transmit...\n", i);
	  if (nwsys->sending == -1) { // really idle?
	    //printf("Medium is idle at time %f, node %d starts to transmit...\n", nwsys->time, i);
	    //printf("Remaining: %f\n", m->remaining);
	    nwsys->lasttime = nwsys->time;
	    nwsys->nwnodes[i]->state = 1; // sending
	    nwsys->sending = i;
	    // update nextHit?
	    if (nwsys->time + remxtime(m) < nextHit) {
	      nextHit = nwsys->time + remxtime(m);
	    }
	  } else { // collision!
	    //printf("Collision when node %d tried to send at time %f!\n", i, nwsys->time);
	    // mark both nodes as colliding
	    nwsys->nwnodes[i]->state = 2; // colliding
	    nwsys->nwnodes[nwsys->sending]->state = 2; // colliding
	    // abort the currently transmitted frame
	    m = (NWmsg *)nwsys->nwnodes[nwsys->sending]->inputQ->getFirst();
	    m->remaining = max(m->length, nwsys->minsize);  // restore remaining
	  }
	}
      }
    }

    // move nodes from colliding to waiting state
    for  (i=0; i<nwsys->nbrOfNodes; i++) {
      if (nwsys->nwnodes[i]->state == 2) {
	nwsys->sending = -1;
	nwsys->nwnodes[i]->state = 3;
	nwsys->nwnodes[i]->nbrcollisions += 1;
	//printf("node %d has now collided %d times in a row\n", i, nwsys->nwnodes[i]->nbrcollisions);
	if (nwsys->nwnodes[i]->nbrcollisions > 10) {
	  nwsys->nwnodes[i]->nbrcollisions = 10;
	  // printf("max number of collisions reached!\n");
	}
	// compute random back-off time
	waittime = urand(0, (1 << nwsys->nwnodes[i]->nbrcollisions) - 1) * nwsys->minsize / nwsys->datarate;
	nwsys->nwnodes[i]->waituntil = nwsys->time + max(waittime, 2.0*COLLISION_WINDOW);
	//printf("Will reattempt at %f\n", nwsys->nwnodes[i]->waituntil);
	if (nwsys->nwnodes[i]->waituntil < nextHit) {
	  nextHit = nwsys->nwnodes[i]->waituntil;
	}
      }
    }
    break;

  case CSMAAMP:
    if (nwsys->sending != -1) { // not been idle
      // printf("Node %d has been sending\n", nwsys->sending);
	m = (NWmsg *)nwsys->nwnodes[nwsys->sending]->inputQ->getFirst();
	// decrease remaining number of bits in current frame
	m->remaining -= (nwsys->datarate * timeElapsed);
	// frame finished?
	if (m->remaining < EPS) {
	  // transmission is finished, move to outputQ
	  // printf("Transmission finished\n");
	  nwsys->nwnodes[nwsys->sending]->inputQ->removeNode(m);

	  if (m->receiver == -1) {
	    // Broadcast
	    // Duplicate message and append to outputQ's of all nodes
	    waituntil = nwsys->time + broadcast(m, 1);
	  } else {
	    nwsys->nwnodes[m->receiver]->outputQ->appendNode(m);
	    m->waituntil = nwsys->time + nwsys->nwnodes[m->receiver]->postdelay;
	    waituntil = m->waituntil;
	  }
	  // update nextHit?
	  if ( waituntil != nwsys->time && waituntil < nextHit ) {
	    nextHit = waituntil;
	  }
	  
	  nwsys->nwnodes[nwsys->sending]->state = 0; // idle
	  nwsys->sending = -1;

	} else {
	  if (nwsys->time + remxtime(m) < nextHit) {
	    nextHit = nwsys->time + remxtime(m);
	  }	  
	}
    }
    
    if (nwsys->sending == -1 || nwsys->time < nwsys->lasttime + COLLISION_WINDOW) {
      // printf("The network *appears* to be idle\n");
      // do any nodes want to transmit?
      for (i=0; i<nwsys->nbrOfNodes; i++) {
	// not sending already and anything to send?
	if (nwsys->nwnodes[i]->state == 0 && (m = (NWmsg *)nwsys->nwnodes[i]->inputQ->getFirst()) != NULL) {
	  // printf("Node %d wants to transmit...\n", i);
	  if (nwsys->sending == -1) { // still idle?
	    // printf("Medium is idle, node %d starts to transmit...\n", i);
	    nwsys->lasttime = nwsys->time;
	    nwsys->nwnodes[i]->state = 1; // sending
	    nwsys->sending = i;
	    // update nextHit?
	    if (nwsys->time + remxtime(m) < nextHit) {
	      nextHit = nwsys->time + remxtime(m);
	    }
	  } else { // collision!
	    // printf("Medium was not idle - collision!\n");
	    m2 = (NWmsg *)nwsys->nwnodes[nwsys->sending]->inputQ->getFirst();
	    // printf("Sending message has priority %f, our message has priority %f\n", m2->prio, m->prio);
	    if (m->prio > m2->prio) { // we have lower prio?
	      // printf("We lose the contention, will try again later\n");
	      if (nwsys->time + remxtime(m2) < nextHit) {
		nextHit = nwsys->time + remxtime(m2);
	      }
	    } else { // we have higher prio
	      // printf("We win the contention, other message aborted!\n");
	      m2->remaining = max(m2->length, nwsys->minsize); // restore remaining
	      nwsys->nwnodes[nwsys->sending]->state = 0;
	      nwsys->nwnodes[i]->state = 1;
	      nwsys->sending = i;
	      if (nwsys->time + remxtime(m) < nextHit) {
		nextHit = nwsys->time + remxtime(m);
	      }
	    }
	  }
	}
      }
    }
    break;

  case RR:
    if (nwsys->sending == -1) {
      // printf("The network has been idle\n");
      // ready for the next transmission?
      if (nwsys->time >= nwsys->waituntil) {
	nwsys->rrturn = (nwsys->rrturn + 1) % nwsys->nbrOfNodes;
	// printf("Token passed to node %d at time %f,\n", nwsys->rrturn, nwsys->time);
	if ((m = (NWmsg *)nwsys->nwnodes[nwsys->rrturn]->inputQ->getFirst()) != NULL) {
	  // printf("  node %d starts to transmit\n", nwsys->rrturn);
	  nwsys->sending = nwsys->rrturn;
	  nwsys->nwnodes[nwsys->rrturn]->state = 1; // we're sending
	  // update nextHit?
	  if (nwsys->time + remxtime(m) < nextHit) {
	    nextHit = nwsys->time + remxtime(m);
	  }
	} else {
	  // printf("  node %d has nothing to send, will idle...\n", nwsys->rrturn);
	  nwsys->waituntil = nwsys->time + nwsys->minsize / nwsys->datarate;
	  // update nextHit?
	  if (nwsys->waituntil > nwsys->time && nwsys->waituntil < nextHit) {
	    nextHit = nwsys->waituntil;
	  }
	}
      } else {// wait some more
	// update nextHit?
	if (nwsys->waituntil < nextHit) {
	  nextHit = nwsys->waituntil;
	}
      }
    } else {
      // printf("Node %d has been sending\n", nwsys->sending);
      m = (NWmsg *)nwsys->nwnodes[nwsys->sending]->inputQ->getFirst();
      // count down remaining transmission length
      m->remaining -= (nwsys->datarate * timeElapsed);
      // frame finished?
      if (m->remaining < EPS) { // yes
	// finished transmission, move to outputQ
	// printf("Transmission finished\n");
	nwsys->nwnodes[nwsys->sending]->inputQ->removeNode(m);
	
	if (m->receiver == -1) {
	  // Broadcast
	  // Duplicate message and append to outputQ's of all nodes
	  waituntil = nwsys->time + broadcast(m, 1);
	} else {
	  nwsys->nwnodes[m->receiver]->outputQ->appendNode(m);
	  m->waituntil = nwsys->time + nwsys->nwnodes[m->receiver]->postdelay;
	  waituntil = m->waituntil;
	}
	// update nextHit?
	if ( waituntil != nwsys->time && waituntil < nextHit ) {
	  nextHit = waituntil;
	}

	nwsys->nwnodes[nwsys->sending]->state = 0;
	nwsys->sending = -1;  // idle and hand over to next node
	nwsys->waituntil = nwsys->time + nwsys->minsize / nwsys->datarate;
	if (nwsys->waituntil < nextHit) {
	  nextHit = nwsys->waituntil;
	}
      } else { // more to transmit in this frame
	if (nwsys->time + remxtime(m) < nextHit) {
	  nextHit = nwsys->time + remxtime(m);
	} 
      }
    }

    break;

  case FDMA:
    // go through all nodes and count down transmission times
    for (i=0; i<nwsys->nbrOfNodes; i++) {
      if (nwsys->nwnodes[i]->state == 1) { // node has been sending
	m = (NWmsg *)nwsys->nwnodes[i]->inputQ->getFirst();
	// decrease remaining number of bits in current frame
	m->remaining -= (nwsys->bandwidths[i] * nwsys->datarate * timeElapsed);
	// frame finished?
	if (m->remaining < EPS) {
	  // transmission is finished, move to outputQ
	  // printf("transmission finished\n");
	  nwsys->nwnodes[i]->inputQ->removeNode(m);
	  
	  if (m->receiver == -1) {
	    // Broadcast
	    // Duplicate message and append to outputQ's of all nodes
	    waituntil = nwsys->time + broadcast(m, 1);
	  } else {
	    nwsys->nwnodes[m->receiver]->outputQ->appendNode(m);
	    m->waituntil = nwsys->time + nwsys->nwnodes[m->receiver]->postdelay;
	    waituntil = m->waituntil;
	  }
	  // update nextHit?
	  if ( waituntil != nwsys->time && waituntil < nextHit ) {
	    nextHit = waituntil;
	  }

	  nwsys->nwnodes[i]->state = 0;

	} else { // frame not finished
	  // update nextHit?
	  if (nwsys->time + remxtime(m) < nextHit) {
	    nextHit = nwsys->time + remxtime(m);
	  } 
	}
      }
    }
    
    // check if any new transmissions should be started
    for (i=0; i<nwsys->nbrOfNodes; i++) {
      if (nwsys->nwnodes[i]->state == 0) { // idle?
	// check if we should start a new transmission
	if ((m = (NWmsg *)nwsys->nwnodes[i]->inputQ->getFirst()) != NULL) {
	  //printf("Node %d starting transmission of frame at %f\n", i, nwsys->time);
	  nwsys->nwnodes[i]->state = 1; // we're sending
	  // update nextHit?
	  if (nwsys->time + remxtime(m) < nextHit) {
	    nextHit = nwsys->time + remxtime(m);
	  } 
	}
      }
    }
    break;

  case TDMA:
    if (nwsys->sending != -1) { // someone has been sending
      m = (NWmsg *)nwsys->nwnodes[nwsys->sending]->inputQ->getFirst();
      // decrease remaining number of bits in current frame
      m->remaining -= (nwsys->datarate * timeElapsed);
      if (m->remaining < EPS) {
	// transmission is finished, move to outputQ
	// printf("transmission finished\n");
	nwsys->nwnodes[nwsys->sending]->inputQ->removeNode(m);
	
	if (m->receiver == -1) {
	  // Broadcast
	  // Duplicate message and append to outputQ's of all nodes
	  waituntil = nwsys->time + broadcast(m, 1);
	} else {
	  nwsys->nwnodes[m->receiver]->outputQ->appendNode(m);
	  m->waituntil = nwsys->time + nwsys->nwnodes[m->receiver]->postdelay;
	  waituntil = m->waituntil;
	}
	// update nextHit?
	if ( waituntil != nwsys->time && waituntil < nextHit ) {
	  nextHit = waituntil;
	}

	// will we have time and are there more messages?
	if (nwsys->time < nwsys->currslottime + nwsys->slottime && (m = (NWmsg *)nwsys->nwnodes[nwsys->sending]->inputQ->getFirst()) != NULL) {
	  // printf("Node %d starting transmission of new message at %f\n", nwsys->sending, nwsys->time);
	  // update nextHit?
	  if (nwsys->time + remxtime(m) < nextHit) {
	    nextHit = nwsys->time + remxtime(m);
	  }
	} else {
	  nwsys->nwnodes[nwsys->sending]->state = 0; // idle
	  nwsys->sending = -1;
	}
      } else { // frame not finished
	// update nextHit?
	if (nwsys->time + remxtime(m) < nextHit) {
	  nextHit = nwsys->time + remxtime(m);
	} 
      }
    }

    // time for new slot?
    if (nwsys->time >= nwsys->currslottime + nwsys->slottime) {
      nwsys->slotcount = (nwsys->slotcount + 1) % nwsys->schedsize;
      // printf("New slot: index %d, sender %d\n", nwsys->slotcount, nwsys->schedule[nwsys->slotcount]);
      nwsys->currslottime += nwsys->slottime;

      // preempt current transmission?
      if (nwsys->sending != -1 && nwsys->schedule[nwsys->slotcount] != nwsys->sending) {
	// printf("current transmission by %d interrupted!\n", nwsys->sending);
	nwsys->nwnodes[nwsys->sending]->state = 2; // wait state
	nwsys->sending = -1;
      }
    }

      // start/resume a transmission?
    if (nwsys->schedule[nwsys->slotcount] != nwsys->sending) {
      i = nwsys->schedule[nwsys->slotcount];
      // resuming an old transmission?
      if (nwsys->nwnodes[i]->state == 2) {
	// printf("Node %d resuming its transmission\n", i);
	nwsys->nwnodes[i]->state = 1;
	nwsys->sending = i;
	m = (NWmsg *)nwsys->nwnodes[i]->inputQ->getFirst();
	if (nwsys->time + remxtime(m) < nextHit) {
	  nextHit = nwsys->time + remxtime(m);
	} 
      } else { // new transmission
	// printf("node %d may start new transmission\n", i);
	if ((m = (NWmsg *)nwsys->nwnodes[i]->inputQ->getFirst()) != NULL) {
	  // printf("Node %d starting transmission of frame at %f\n", i, nwsys->time);
	  nwsys->nwnodes[i]->state = 1; // we're sending
	  nwsys->sending = i;
	  // update nextHit?
	  if (nwsys->time + remxtime(m) < nextHit) {
	    nextHit = nwsys->time + remxtime(m);
	  } 
	} else {
	  // printf("Node %d has nothing to send...\n");
	}
      }
    }

    if (nwsys->currslottime + nwsys->slottime < nextHit) {
      nextHit = nwsys->currslottime + nwsys->slottime;
    }

    break;

  case SFDSE: // Symmetric Full Duplex Switched Ethernet
    
    // FROM SENDER TO SWITCH

    // go through all nodes and count down transmission times
    for (i=0; i<nwsys->nbrOfNodes; i++) {
      if (nwsys->nwnodes[i]->state == 1) { // node has been sending
	m = (NWmsg *)nwsys->nwnodes[i]->inputQ->getFirst();
	// decrease remaining number of bits in current frame
	m->remaining -= (nwsys->datarate * timeElapsed);
	// frame finished?
	if (m->remaining < EPS) {
	  // transmission is finished, move to switchQ
	  //printf("transmission finished, move to switchQ\n");
	  m->remaining = max(m->length, nwsys->minsize); // restore remaining
	  if (switchmalloc(i,m->receiver,m->length)) {
	    nwsys->nwnodes[i]->inputQ->removeNode(m);
	    if (m->receiver == -1) {
	      // Broadcast
	      // Duplicate message and append to switchQ's of all nodes
	      waituntil = nwsys->time + broadcast(m, 2);
	    } else {
	      nwsys->nwnodes[m->receiver]->switchQ->appendNode(m);
	    }
	  } else {
	    if (nwsys->overflow == BUFFULLRETRY) {
	      printf("Switch buffer full, retransmitting!\n");
	    } else {
	      printf("Switch buffer full, dropping!\n");
#ifndef KERNEL_C
	      mxDestroyArray(m->dataMATLAB);
#endif
	      nwsys->nwnodes[i]->inputQ->deleteNode(m);
	    }
	  }
	  
	  nwsys->nwnodes[i]->state = 0;

	} else { // frame not finished
	  // update nextHit?
	  if (nwsys->time + remxtime(m) < nextHit) {
	    nextHit = nwsys->time + remxtime(m);
	  } 
	}
      }
    }
    
    // check if any new transmissions should be started
    for (i=0; i<nwsys->nbrOfNodes; i++) {
      if (nwsys->nwnodes[i]->state == 0) { // idle?
	// check if we should start a new transmission
	if ((m = (NWmsg *)nwsys->nwnodes[i]->inputQ->getFirst()) != NULL) {
	  //printf("Node %d starting transmission of frame at %f\n", i, nwsys->time);
	  nwsys->nwnodes[i]->state = 1; // we're sending
	  // update nextHit?
	  if (nwsys->time + remxtime(m) < nextHit) {
	    nextHit = nwsys->time + remxtime(m);
	  } 
	}
      }
    }

    // FROM SWITCH TO RECEIVER

    // go through all nodes and count down transmission times
    for (i=0; i<nwsys->nbrOfNodes; i++) {
      if (nwsys->nwnodes[i]->swstate == 1) { // node has been sending
	m = (NWmsg *)nwsys->nwnodes[i]->switchQ->getFirst();
	// decrease remaining number of bits in current frame
	m->remaining -= (nwsys->datarate * timeElapsed);
	// frame finished?
	if (m->remaining < EPS) {
	  // transmission is finished, move to outputQ
	  //printf("transmission finished, move to outputQ\n");
	  m->remaining = max(m->length, nwsys->minsize); // restore remaining
	  nwsys->nwnodes[i]->switchQ->removeNode(m);
	  nwsys->nwnodes[i]->outputQ->appendNode(m);
	  nwsys->nwnodes[i]->swstate = 0;
	  switchfree(i, m->length);
	} else { // frame not finished
	  // update nextHit?
	  if (nwsys->time + remxtime(m) < nextHit) {
	    nextHit = nwsys->time + remxtime(m);
	  } 
	}
      }
    }
    
    // check if any new transmissions should be started
    for (i=0; i<nwsys->nbrOfNodes; i++) {
      if (nwsys->nwnodes[i]->swstate == 0) { // idle?
	// check if we should start a new transmission
	if ((m = (NWmsg *)nwsys->nwnodes[i]->switchQ->getFirst()) != NULL) {
	  //printf("Switch output %d starting transmission of frame at %f\n", i, nwsys->time);
	  nwsys->nwnodes[i]->swstate = 1; // we're sending
	  // update nextHit?
	  if (nwsys->time + remxtime(m) < nextHit) {
	    nextHit = nwsys->time + remxtime(m);
	  } 
	}
      }
    }    

    break;

  default:
    printf("Protocol not implemented!\n");
    break;
  }

  // Check if messages have finished waiting in the outputQ's

  for (i=0; i<nwsys->nbrOfNodes; i++) {
    m = (NWmsg *)nwsys->nwnodes[i]->outputQ->getFirst();
    while (m != NULL) {
      next = (NWmsg *)m->getNext();
      if (m->waituntil - nwsys->time < EPS) {
	// finished waiting, move to postprocQ
	//printf("*** moving message from outputQ %d to postprocQ at %f\n", i+1, nwsys->time);
	nwsys->nwnodes[i]->outputQ->removeNode(m);
	double rNbr = ((double) rand() / (double) RAND_MAX);
	if (rNbr < nwsys->lossprob) {
	  // packet lost, do not forward to post-proc
	  //printf("Network: A packet headed for node # %d, was lost at time %f\n", i + 1 , nwsys->time);
#ifndef KERNEL_C
	  mxDestroyArray(m->dataMATLAB);
#endif
	  delete m;
	} else {
	  nwsys->nwnodes[i]->postprocQ->appendNode(m);
	  if (nwsys->outputs[i] == 0.0) {
	    nwsys->outputs[i] = 1.0; // trigger rcv output
	  } else {
	    nwsys->outputs[i] = 0.0; // trigger rcv output
	  }
	}
      } else {
	// update nextHit?
	if (m->waituntil < nextHit) {
	  nextHit = m->waituntil;
	}
      }
      m = next; // get next
    }
  }

  // done

  //printf("Next hit scheduled for %f\n", nextHit);

  // produce output graph

  for (i=0; i<nwsys->nbrOfNodes; i++) {
    if (nwsys->nwnodes[i]->state == 1) {
      nwsys->sendschedule[i] = i+1.5;  // sending
    } else if ((m = (NWmsg *)nwsys->nwnodes[i]->inputQ->getFirst()) != NULL) {
      nwsys->sendschedule[i] = i+1.25; // waiting
    } else {
      nwsys->sendschedule[i] = i+1.0;     // idle
    }
  }

  return nextHit;
}

// ------- Simulink callback functions ------- 

#ifdef __cplusplus
extern "C" { // use the C fcn-call standard for all functions  
#endif       // defined within this scope   

#define S_FUNCTION_NAME ttnetwork
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

static void mdlInitializeSizes(SimStruct *S)
{
  int nbrOfNodes = 0;
  const mxArray *arg;

  ssSetNumSFcnParams(S, 12);  /* Number of expected parameters */
  if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
    return; /* Parameter mismatch will be reported by Simulink */
  }
  
  // Parse second argument only to determine nbrOfNodes

  // 2 - Number of nodes
  arg = ssGetSFcnParam(S, 2);
  if (mxIsDoubleScalar(arg)) {
     nbrOfNodes = (int) *mxGetPr(arg);
  }
  if (nbrOfNodes < 1) {
    ssSetErrorStatus(S, "TrueTime Network: The number of nodes must be an integer > 0");
    return;
  }

  ssSetNumContStates(S, 0);
  ssSetNumDiscStates(S, 0);
  
  ssSetNumInputPorts(S, 1);
  ssSetInputPortDirectFeedThrough(S, 0, 0);
  ssSetInputPortWidth(S, 0, nbrOfNodes);
  
  ssSetNumOutputPorts(S, 2);
  ssSetOutputPortWidth(S, 0, nbrOfNodes);
  ssSetOutputPortWidth(S, 1, nbrOfNodes);

  ssSetNumSampleTimes(S, 1);
  
  ssSetNumRWork(S, 0);
  ssSetNumIWork(S, 0);
  ssSetNumPWork(S, 0); 
  ssSetNumModes(S, 0);
  ssSetNumNonsampledZCs(S, 1);

  // Make sure cleanup is performed even if errors occur
  ssSetOptions(S, SS_OPTION_RUNTIME_EXCEPTION_FREE_CODE | 
	          SS_OPTION_CALL_TERMINATE_ON_EXIT);


}


static void mdlInitializeSampleTimes(SimStruct *S)
{
  ssSetSampleTime(S, 0, CONTINUOUS_SAMPLE_TIME);
  ssSetOffsetTime(S, 0, FIXED_IN_MINOR_STEP_OFFSET);
}


#define MDL_START
static void mdlStart(SimStruct *S)
{
  // allocate data

  const mxArray *arg;

  int i;

  static mxArray *lhs[1];
  static mxArray *rhs[2];
  char nwsysp[100];

  // Create new network struct

  //srand(314);
  nwsys = new RTnetwork;
  ssSetUserData(S, nwsys); // save pointer in UserData

  // 1 - Network type
  arg = ssGetSFcnParam(S, 0);
  if (mxIsDoubleScalar(arg)) {
    nwsys->type = (int)(*mxGetPr(arg))-1;
  }
  if (nwsys->type < 0) {
    ssSetErrorStatus(S, "Error in type argument");
    return;
  }
  // printf("type: %d\n", nwsys->type);

  // 2 - Network Number
  arg = ssGetSFcnParam(S, 1);
  if (mxIsDoubleScalar(arg)) {
     nwsys->networkNbr = (int) *mxGetPr(arg);
  }

  // 3 - Number of nodes
  arg = ssGetSFcnParam(S, 2);
  nwsys->nbrOfNodes = (int) *mxGetPr(arg); // we know it's right
  // printf("nbrOfNodes: %d\n", nwsys->nbrOfNodes);

  // 4 - Data rate (bits/s)
  arg = ssGetSFcnParam(S, 3);
  if (mxIsDoubleScalar(arg)) {
     nwsys->datarate = *mxGetPr(arg);
  }
  if (nwsys->datarate < 0.0) {
    ssSetErrorStatus(S, "The data rate must be > 0");
    return;
  }
  // printf("datarate: %f\n", nwsys->datarate);

  // 5 - Minimum frame size
  arg = ssGetSFcnParam(S, 4);
  if (mxIsDoubleScalar(arg)) {
    nwsys->minsize = (int) *mxGetPr(arg);
  }
  if (nwsys->minsize < 0) {
    ssSetErrorStatus(S, "The minimum frame size must be >= 0");
    return;
  }
  
  // 8 - Loss Probability
  arg = ssGetSFcnParam(S, 5);
  if (mxIsDoubleScalar(arg)) {
    nwsys->lossprob = *mxGetPr(arg);
  }
  if (nwsys->lossprob < -EPS || nwsys->lossprob > 1.0+EPS) {
    ssSetErrorStatus(S, "The loss probability must be between 0 and 1");
    return;
  }

  if (nwsys->type == FDMA) {

    // 9 - Bandwidth allocations
    arg = ssGetSFcnParam(S, 6);
    if (mxIsDouble(arg)) {
      if ((size_t)mxGetNumberOfElements(arg) != (size_t)nwsys->nbrOfNodes) {
	ssSetErrorStatus(S, "The number of bandwidth allocations must equal the number of nodes");
	return;
      }
      nwsys->bandwidths = new double[nwsys->nbrOfNodes];
      for (i=0; i<nwsys->nbrOfNodes; i++) {
	nwsys->bandwidths[i] = mxGetPr(arg)[i];
	//printf("bandwidth %d: %f\n", i, nwsys->bandwidths[i]);
      }
    } else {
      ssSetErrorStatus(S, "The bandwidth allocations must be a vector of doubles");
    }

  }

  if (nwsys->type == TDMA) {

    // 10 - Slotsize (translated into slottime)
    arg = ssGetSFcnParam(S, 7);
    if (mxIsDoubleScalar(arg)) {
      nwsys->slottime = *mxGetPr(arg)/nwsys->datarate;
    }
    if (nwsys->slottime <= 0.0) {
      ssSetErrorStatus(S, "The slot size must be > 0");
      return;
    }
    // printf("slottime: %f\n", nwsys->slottime);

    // 11 - Schedule
    arg = ssGetSFcnParam(S, 8);
    if (mxIsDouble(arg)) {
      nwsys->schedsize = mxGetNumberOfElements(arg);
    }
    if (nwsys->schedsize < 1) {
      	ssSetErrorStatus(S, "A schedule must be entered");
	return;
    }
    nwsys->schedule = new int[nwsys->schedsize];
    // printf("schedule: ");
    for (i=0; i<nwsys->schedsize; i++) {
      nwsys->schedule[i] = ((int)mxGetPr(arg)[i])-1;
      if (nwsys->schedule[i] < -1 || nwsys->schedule[i] > nwsys->nbrOfNodes-1) {
	ssSetErrorStatus(S, "Illegal node (< 0 or > nbrOfNodes) in schedule");
	return;
      }
      // printf("%d ", nwsys->schedule[i]);
    }
    // printf("\n");
  }

  if (nwsys->type == SFDSE) {

    // 12 - Switch memory size
    arg = ssGetSFcnParam(S, 9);
    if (mxIsDoubleScalar(arg)) {
      nwsys->memsize = (int) *mxGetPr(arg);
    }
    if (nwsys->memsize <= 0) {
      ssSetErrorStatus(S, "The switch memory size must be > 0");
      return;
    }
    nwsys->switchmem = nwsys->memsize;

    // 13 - Switch buffer type
    arg = ssGetSFcnParam(S, 10);
    if (mxIsDoubleScalar(arg)) {
      nwsys->buftype = (int)(*mxGetPr(arg))-1;
    }
    if (nwsys->buftype < 0) {
      ssSetErrorStatus(S, "Error in buffer type argument");
      return;
    }

    // 14 - Switch overflow behavior
    arg = ssGetSFcnParam(S, 11);
    if (mxIsDoubleScalar(arg)) {
      nwsys->overflow = (int)(*mxGetPr(arg))-1;
    }
    if (nwsys->overflow < 0) {
      ssSetErrorStatus(S, "Error in buffer type argument");
      return;
    }
  }

  /* Write pointer to Simulink block UserData */
  mexCallMATLAB(1, lhs, 0, NULL, "gcbh");
  sprintf(nwsysp,"%p",nwsys);
  rhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
  *mxGetPr(rhs[0]) = *mxGetPr(lhs[0]);
  rhs[1] = mxCreateString("UserData");
  rhs[2] = mxCreateString(nwsysp);
  mexCallMATLAB(0, NULL, 3, rhs, "set_param");

  /* Write pointer to MATLAB global workspace */
  /* FIX: The code above is intended to be removed and replaced by this. */
  /* Write rtsys pointer to global workspace */
  mxArray* var = mxCreateDoubleScalar(0.0);
  mexMakeArrayPersistent(var);
  *((long *)mxGetPr(var)) = (long) nwsys;
  char nwsysbuf[100];
  sprintf(nwsysbuf, "_nwsys_%d", nwsys->networkNbr);
  mexPutVariable("global", nwsysbuf, var);
 
  nwsys->inputs = new double[nwsys->nbrOfNodes];
  nwsys->oldinputs = new double[nwsys->nbrOfNodes];
  nwsys->outputs   = new double[nwsys->nbrOfNodes];
  nwsys->sendschedule = new double[nwsys->nbrOfNodes];

  for (i=0; i<nwsys->nbrOfNodes; i++) {
    nwsys->inputs[i] = 0.0;
    nwsys->oldinputs[i] = 0.0;
    nwsys->outputs[i] = 0.0;
    nwsys->sendschedule[i] = i+1;
  }

  nwsys->time = 0.0;
  nwsys->prevHit = 0.0;

  nwsys->nwnodes = new NWnode*[nwsys->nbrOfNodes];
  for (i=0; i<nwsys->nbrOfNodes; i++) {
    nwsys->nwnodes[i] = new NWnode();
    nwsys->nwnodes[i]->switchmem = nwsys->switchmem / nwsys->nbrOfNodes;
  }
  
  nwsys->waituntil = 0.0;
  nwsys->sending = -1;  // Note! -1 means nobody is sending
  nwsys->rrturn = nwsys->nbrOfNodes - 1; // want to start at 0
  nwsys->lasttime = -1.0;

  nwsys->slotcount = nwsys->schedsize - 1; // want to start at 0
  nwsys->currslottime = -nwsys->slottime;  // same here
 
}


#define MDL_INITIALIZE_CONDITIONS
static void mdlInitializeConditions(SimStruct *S)
{


}


static void mdlOutputs(SimStruct *S, int_T tid)
{
  int i, run = 0;

  nwsys = (RTnetwork*)ssGetUserData(S);

  nwsys->time = ssGetT(S);

  for (i=0; i < nwsys->nbrOfNodes; i++) {
    // printf("input %d: %f\n", i+1, input);
    if (fabs(nwsys->inputs[i]-nwsys->oldinputs[i]) > 0.1) {
      // printf("event at input %d\n", i);
      nwsys->oldinputs[i] = nwsys->inputs[i];
      nwsys->nextHit = runNetwork();
      run = 1;
    }
  }

  // not sure if this is really needed...
  if (run == 0 && nwsys->time >= nwsys->nextHit) {
    nwsys->nextHit = runNetwork();
  }

  for (i=0; i<nwsys->nbrOfNodes; i++) {
    ssGetOutputPortRealSignal(S,0)[i] = nwsys->outputs[i];
  }

  for (i=0; i<nwsys->nbrOfNodes; i++) {
    ssGetOutputPortRealSignal(S,1)[i] = nwsys->sendschedule[i];
  }

} 


#define MDL_ZERO_CROSSINGS

static void mdlZeroCrossings(SimStruct *S)
{
  int i;

  nwsys = (RTnetwork*)ssGetUserData(S);

  /* Copy inputs and check for events */
  for (i=0; i < nwsys->nbrOfNodes; i++) {
    if (fabs(*ssGetInputPortRealSignalPtrs(S,0)[i] - nwsys->inputs[i]) > 0.1) {
      nwsys->nextHit = ssGetT(S);
    }
    nwsys->inputs[i] = *ssGetInputPortRealSignalPtrs(S,0)[i];
  }
  ssGetNonsampledZCs(S)[0] = nwsys->nextHit - ssGetT(S);
}



static void mdlTerminate(SimStruct *S)
{
  nwsys = (RTnetwork*) ssGetUserData(S);
  if (nwsys == NULL) {
    return;
  }

  if (nwsys->inputs) delete[] nwsys->inputs;
  if (nwsys->sendschedule) delete[] nwsys->sendschedule;
  if (nwsys->outputs) delete[] nwsys->outputs;
  if (nwsys->oldinputs) delete[] nwsys->oldinputs;
  if (nwsys->schedule) delete[] nwsys->schedule;
  if (nwsys->bandwidths) delete[] nwsys->bandwidths;

  char nwsysbuf[100];
  mxArray* rhs[2];
  sprintf(nwsysbuf, "_nwsys_%d", nwsys->networkNbr);
  rhs[0] = mxCreateString("global");
  rhs[1] = mxCreateString(nwsysbuf);
  mexCallMATLAB(0, NULL, 2, rhs, "clear"); 

  delete nwsys;
}


#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif

#ifdef __cplusplus
} // end of extern "C" scope
#endif

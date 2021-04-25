/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];

    /* Local vars */
    int proctmp;
    int pagetmp;

    /* initialize static vars on first run */
    if(!initialized){
	for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
	    for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
		timestamps[proctmp][pagetmp] = 0; 
	    }
	}
	initialized = 1;
    }
    
    /* TODO: Implement LRU Paging */
    int pc;
    int page;
    int evicted;

    for(int process = 0; process < MAXPROCESSES; process++){

      // Get the program counter for current process and the requested page.
      pc = q[process].pc;
      page = pc/PAGESIZE;

      // Check if the requested page is already paged in.
      if(q[process].pages[page] != 1){

	// If the page table is full, find the least recently used page.
	if(pagein(process, page) != 1){

	  int timestamp = tick;

	  // Sweep through the page table for the current process and find the oldest page therein.
	  for(int current = 0; current < q[process].npages; current++){
	    if(q[process].pages[current] == 1 && timestamps[process][current] < timestamp){
	      timestamp = timestamps[process][current];
	      evicted = current;
	    }
	  }

	  // Swap out the page selected for eviction.
	  pageout(process, evicted);
	}
      }
      timestamps[process][page] = tick;
      
    }

    /* advance time for next pageit iteration */
    tick++;
} 

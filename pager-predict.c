/*
 * File: pager-predict.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains a predictive pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for a predictive pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int lastpc[MAXPROCESSES];
    static int type[MAXPROCESSES];
    
    /* Local vars */
    int process;
    int pc;
    int page;
    int jump_length;

    /* initialize static vars on first run */
    if(!initialized){
      /* Init complex static vars here */
      for(int i = 0; i < MAXPROCESSES; i++){
	lastpc[i] = -1;
	type[i] = 4;
      }
      initialized = 1;
    }
    
    /* TODO: Implement Predictive Paging */

    for (process = 0; process < MAXPROCESSES; process++){

      // Only work with the currently active process.
      if(q[process].active != 1){
	continue;
      }

      // Compute the current program counter, requested page and the length of the pc-jump (if any).
      pc = q[process].pc;
      page = pc/PAGESIZE;
      jump_length = lastpc[process] - pc;

      // Using the values given by the pseudocode in the assignment write-up, we can determine what type of program the curent process is.
      // By default, the paging algorithm assumes a linear structure (type 4 in the write-up), but as the program runs, it reclassifies the process
      // with a heuristic of pc-jump length.
      if((jump_length > 1 || jump_length < -1) && type[process] == 4){

	if(jump_length == -902 || jump_length == -132 || jump_length == 1533 || jump_length == 1401){
	  // If jump length is -902, -132, 1533, or 1401 then we are dealing with a program that has a loop with an inner branch (see write-up).	  
	  type[process] = 1;
	  
	}else if(jump_length == 1129){
	  // If jump length is 1129, we are dealing with a program with a single loop (see write-up).	  
	  type[process] = 2;
	  
	}else if(jump_length == 516 || jump_length == 1683){
	  // If jump length is -516 orr 1683 we are dealing with a program with a nested loop strucure (see write-up).	  
	  type[process] = 3;
	  
	}else if(jump_length == 501 || jump_length == 503){
	  // If jump length is 501 or 503, we are dealing with a program with a probabalistic backwards branch (see write-up).	  
	  type[process] = 5;
	  
	}
      }

      // If we have reached the end of each process, set last pc value to -1 and start with the assumption that the new process is a linear program.
      if((type[process] == 1 && pc == 1534) || (type[process] == 2 && pc == 1130) || (type[process] == 3 && pc == 1684)
	 || (type[process] == 4 && pc == 1911) || (type[process] == 5 && pc == 504)) {

	type[process] = 4;
	lastpc[process] = -1;
      }

      // Page in the currently requested page.
      pagein(process, page);

      // Predict the requested page based on the program type determined above.
      if(type[process] == 1){

	/*if(process == 1 && (pc != lastpc[process] || pc == 0)){
	  FILE* out1 = fopen("type1.csv", "a");
	  fprintf(out1, "Page: %d, pc: %d, pid: %d\n", page, pc, process);
	  fclose(out1);
	  }*/

	/* If the program is type 1 (from the write-up) page in the next pages based on the current page. 
	 * When the current page is page 0, type 1 programs always request page 1 next, when the current page is
	 * page 3, type 1 programs may request page 4, 10, or 11.  If the current page is 4, type 1 programs always
	 * request page 5, and we page out pages 3 and 10.  When the curent page is page 10, type 1 programs may request
	 * either page 11 or page 0, and we page out pages 9, 3, and 4.  At page 11, we page in page 0 and page out page 10.
	 * If the current page is any other page, it will suffice to page in the next sequential page and page out the previous
	 * sequential page.
	 */
	if(page == 0){
	
	  pagein(process, 1);
	  pageout(process, 11);
	  
	}else if(page == 3){
	  
	  pagein(process, 4);
	  pagein(process, 10);
	  pagein(process, 11);
	  pageout(process, 2);
	  
	}else if(page == 4){
	  
	  pagein(process, 5);
	  pageout(process, 3);
	  pageout(process, 10);
	  
	}else if(page == 10){
	  
	  pagein(process, 11);
	  pagein(process, 0);
	  pageout(process, 9);
	  pageout(process, 3);
	  pageout(process, 4);
	  
	}else if(page == 11){
	  
	  pagein(process, 0);
	  pageout(process, 10);
	  
	}else{
	  
	  pagein(process, page+1);
	  pageout(process, page-1);
	  
	}
	
      }else if(type[process] == 2){
	// If the program is type 2 (from the write-up), predict the next pages based on the current page.

	/* If the current page is page 0, type 2 programs always request page 1 next, and if the curent page is page 
	 * 8, type 2 programs always request page 0 next.  If the current page is any othe page, it will suffice to page
	 * in the next sequential page and page out the previous sequential page.
	 */
	if(page == 0){
	  
	  pagein(process, 1);
	  pageout(process, 8);
	  
	}else if(page == 8){
	  
	  pagein(process, 0);
	  pageout(process, 7);
	  
	}else{
	  
	  pagein(process, page+1);
	  pageout(process, page-1);
	  
	}
	
      }else if(type[process] == 3){
	// If the program is type 3 (from the write-up), predict the next pages based on the current page.

	/* If the current page is page 0, type 3 programs will always request page 1 next, and the previous page may have been either page 9 or 13.  
	 * If the current page is page 9, type 3 programs will always request page 10 next and the previous page may have been either page 0 or 13.
	 * If the current page is page 12, type 3 programs may request page 0, 9, or 13 next, and if the current page is 13, type 3 programs may 
	 * request page 0 or 9 next.  Otherwise, it is sufficient to page in the next sequential page and page out the previous sequential page.
	 */
	if(page == 0){
	  
	  pagein(process, 1);
	  pageout(process, 9);
	  pageout(process, 13);
	  
	}else if(page == 9){
	  
	  pagein(process, 10);
	  pageout(process, 0);
	  pageout(process, 13);
	  
	}else if(page == 12){
	  
	  pagein(process, 0);
	  pagein(process, 9);
	  pagein(process, 13);
	  pageout(process, 11);
	  
	}else if(page == 13){
	  
	  pagein(process, 0);
	  pagein(process, 9);
	  pageout(process, 12);
	  
	}else{
	  
	  pagein(process, page+1);
	  pageout(process, page-1);
	  
	}
	
      }else if(type[process] == 5){
	//If the program is type 3 (from the write-up), predict the next page based on the current page.

	/* If the current page is page 0, type 5 programs always request page 1 next, and the previous page is either page 3 or page 4.
	 * If the current page is page 3, type 5 programs always request page 0 next, and the previous page is always 2.  Otherwise,
	 * it should be sufficient to page in the next sequential page and page out the previous sequential page.
	 */
	if(page == 0){
	  
	  pagein(process, 1);
	  pageout(process, 3);
	  pageout(process, 4);
	  
	}else if(page == 3){
	  
	  pagein(process, 0);
	  pageout(process, 2);
	  
	}else{
	  
	  pagein(process, page+1);
	  pageout(process, page-1);
	  
	}
      }else{
	// If the program type is not 1, 2, 3 or 5, assume that the process is a linear program.  After examining the page requests, I found that
	// linear programs always request pages in sequential order.

	/*if(process == 1 && (pc != lastpc[process] || pc == 0)){
	  FILE* out = fopen("type4.csv", "a");
	  fprintf(out, "Page: %d, pc: %d, pid: %d\n", page, pc, process);
	  fclose(out);
	  }*/

	pagein(process, page+1);
	pageout(process, page-1);
	  
      }

      // Mark the last value of the program counter for this process.
      lastpc[process] = pc;
    }
    
    /* advance time for next pageit iteration */
    tick++;
} 

/* 
    File: semaphore.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 08/02/11

*/

#ifndef _semaphore_H_                   // include file only once
#define _semaphore_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <pthread.h>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   S e m a p h o r e  */
/*--------------------------------------------------------------------------*/


class Semaphore {
private:

  int             value;
  pthread_mutex_t m;
  pthread_cond_t  c;
  void Lock() { pthread_mutex_lock(&m); } 
  void Unlock() { pthread_mutex_unlock(&m); }
public:

  /* -- CONSTRUCTOR/DESTRUCTOR */

  Semaphore(int _val){
    value = _val;
    m = PTHREAD_MUTEX_INITIALIZER;
    c = PTHREAD_COND_INITIALIZER;
  }

  //~Semaphore(){}

  /* -- SEMAPHORE OPERATIONS */

  int P(){
    Lock();
    while(value <= 0)//wait for semaphore to be ready
      pthread_cond_wait(&c, &m);
    value--;
    Unlock();
  }
  int V(){
    Lock();
    int last_val = value;
    value++;
    Unlock();
    if(last_val == 0)//signal process to wake up
      pthread_cond_signal(&c);
  }
};


#endif



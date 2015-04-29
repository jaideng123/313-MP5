/* 
/* 
    File: dataserver.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/16

    Dataserver main program for MP3 in CSCE 313
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "reqchannel.H"
#include "NetworkRequestChannel.H"

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* VARIABLES */
/*--------------------------------------------------------------------------*/

static int nthreads = 0;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

void handle_process_loop(RequestChannel & _channel);

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

string int2string(int number) {
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}
void process_hello(int sock, string & _request) {
	char* data = &_request[0];
	write(sock,data,strlen(_request.c_str())+1);
}

void process_data(int sock, const string &  _request) {
  usleep(1000 + (rand() % 5000));
  string r = int2string(rand() % 100);
  char* data = const_cast<char*>(r.c_str());
  write(sock,data,strlen(r.c_str())+1);
}
/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THREAD FUNCTIONS */
/*--------------------------------------------------------------------------*/


void* connection_handler(void* args) {
  int socket = *(int*)args;
  int continue_processing = 1;
    
  while(continue_processing) {
    char buf[1000];
        
    if (read(socket, buf, 1000) < 0) {
      perror(string("SERVER ERROR: Error reading from pipe!").c_str());
    } 
        
    string request = buf;
	if (request.compare(0, 4, "quit") == 0) {
		continue_processing = 0;
	}
	else if (request.compare(0, 5, "hello") == 0) {
		process_hello(socket, request);
	}
	else if (request.compare(0, 4, "data") == 0) {
		process_data(socket, request);
	}
  }
  close(socket);

}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
  int sock = 20000;
  int back_log = 10;
  int c;
  while ((c = getopt (argc, argv, "b:s:h:")) != -1) {
  switch(c) {
	case 'b':
		back_log = atoi(optarg);
		break;
	case 's':
		sock = atoi(optarg);
		break;
	case '?':
		return 1;
	default:
		abort();
	}
  }
  //  cout << "Establishing control channel... " << flush;
  NetworkRequestChannel control_channel(sock,connection_handler,back_log);
  cout<<"Server Closed";

}


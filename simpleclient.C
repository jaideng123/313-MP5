/* 
    File: simpleclient.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2013/01/31

    Simple client main program for MP3 in CSCE 313
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
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <cstring>
#include <algorithm>  

#include <errno.h>
#include <unistd.h>

#include "reqchannel.H"
#include "BoundedBuffer.H"
#include <pthread.h>

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

struct Arguments{
  char id;
  int rep;
  BoundedBuffer* b;
  string channel;
  BoundedBuffer * b1;
  BoundedBuffer * b2;
  BoundedBuffer * b3;
};

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/
vector< vector<int> > hist(3);
void *request(void *param){
  Arguments *args = (Arguments *)param;
  for(int i = 0; i < args->rep; ++i){
    Item item(args->id,"data "+string(1,args->id)+" Smith");
    args->b->add(item);
  }
  args->b->finished();
}
void *worker(void *param){
  Arguments* arg = (Arguments *)param;
  RequestChannel chan(arg->channel, RequestChannel::CLIENT_SIDE);
  while(arg->b->numFinished < 3 || arg->b->getSize() > 0){
    Item i = arg->b->remove();
    if(i.getMessage() != "NULL" && i.getPerson() != 'n'){
      i.setData(chan.send_request(i.getMessage()));
      cout<<"\nResponse: "<<i.getData()<<i.getPerson()<<endl;
      if(i.getPerson() == 'j')
        arg->b1->add(i);
      else if(i.getPerson() == 'l')
        arg->b2->add(i);
      else if(i.getPerson() == 'g')
        arg->b3->add(i); 
    }
  }
  arg->b1->finished();
  arg->b2->finished();
  arg->b3->finished();
  chan.send_request("quit");
}
void printHistogram(int i){
    string name = "JLG";
    cout<<"Histogram: "<<name[i]<<endl;
    int x = 0;
    for (int j = 0; j < 100; ++j)
    {
      cout<<j<<":";
      for (int k = 0; k < hist[i][j]; ++k){
        cout<<"*";
        ++x;
      }
      cout<<endl;
    }
    cout<<"Requests :"<<x<<endl;
}
void printHist(){
	cout << "Range\tJohn Smith\tJoeSmith\tJaneSmith" << endl;
	int total = 0;
	int curfreq = 0;
	for(int i = 0; i < 10; ++i){
	cout << i*10 << "-" << (i*10)+9 << '\t';
		for(int p = 0; p < 3; ++p){
			for (int j = i*10; j < ((i*10)+10); ++j){
			  for (int k = 0; k < hist[p][j]; ++k){
					++curfreq;
				}
			}
			cout << curfreq << "\t\t";
			total += curfreq;
			curfreq = 0;
		}
		cout << endl;
	}
	cout << "Total = " << total << endl;
}
void *histogram(void *param){
	Arguments* arg = (Arguments *)param;
	while(arg->b->numFinished < arg->rep || arg->b->getSize() > 0){
		 Item i = arg->b->remove();
		 if(i.getMessage() != "NULL" && i.getPerson() != 'n'){
			 cout<<"Data:"<<i.getData()<<endl;
			if(i.getPerson() == 'j')
				hist[0][atoi(i.getData().c_str())]++;
			else if(i.getPerson() == 'l')
				hist[1][atoi(i.getData().c_str())]++;
		   	else if(i.getPerson() == 'g')
				hist[2][atoi(i.getData().c_str())]++;
		 }
	}
}

int main(int argc, char * argv[]) {
  pid_t parent = getpid();
  pid_t pid = fork();
  if (pid == -1)
  {
    // error, failed to fork()
  } 
  else if(pid == 0){
    // we are the child
     char *arg[12];
    if(execv("./dataserver",argv) == -1)
      cout<<"Error, fork failed"<<endl;
    return -1;
  }
  int c;
  int index;
  int n = 10; //number of data requests per person
  int bb = 15; //size of bounded buffer in requests
  int w = 3; //number of worker threads
  while ((c = getopt (argc, argv, "n:b:w:")) != -1) {
  switch(c) {
	case 'n':
		n = atoi(optarg);
		break;
	case 'b':
		bb = atoi(optarg);
		break;
	case 'w':
		w = atoi(optarg);
		break;
	case '?':
		return 1;
	default:
		abort();
	}
  }
  cout << "n: " << n << " b: " << bb << " w: " << w << endl;
  vector<pthread_t> clients(3);
  vector<pthread_t> workers(w);
  vector<pthread_t> histWorkers(3);
  vector<int> temp(100);
  hist[0] = temp;
  hist[1] = temp;
  hist[2] = temp;
  RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
  Semaphore s(1);
  BoundedBuffer b(bb,&s);
  Arguments arg1;
  arg1.id = 'j';
  arg1.b = &b;
  arg1.rep = n;
  pthread_create(&clients[0], NULL, request, &arg1);
  Arguments arg2;
  arg2.id = 'l';
  arg2.b = &b;
  arg2.rep = n;
  pthread_create(&clients[1], NULL, request, &arg2);
  Arguments arg3;
  arg3.id = 'g';
  arg3.b = &b;
  arg3.rep = n;
  pthread_create(&clients[2], NULL, request, &arg3);
    //usleep(10000000);
  vector<Arguments> arr(w);
  BoundedBuffer hist1(bb,&s);
  BoundedBuffer hist2(bb,&s);
  BoundedBuffer hist3(bb,&s);
  for (int i = 0; i < w; ++i)
  {
    arr[i].channel = chan.send_request("newthread");
    arr[i].b = &b;
	arr[i].b1 = &hist1;
	arr[i].b2 = &hist2;
	arr[i].b3 = &hist3;
    pthread_create(&workers[i], NULL, worker, &arr[i]);
  }
  Arguments arg4;
  arg4.id = 'j';
  arg4.rep = w;
  arg4.b = &hist1;
  pthread_create(&histWorkers[0], NULL, histogram, &arg4);
  Arguments arg5;
  arg5.id = 'l';
  arg5.rep = w;
  arg5.b = &hist2;
  pthread_create(&histWorkers[1], NULL, histogram, &arg5);
  Arguments arg6;
  arg6.id = 'g';
  arg6.rep = w;
  arg6.b = &hist3;
  pthread_create(&histWorkers[2], NULL, histogram, &arg6);
  
  for (int i = 0; i < clients.size(); ++i)
  {
    pthread_join(clients[i], NULL);
  }
  for (int i = 0; i < w; ++i)
  {
    pthread_join(workers[i], NULL);
  }
  for (int i = 0; i < 3; ++i)
  {
    pthread_join(histWorkers[i], NULL);
  }
  chan.send_request("quit");
  usleep(1000000);
  
	printHist();
  //for getopt failures
	for(index = optind; index < argc; ++index){
		printf("Non-option argument %s\n", argv[index]);
	}
	return 0;
}

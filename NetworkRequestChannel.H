#include<string>
#include<iostream>
#include<unistd.h>
#include<sys/time.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>


class NetworkRequestChannel {
public:
	NetworkRequestChannel(const string _server_host_name, const unsigned short _port_no){}
	/* Creates a CLIENT-SIDE local copy of the channel. The channel is connected
	to the given port number at the given server host.
	THIS CONSTRUCTOR IS CALLED BY THE CLIENT. */
	NetworkRequestChannel(const unsigned short _port_no,
	void * (*connection_handler) (int *)){
		//open socket and listen
		int fd, optval = 1;
		struct sockaddr_in addr;
		fd = socket(AF_INET,SOCK_STREAM,0);
		assert(fd > -1);
		setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(const void *) &optval,sizeof(int));
		bzero((char*) &addr,sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = honl(INADDR_ANY);
		addr.sin_port = htons(_port_no);
		//bind
		int rc = bind(fd,(struct sockaddr*) &addr,sizeof(addr));
		assert(rc == 0);
		//listen
		rc = listen(fd,1024);
		assert(rc == 0);
		//now for the good stuff
		int mainfd = fd;
		
		
		
	}
	/* Creates a SERVER-SIDE local copy of the channel that is accepting connections
	at the given port number.
	NOTE that multiple clients can be connected to the same server-side end of the
	request channel. Whenever a new connection comes in, it is accepted by the
	the server, and the given connection handler is invoked. The parameter to
	the connection handler is the file descriptor of the slave socket returned
	by the accept call.
	NOTE that the connection handler does not want to deal with
	closing the socket. You will have to close the socket once the
	connection handler is done. */
	~NetworkRequestChannel(){}
	/* Destructor of the local copy of the channel. */
	string send_request(string _request){}
	/* Send a string over the channel and wait for a reply. */
	string cread(){}
	/* Blocking read of data from the channel. Returns a string of characters
	read from the channel. Returns NULL if read failed. */
	int cwrite(string _msg){}
	/* Write the data to the channel. The function returns the number of
	characters written to the channel. */
private:
};
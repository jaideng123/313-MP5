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
#include <sstream>


class NetworkRequestChannel {
public:
	int connfd;
	struct sockaddr_in serverIn;
	
	int createServerConnection(const char * svc)
	{

	memset(&serverIn, 0, sizeof(serverIn));
	serverIn.sin_family = AF_INET;
	serverIn.sin_addr.s_addr = INADDR_ANY;

	if(struct servent * pse = getservbyname(svc, "tcp"))
		serverIn.sin_port = pse->s_port;

	else if((serverIn.sin_port = htons((unsigned short)atoi(svc))) == 0)
		cerr << "can't get port\n";


	int socknum  = socket(AF_INET, SOCK_STREAM, 0);

	if(socknum < 0)
		cerr << "can't create socket \n";

	if(bind(socknum, (struct sockaddr *)&serverIn, sizeof(serverIn)) < 0)
		cerr << "can't bind...\n";

	listen(socknum,30);//check for connections

	return socknum;
	}
	
	NetworkRequestChannel(const string _server_host_name, const unsigned short _port_no){
		struct sockaddr_in sockIn;
		memset(&sockIn, 0, sizeof(sockIn));
		sockIn.sin_family = AF_INET;
		char str[15];
		sprintf(str, "%d", _port_no);
		if(struct servent * pse = getservbyname(str, "tcp"))//make port
			sockIn.sin_port = pse->s_port;

		else if ((sockIn.sin_port = htons((unsigned short)_port_no)) == 0)
			cerr << "cant connect port\n";

		if(struct hostent * hn = gethostbyname(_server_host_name.c_str()))
			memcpy(&sockIn.sin_addr, hn->h_addr, hn->h_length);

		else if((sockIn.sin_addr.s_addr = inet_addr(_server_host_name.c_str())) == INADDR_NONE)
			cerr << "cant determine host <" <<  _server_host_name << ">\n";

		int s = socket(AF_INET, SOCK_STREAM, 0);
		if(s < 0)
			cerr << "cant create socket\n";

		if(connect(s, (struct sockaddr *)&sockIn, sizeof(sockIn)) < 0)
			cerr << "cant connect to " <<  _server_host_name << ":" << _port_no<<endl;
		connfd = s;
	}
	/* Creates a CLIENT-SIDE local copy of the channel. The channel is connected
	to the given port number at the given server host.
	THIS CONSTRUCTOR IS CALLED BY THE CLIENT. */
	
	NetworkRequestChannel(const unsigned short _port_no,
	void * (*connection_handler) (void *)){
		stringstream ss;
	ss << _port_no;
	string port = ss.str();

	int master = createServerConnection(port.c_str());
	int serverSize = sizeof(serverIn);


	while(true)
	{
		int * slave = new int;

		pthread_t thread;
		pthread_attr_t attr;
		pthread_attr_init(&attr);


		*slave = accept(master,(struct sockaddr*)&serverIn, (socklen_t*)&serverSize);

		if(slave < 0)
		{
			delete slave;

			if(errno == EINTR)
                continue;//retry
			else cerr << "unknown error in accept()\n";
		}

		pthread_create(&thread, &attr, connection_handler, (void*)slave);


	}
	cout << "Connection complete\n";
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
	~NetworkRequestChannel(){
		close(connfd);
	}
	/* Destructor of the local copy of the channel. */
	string send_request(string _request){
		cwrite(_request);
		return cread();
	}
	/* Send a string over the channel and wait for a reply. */
	string cread(){
		string rc;
		char data[1000];
		read(connfd,data,1000);
		rc = data;
		return rc;
	}
	/* Blocking read of data from the channel. Returns a string of characters
	read from the channel. Returns NULL if read failed. */
	int cwrite(string _msg){
		char* data = &_msg[0];
		int success = write(connfd,data,strlen(_msg.c_str())+1);
		if(success == -1)
			cout<<"Bad Write\n";
		return success;

	}
	/* Write the data to the channel. The function returns the number of
	characters written to the channel. */

private:
};
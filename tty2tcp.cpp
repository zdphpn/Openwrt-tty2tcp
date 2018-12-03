//============================================================================
// Name        : frpoj.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <fcntl.h>
#include <sys/types.h>

#include <iostream>
#include <list>
using namespace std;

void error(const char *msg)
{
	fputs(msg,stderr);
	exit(1);
}

static void ser_set()
{

}

char recv_data[100];
int recv_len;

int main() {

	int serv_sock,clnt_sock;							//socket handle
	struct sockaddr_in serv_addr,clnt_addr;				//socket address
	socklen_t clnt_addr_size;							//client address size

	fd_set reads,cpy_reads;								//select fd
	int fd_min,fd_max,fd_ser;							//min&max fd

	list<int> clnt_list;								//client list



	serv_sock=socket(PF_INET,SOCK_STREAM,0);			//make socket
	if(serv_sock<0)
	{
		error("socket() error\n");
	}
	else
	{
		printf("socket() success\n");
	}

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;						//TCP
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);		//local address
	serv_addr.sin_port=htons(9000);						//local port

														//bind
	if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
	{
		error("bind() error\n");
	}
	else
	{
		printf("bind() success\n");
	}
	if(listen(serv_sock,5)<0)							//listen
	{
		error("listen() error\n");
	}
	else
	{
		printf("listen() success\n");
	}

	FD_ZERO(&reads);
	FD_SET(serv_sock,&reads);							//set fd is the first handle(server socket)
	fd_min=serv_sock;


	fd_ser=open("/dev/ttyS1",O_RDWR|O_NOCTTY|O_NDELAY);
	if(fd_ser<0)
	{
		error("serial open error\n");
	}
	else
	{
		printf("serial open success\n");
	}
    FD_SET(fd_ser,&reads);
	fd_max=fd_ser;

	ser_set();


	while(1)
	{
		cpy_reads=reads;								//copy fd

		if(select(fd_max+1,&cpy_reads,0,0,NULL)>0)		//select,it block until fd state change
		{
			for(int i=fd_min;i<fd_max+1;i++)			//for
			{
				if(FD_ISSET(i,&cpy_reads))				//get changed fd
				{
					if(i==serv_sock)					//if server socket
					{
						clnt_addr_size=sizeof(clnt_addr);
														//accept
						clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
						FD_SET(clnt_sock,&reads);		//add fd
						if(fd_max<clnt_sock)			//update fd_max
						{
							fd_max=clnt_sock;
						}
						printf("str con:%d\n",clnt_sock);

						clnt_list.push_front(clnt_sock);//add to client list
					}
					else if(i==fd_ser)
					{
						recv_len=read(i,recv_data,10);
						if(recv_len>0)
						{
							list<int>::iterator plist;	//list iterator

							for(plist=clnt_list.begin();plist!=clnt_list.end();plist++)
							{							//send to all client
								write(*plist,recv_data,recv_len);
							}
						}
					}
					else								//other
					{
						recv_len=read(i,recv_data,10);	//read
						if(recv_len==0)					//0 is close
						{
							FD_CLR(i,&reads);			//delete fd
							close(i);
							printf("end con:%d\n",i);

							clnt_list.remove(i);		//delete from client list
						}
						else
						{
							write(fd_ser,recv_data,recv_len);
						}
					}
				}
			}
		}
	}

	close(serv_sock);
	printf("bye");

	return 0;
}

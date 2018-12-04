
//============================================================================
// Name        : tty2tcp.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : tty2tcp in C++, Ansi-style
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
#include <termios.h>

#include <iostream>
#include <list>
using namespace std;


char recv_data[100];                                    //data
int recv_len;                                           //data len

int serv_sock, fd_ser;                                  //server socket&serial fd
list<int> clnt_list;                                    //client list


static void error(const char *msg)
{
    fputs(msg, stderr);

    list<int>::iterator plist;                          //list iterator
    for(plist = clnt_list.begin(); plist != clnt_list.end(); plist++)
    {
    	if(*plist>0)
        {
            close(*plist);
        }
    }
    if(serv_sock>0)
    {
        close(serv_sock);                               //close
    }
    if(fd_ser>0)
    {
        close(fd_ser);
    }

    exit(1);
}

static void ser_set(int fd)
{
    struct termios opt;

    bzero(&opt,sizeof(opt));
    opt.c_cflag|=(CLOCAL|CREAD);

    opt.c_cflag&=(~CSIZE);
    opt.c_cflag|=CS8;                                   //8bit

    opt.c_cflag&=(~PARENB);                             //no parity

    cfsetispeed(&opt,B115200);                          //115200bps
    cfsetospeed(&opt,B115200);

    opt.c_cflag&=(~CSTOPB);                             //1 stop bit

    opt.c_lflag|=(ICANON|ECHO);                         //ican&echo

    if(tcsetattr(fd,TCSANOW,&opt)<0)                    //set
    {
        error("tcsetattr() error\n");
    }
}


int main()
{

    int clnt_sock;                                      //client socket handle
    struct sockaddr_in serv_addr, clnt_addr;            //socket address
    socklen_t clnt_addr_size;                           //client address size

    fd_set reads, cpy_reads;                            //select fd
    int fd_min, fd_max;                                 //min&max fd

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);        //make socket
    if(serv_sock < 0)
    {
        error("socket() error\n");
    }
    else
    {
        printf("socket() success\n");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;                     //TCP
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);      //local address
    serv_addr.sin_port = htons(9000);                   //local port

                                                        //bind
    if(bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("bind() error\n");
    }
    else
    {
        printf("bind() success\n");
    }
    if(listen(serv_sock, 5) < 0)                        //listen
    {
        error("listen() error\n");
    }
    else
    {
        printf("listen() success\n");
    }

    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);                          //set fd is the first handle(server socket)
    fd_min = serv_sock;


    fd_ser = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd_ser < 0)
    {
        error("serial open error\n");
    }
    else
    {
        printf("serial open success\n");
    }
    FD_SET(fd_ser, &reads);                             //add fd
    fd_max = fd_ser;

    ser_set(fd_ser);                                    //serial set


    while(1)
    {
        cpy_reads = reads;                              //copy fd

        if(select(fd_max + 1, &cpy_reads, 0, 0, NULL) > 0)//select,it block until fd state change
        {
            for(int i = fd_min; i < fd_max + 1; i++)    //for
            {
                if(FD_ISSET(i, &cpy_reads))             //get changed fd
                {
                    if(i == serv_sock)                  //if server socket
                    {
                        clnt_addr_size = sizeof(clnt_addr);
                                                        //accept
                        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
                        FD_SET(clnt_sock, &reads);      //add fd
                        if(fd_max < clnt_sock)          //update fd_max
                        {
                            fd_max = clnt_sock;
                        }
                        printf("str con:%d\n", clnt_sock);

                        clnt_list.push_front(clnt_sock);//add to client list
                    }
                    else if(i == fd_ser)                //if serial
                    {
                        recv_len = read(i, recv_data, sizeof(recv_data));
                        if(recv_len > 0)
                        {
                            list<int>::iterator plist;  //list iterator

                            for(plist = clnt_list.begin(); plist != clnt_list.end(); plist++)
                            {
                                                        //send to all client
                                write(*plist, recv_data, recv_len);
                            }
                        }
                    }
                    else                                //other
                    {                                   //read
                        recv_len = read(i, recv_data, sizeof(recv_data));
                        if(recv_len == 0)               //0 is close
                        {
                            FD_CLR(i, &reads);          //delete fd
                            close(i);
                            printf("end con:%d\n", i);

                            clnt_list.remove(i);        //delete from client list
                        }
                        else
                        {
                            write(fd_ser, recv_data, recv_len);//write to serial
                        }
                    }
                }
            }
        }
    }

    list<int>::iterator plist;                           //list iterator
    for(plist = clnt_list.begin(); plist != clnt_list.end(); plist++)
    {
        close(*plist);
    }
    close(serv_sock);                                    //close
    close(fd_ser);
    printf("bye");

    return 0;
}



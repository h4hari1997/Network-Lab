////first code cliient.....................
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
using namespace std;

int create_socket(int,char *);

#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

#define MAXLINE 4096 /*max text line length*/

int
main(int argc, char **argv)
{
 int sockfd;
 struct sockaddr_in servaddr;
 char sendline[MAXLINE], recvline[MAXLINE];

 //basic check of the arguments
 //additional checks can be inserted
 if (argc !=3) {
  cerr<<"Usage: ./a.out <IP address of the server> <port number>"<<endl;
  exit(1);
 }

 //Create a socket for the client
 //If sockfd<0 there was an error in the creation of the socket
 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  cerr<<"Problem in creating the socket"<<endl;
  exit(2);
 }

 //Creation of the socket
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr= inet_addr(argv[1]);
 servaddr.sin_port =  htons(atoi(argv[2])); //convert to big-endian order

 //Connection of the client to the socket
 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  cerr<<"Problem in connecting to the server"<<endl;
  exit(3);
 }

 cout<<"ftp>";
int flag=0;
 while (fgets(sendline, MAXLINE, stdin) != NULL) {

  send(sockfd, sendline, MAXLINE, 0);
  char *token,*dummy;
  dummy=sendline;
  token=strtok(dummy," ");
  int child;
  
   
   if (strcmp("GOODBYE\n",sendline)==0)  {
        char m1[MAXLINE];
        recv(sockfd, m1, MAXLINE,0);
        cout<<m1;
       if(flag!=0)
        kill(0, SIGKILL);
   	close(sockfd);
	return 0;
   }


   else if (strcmp("RETR",token)==0)  {
   	char port[MAXLINE], buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE],message[MAXLINE];
	int data_port,datasock,lSize,num_blks,num_last_blk,i;
	FILE *fp;
	recv(sockfd, port, MAXLINE,0);
       
	data_port=atoi(port);
	datasock=create_socket(data_port,argv[1]);
	token=strtok(NULL," \n");
	recv(datasock,message,MAXLINE,0);
	if(strcmp("1",message)==0){
		if((fp=fopen(token,"w"))==NULL)
			{cout<<"Error in creating file\n";
                         close(datasock);
                        }
		else
		{     flag=1;  
		    child=fork();
		   if(child!=0)
		   {
		        	
			
			recv(datasock, char_num_blks, MAXLINE,0);
			num_blks=atoi(char_num_blks);
			for(i= 0; i < num_blks; i++) { 
				recv(datasock, buffer, MAXLINE,0);
				fwrite(buffer,sizeof(char),MAXLINE,fp);
				//cout<<buffer<<endl;
			}
			recv(datasock, char_num_last_blk, MAXLINE,0);
			num_last_blk=atoi(char_num_last_blk);
			if (num_last_blk > 0) { 
				recv(datasock, buffer, MAXLINE,0);
				fwrite(buffer,sizeof(char),num_last_blk,fp);
				//cout<<buffer<<endl;
			}
			fclose(fp);
			//cout<<"File download done."<<endl;
                        close(datasock);
                        
		}
		else
		{
		cout<<"tranfering in background started\n";
		}
		}
		
	}
	else{    
		fflush(stdin);
		char time[100];
		recv(datasock,time,MAXLINE,0);
		cout<<"<"<<time<<">"<<message<<endl;
                close(datasock);
	}
   }
   else{
	cerr<<"Error in command. Check Command"<<endl;
   }
   cout<<"ftp>";

 }

 exit(0);
}


int create_socket(int port,char *addr)
{
 int sockfd;
 struct sockaddr_in servaddr;


 //Create a socket for the client
 //If sockfd<0 there was an error in the creation of the socket
 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  cerr<<"Problem in creating the socket"<<endl;
  exit(2);
 }

 //Creation of the socket
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr= inet_addr(addr);
 servaddr.sin_port =  htons(port); //convert to big-endian order

 //Connection of the client to the socket
 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  cerr<<"Problem in creating data channel"<<endl;
  exit(3);
 }

return(sockfd);
}
//first server
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <unistd.h>
#include <chrono> 
#include <ctime> 
using namespace std;

int create_socket(int);
int accept_conn(int);

#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

#define MAXLINE 4096 /*max text line length*/
#define LISTENQ 8 /*maximum number of client connections*/

int main (int argc, char **argv)
{
 int listenfd, connfd, n;
 pid_t childpid;
 socklen_t clilen;
 char buf[MAXLINE];
 struct sockaddr_in cliaddr, servaddr;

 if (argc !=2) {						//validating the input
  cerr<<"Usage: ./a.out <port number>"<<endl;
  exit(1);
 }
 

 //Create a socket for the soclet
 //If sockfd<0 there was an error in the creation of the socket
 if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  cerr<<"Problem in creating the socket"<<endl;
  exit(2);
 }


 //preparation of the socket address
memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 if(atoi(argv[1])<=1024){
	cerr<<"Port number must be greater than 1024"<<endl;
	exit(2);
 }
 servaddr.sin_port = htons(atoi(argv[1]));

 //bind the socket
 bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

 //listen to the socket by creating a connection queue, then wait for clients
 listen (listenfd, LISTENQ);

 cout<<"Server running...waiting for connections."<<endl;

 for ( ; ; ) {

  clilen = sizeof(cliaddr);
  //accept a connection
  connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);

  cout<<"Received request..."<<endl;

  if ( (childpid = fork ()) == 0 ) {//if it’s 0, it’s child process

  cout<<"Child created for dealing with client requests"<<endl;

  //close listening socket
  close (listenfd);
  int data_port=1024;						//for data connection
  while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
   cout<<"String received from client: "<<buf;
   char *token,*dummy;
   dummy=buf;
   token=strtok(dummy," ");

   if (strcmp("GOODBYE\n",buf)==0)  {
   	cout<<"The client has quit\n";
   	
        send(connfd,"THANK YOU ALL CONNECTIONS SUCCESSFULLY TERMINATED",MAXLINE,0);
        break;
        
   }

 
   if (strcmp("RETR",token)==0)  {
	char port[MAXLINE],buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE];
	int datasock,datasock1,lSize,num_blks,num_last_blk,i;
	FILE *fp;
	token=strtok(NULL," \n");
	cout<<"Filename given is: "<<token<<endl;
					//accepting connnection by client
	if ((fp=fopen(token,"r"))!=NULL)
	{       data_port=20020;
             
	        sprintf(port,"%d",data_port);
        	datasock1=create_socket(data_port);				//creating socket for data connection
	        send(connfd, port,MAXLINE,0);					//sending port no. to client
	        datasock=accept_conn(datasock1);	 
                close(datasock1);


		//size of file
		send(datasock,"1",MAXLINE,0);
		fseek (fp , 0 , SEEK_END);
		lSize = ftell (fp);
		rewind (fp);
		num_blks = lSize/MAXLINE;
		num_last_blk = lSize%MAXLINE; 
		sprintf(char_num_blks,"%d",num_blks);
		send(datasock, char_num_blks, MAXLINE, 0);
		//cout<<num_blks<<"	"<<num_last_blk<<endl;
                
		for(i= 0; i < num_blks; i++) { 
			fread (buffer,sizeof(char),MAXLINE,fp);
			send(datasock, buffer, MAXLINE, 0);
			//cout<<buffer<<"	"<<i<<endl;
		}
		sprintf(char_num_last_blk,"%d",num_last_blk);
		send(datasock, char_num_last_blk, MAXLINE, 0);
		if (num_last_blk > 0) { 
			fread (buffer,sizeof(char),num_last_blk,fp);
			send(datasock, buffer, MAXLINE, 0);
			//cout<<buffer<<endl;
		}
		fclose(fp);
		cout<<"File upload done.\n";
                close(datasock);
                
		
	}
	else{ data_port=20023;
	fflush(stdin);      
	
	sprintf(port,"%d",data_port);
	datasock1=create_socket(data_port);				//creating socket for data connection
	send(connfd, port,MAXLINE,0);					//sending port no. to client
	datasock=accept_conn(datasock1);	
        close(datasock1);
        fflush(stdin);
           auto timenow = 
      chrono::system_clock::to_time_t(chrono::system_clock::now()); 
  
  //  cout << "<"<<ctime(&timenow)<<">"; 
        char *time=ctime(&timenow);
        //cout<<"sending file error"<<endl;
	char hello[]="FILE NOT FOUND AT CURRENT WORKING DIRECTORY";
      
        send(datasock,hello,MAXLINE,0);
        send(datasock,time,MAXLINE,0);
        fflush(stdin);
                close(datasock);
               
	}
   }

  }

  if (n < 0)
   cout<<"Read error"<<endl;

  exit(0);
 }
 //close socket of the server
 close(connfd);
}
}

int create_socket(int port)
{ int opt=1;
int listenfd;
struct sockaddr_in dataservaddr;


//Create a socket for the soclet
//If sockfd<0 there was an error in the creation of the socket
if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
cerr<<"Problem in creating the data socket"<<endl;
exit(2);
}

if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT,&opt,sizeof(opt)))
   { cerr<<"setsockopt error";
     exit(2);
    }
//preparation of the socket address
memset(&dataservaddr, 0, sizeof(dataservaddr));
dataservaddr.sin_family = AF_INET;
dataservaddr.sin_addr.s_addr = htonl(INADDR_ANY);
dataservaddr.sin_port = htons(port);

if ((bind (listenfd, (struct sockaddr *) &dataservaddr, sizeof(dataservaddr))) <0) {
cerr<<"Problem in binding the data socket"<<endl;
exit(2);
}

 //listen to the socket by creating a connection queue, then wait for clients
 listen (listenfd, 1);

return(listenfd);
}

int accept_conn(int sock)
{
int dataconnfd;
socklen_t dataclilen;
struct sockaddr_in datacliaddr;

dataclilen = sizeof(datacliaddr);
  //accept a connection
if ((dataconnfd = accept (sock, (struct sockaddr *) &datacliaddr, &dataclilen)) <0) {
cerr<<"Problem in accepting the data socket"<<endl;
exit(2);
}

return(dataconnfd);
}
//server 2  
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



/*


#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <unistd.h>
#include<signal.h>
#include <chrono> 
#include <ctime> 
using namespace std;

int create_socket(int);
int accept_conn(int);

#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

#define MAXLINE 4096 /*max text line length*/
#define LISTENQ 8 /*maximum number of client connections*/

int main (int argc, char **argv)
{
 int listenfd, connfd, n;
 pid_t childpid;
 socklen_t clilen;
 char buf[MAXLINE];
 struct sockaddr_in cliaddr, servaddr;

 if (argc !=2) {						//validating the input
  cerr<<"Usage: ./a.out <port number>"<<endl;
  exit(1);
 }
 

 //Create a socket for the soclet
 //If sockfd<0 there was an error in the creation of the socket
 if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  cerr<<"Problem in creating the socket"<<endl;
  exit(2);
 }


 //preparation of the socket address
memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 if(atoi(argv[1])<=1024){
	cerr<<"Port number must be greater than 1024"<<endl;
	exit(2);
 }
 servaddr.sin_port = htons(atoi(argv[1]));

 //bind the socket
 bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

 //listen to the socket by creating a connection queue, then wait for clients
 listen (listenfd, LISTENQ);

 cout<<"Server running...waiting for connections."<<endl;

 for ( ; ; ) {

  clilen = sizeof(cliaddr);
  //accept a connection
  connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);

  cout<<"Received request..."<<endl;

  if ( (childpid = fork ()) == 0 ) {//if it’s 0, it’s child process

  cout<<"Child created for dealing with client requests"<<endl;

  //close listening socket
  close (listenfd);
  int data_port=1024;						//for data connection
  while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
   cout<<"String received from client: "<<buf;
   char *token,*dummy;
   dummy=buf;
   token=strtok(dummy," ");

   if (strcmp("GOODBYE\n",buf)==0)  {
   	cout<<"The client has quit\n";
        send(connfd,"THANK YOU ALL CONNECTIONS SUCCESSFULLY TERMINATED",MAXLINE,0);
        break;
        
   }

 
   if (strcmp("RETR",token)==0)  {
	char port[MAXLINE],buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE];
	int datasock,datasock1,lSize,num_blks,num_last_blk,i;
	FILE *fp;
	token=strtok(NULL," \n");
	cout<<"Filename given is: "<<token<<endl;
					//accepting connnection by client
	if ((fp=fopen(token,"r"))!=NULL)
	{       data_port=20020;
               
	        sprintf(port,"%d",data_port);
        	datasock1=create_socket(data_port);				//creating socket for data connection
	        send(connfd, port,MAXLINE,0);					//sending port no. to client
	        datasock=accept_conn(datasock1);	 
                close(datasock1);


		//size of file
		send(datasock,"1",MAXLINE,0);
		fseek (fp , 0 , SEEK_END);
		lSize = ftell (fp);
		rewind (fp);
		num_blks = lSize/MAXLINE;
		num_last_blk = lSize%MAXLINE; 
		sprintf(char_num_blks,"%d",num_blks);
		send(datasock, char_num_blks, MAXLINE, 0);
		//cout<<num_blks<<"	"<<num_last_blk<<endl;

		for(i= 0; i < num_blks; i++) { 
			fread (buffer,sizeof(char),MAXLINE,fp);
			send(datasock, buffer, MAXLINE, 0);
			//cout<<buffer<<"	"<<i<<endl;
		}
		sprintf(char_num_last_blk,"%d",num_last_blk);
		send(datasock, char_num_last_blk, MAXLINE, 0);
		if (num_last_blk > 0) { 
			fread (buffer,sizeof(char),num_last_blk,fp);
			send(datasock, buffer, MAXLINE, 0);
			//cout<<buffer<<endl;
		}
		fclose(fp);
		cout<<"File upload done.\n";
                close(datasock);
                
		
	}
	else{ data_port=20023;
	fflush(stdin);      
	
	sprintf(port,"%d",data_port);
	datasock1=create_socket(data_port);				//creating socket for data connection
	send(connfd, port,MAXLINE,0);					//sending port no. to client
	datasock=accept_conn(datasock1);	
        close(datasock1);
        fflush(stdin);
           auto timenow = 
      chrono::system_clock::to_time_t(chrono::system_clock::now()); 
  
    cout << "<"<<ctime(&timenow)<<">"; 
        char *time=ctime(&timenow);
        //cout<<"sending file error"<<endl;
	char hello[]="FILE NOT FOUND AT CURRENT WORKING DIRECTORY";
      
        send(datasock,hello,MAXLINE,0);
        send(datasock,time,MAXLINE,0);
        fflush(stdin);
                close(datasock);
               
	}
   }

  }

  if (n < 0)
   cout<<"Read error"<<endl;

  exit(0);
 }
 //close socket of the server
 close(connfd);
}
}

int create_socket(int port)
{ int opt=1;
int listenfd;
struct sockaddr_in dataservaddr;


//Create a socket for the soclet
//If sockfd<0 there was an error in the creation of the socket
if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
cerr<<"Problem in creating the data socket"<<endl;
exit(2);
}

if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT,&opt,sizeof(opt)))
   { cerr<<"setsockopt error";
     exit(2);
    }
//preparation of the socket address
memset(&dataservaddr, 0, sizeof(dataservaddr));
dataservaddr.sin_family = AF_INET;
dataservaddr.sin_addr.s_addr = htonl(INADDR_ANY);
dataservaddr.sin_port = htons(port);

if ((bind (listenfd, (struct sockaddr *) &dataservaddr, sizeof(dataservaddr))) <0) {
cerr<<"Problem in binding the data socket"<<endl;
exit(2);
}

 //listen to the socket by creating a connection queue, then wait for clients
 listen (listenfd, 1);

return(listenfd);
}

int accept_conn(int sock)
{
int dataconnfd;
socklen_t dataclilen;
struct sockaddr_in datacliaddr;

dataclilen = sizeof(datacliaddr);
  //accept a connection
if ((dataconnfd = accept (sock, (struct sockaddr *) &datacliaddr, &dataclilen)) <0) {
cerr<<"Problem in accepting the data socket"<<endl;
exit(2);
}

return(dataconnfd);
}



------------------------------------------




#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <string.h>
#include<signal.h>
#include <arpa/inet.h>
using namespace std;

int create_socket(int,char *);

#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

#define MAXLINE 4096 /*max text line length*/


int main(int argc, char **argv)
{
 int sockfd;
 pid_t childpid;
 struct sockaddr_in servaddr;
 char sendline[MAXLINE], recvline[MAXLINE];

 //basic check of the arguments
 //additional checks can be inserted
 if (argc !=3) {
  cerr<<"Usage: ./a.out <IP address of the server> <port number>"<<endl;
  exit(1);
 }

 //Create a socket for the client
 //If sockfd<0 there was an error in the creation of the socket
 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  cerr<<"Problem in creating the socket"<<endl;
  exit(2);
 }

 //Creation of the socket
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr= inet_addr(argv[1]);
 servaddr.sin_port =  htons(atoi(argv[2])); //convert to big-endian order

 //Connection of the client to the socket
 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  cerr<<"Problem in connecting to the server"<<endl;
  exit(3);
 }

 cout<<"ftp>";

 while (fgets(sendline, MAXLINE, stdin) != NULL) {

  send(sockfd, sendline, MAXLINE, 0);
  char *token,*dummy;
  dummy=sendline;
  token=strtok(dummy," ");
   
if(childpid=fork()==0)
{

   if (strcmp("GOODBYE\n",sendline)==0)  {
        char m1[MAXLINE];
        recv(sockfd, m1, MAXLINE,0);
        cout<<m1;
        kill(0,SIGKILL);
    close(sockfd);
  return(0);
   }


   else if (strcmp("RETR",token)==0)  {
    char port[MAXLINE], buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE],message[MAXLINE];
  int data_port,datasock,lSize,num_blks,num_last_blk,i;
  FILE *fp;
  recv(sockfd, port, MAXLINE,0);
       
  data_port=atoi(port);
  datasock=create_socket(data_port,argv[1]);
  token=strtok(NULL," \n");
  recv(datasock,message,MAXLINE,0);
  if(strcmp("1",message)==0){
    if((fp=fopen(token,"w"))==NULL)
      {cout<<"Error in creating file\n";
                         close(datasock);
                        }
    else
    {
      recv(datasock, char_num_blks, MAXLINE,0);
      num_blks=atoi(char_num_blks);
      for(i= 0; i < num_blks; i++) { 
        recv(datasock, buffer, MAXLINE,0);
        fwrite(buffer,sizeof(char),MAXLINE,fp);
        //cout<<buffer<<endl;
      }
      recv(datasock, char_num_last_blk, MAXLINE,0);
      num_last_blk=atoi(char_num_last_blk);
      if (num_last_blk > 0) { 
        recv(datasock, buffer, MAXLINE,0);
        fwrite(buffer,sizeof(char),num_last_blk,fp);
        //cout<<buffer<<endl;
      }
      fclose(fp);
      cout<<"File download done."<<endl;
                        close(datasock);
                        
    }
  }
  else{    
    fflush(stdin);
    cout<<message<<endl;
    char time[100];
    recv(datasock,time,MAXLINE,0);
    cout<<"<"<<time<<">"<<message;
                close(datasock);
                 kill(0,SIGKILL);
  }
   }
   else{
  cerr<<"Error in command. Check Command"<<endl;
   kill(0,SIGKILL);
   }}

   cout<<"ftp>";

 }

 exit(0);
}


int create_socket(int port,char *addr)
{
 int sockfd;
 struct sockaddr_in servaddr;


 //Create a socket for the client
 //If sockfd<0 there was an error in the creation of the socket
 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  cerr<<"Problem in creating the socket"<<endl;
  exit(2);
 }

 //Creation of the socket
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr= inet_addr(addr);
 servaddr.sin_port =  htons(port); //convert to big-endian order

 //Connection of the client to the socket
 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  cerr<<"Problem in creating data channel"<<endl;
  exit(3);
 }

return(sockfd);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




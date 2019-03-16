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
#include <unistd.h>
#define MAXLINE 20

using namespace std;

int main(int argc, char **argv)
{
		int sockfd,pval;
		char a[2] = {0};
		char b;
		char buff[50] = {0};

		FILE *fp;

		struct sockaddr_in servaddr;


		if( (sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) 
		{
			cerr<<"Problem in creating the socket"<<endl;
			exit(2);
		}

		memset(&servaddr, 0, sizeof(servaddr));
		
		servaddr.sin_family = AF_INET;
		servaddr.sin_port =  htons(80);

		pval=inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr.s_addr);	

		if( connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0)
		{
			cerr<<"Problem in connecting to the server"<<endl;
			exit(3);
		}

		while(1) 
		{
			fgets(buff, MAXLINE, stdin);
			buff[strlen(buff)-2] = ' ';

			send(sockfd, buff, MAXLINE, 0);

			char token[50]={0};
			char token1[50]={0};
			char dummy[50]={0};

			dummy = strdup(buff);
			token = strtok(dummy," ");
			buff = {0};

			if( strcmp(token,"HEAD") == 0 )
			{
				while( (recv(sockfd, a, 1, 0) > 0) &&  (strcmp(a,"$") != 0) ) 
				{
					cout<<a[0];
					a={0};
				}

				cout<<endl;
// TIME //
			}

			else if( strcmp(token,"GET") == 0 )
			{
				while( (recv(sockfd, a, 1, 0) > 0) &&  (strcmp(a,"$") != 0) ) 
				{
					cout<<a[0];
					a = {0};
				}

				cout<<endl;

				recv(sockfd, a, 1 ,0);

				if(a[0]=='y')
				{
					a = {0};

					while( (recv(sockfd, a, 1, 0) > 0) &&  (a[0]!=EOF) ) 
					{
						cout<<a[0];
						a = {0};
					}

					cout<<endl;
				}
			}

			else if( strcmp(token,"DELETE") == 0 )
			{
				while( (recv(sockfd, a, 1, 0) > 0) &&  (strcmp(a,"$") != 0) ) 
				{
					cout<<a[0];
					a={0};
				}
				cout<<endl;
			}

			else if( strcmp(token,"PUT") == 0 )
			{
				token = strtok(dummy," ");
				token1 = strtok(dummy," ");

				fp = fopen(token,"r");

				while( (b=getc(fp)) != EOF )
				{
					a[0]=b;
					send(sockfd, a, 1, 0);
					a={0};
				}

				if(token1!=NULL)
				{
					while( (recv(sockfd, a, 1, 0) > 0) &&  (strcmp(a,"$") != 0) ) 
					{
						cout<<a[0];
						a={0};
					}
				}
			}

			else
			{
				while( (recv(sockfd, a, 1, 0) > 0) &&  (strcmp(a,"$") != 0) ) 
				{
					cout<<a[0];
					a={0};
				}
				cout<<endl;

				close(sockfd);
				exit(0);
			}			
		}

		exit(0);
}
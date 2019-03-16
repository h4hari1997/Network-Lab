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
#include <unistd.h>
#define MAXLINE 20

using namespace std;

int main(int argc, char **argv)
{
 
       int listenfd, connfd, n;
       pid_t childpid;
       socklen_t clilen;
       char buf[MAXLINE];
       FILE *fp;

       char http200[] = "HTTP/1.1 200 OK$";
       char http400[] = "HTTP/1.1 400$";
       char http403[] = "HTTP/1.1 403$";
       char http404[] = "HTTP/1.1 404$";
       char http405[] = "HTTP/1.1 405$";
       char time[] = "//////////////////";
       char a[2]="y";
       char b;

       struct sockaddr_in cliaddr, servaddr;
       
       if( (listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) 
       {
          cerr<<"Problem in creating the socket"<<endl;
          exit(2);
       }

       memset(&servaddr, 0, sizeof(servaddr));

       servaddr.sin_family = AF_INET;
       servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
       servaddr.sin_port = htons(80);

       bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

       listen(listenfd, LISTENQ);

       cout<<"Server running...waiting for connections."<<endl;

       while(1)
       {
          clilen = sizeof(cliaddr);
          connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);
          cout<<"Received request..."<<endl;

          if( (childpid = fork ()) == 0 ) 
          {
                cout<<"Child created for dealing with client requests"<<endl;
                close(listenfd);

                while( (n = recv(connfd, buf, MAXLINE, 0)) > 0)
                {
                      cout<<"String received from client: "<<buf;

                      char token[50]={0};
                      char token1[50]={0};
                      char token2[50]={0};
                      char token3[50]={0};
                      char dummy[50]={0};

                      dummy = strdup(buff);
                      token=strtok(dummy," ");

                      if(strcmp("HEAD",token)==0)  
                      {
                          token=strtok(NULL," ");
                          token1=strtok(NULL," ");
                          
                          cout<<"Filename given is: "<<token<<endl;

                          if(strcmp(token1,"HTTP/1.1")!=0)
                              send(connfd, http400, strlen(http400), 0);
                          else
                          {   
                              if(strcmp(token,"/")!=0)
                              {  
                                  for(int i=0;i<strlen(token)-1;i++)
                                     token[i]=token[i+1]; 

                                  token2=strtok(token,"/");
                                  
                                  while(token2!=NULL)
                                  {
                                      strcpy(token3,token2);
                                      token2=strtok(NULL,"/");
                                  }

                                  if( (token3[0]=='1') && (token3[1]=='1') )
                                      send(connfd, http403, strlen(http403), 0);
                                  else
                                  {
                                      if((fp=fopen(token,"r"))==NULL)
                                          send(connfd, http404, strlen(http404), 0);
                                      else
                                      {
                                          send(connfd, http200, strlen(http200), 0);                                      
                                          send(connfd, time, strlen(time), 0);
                                      }
                                  }
                              }
                              else
                              {
                                  if((fp=fopen("index.html","r"))==NULL)
                                      send(connfd, http404, strlen(http404), 0);
                                  else
                                  {
                                      send(connfd, http200, strlen(http200), 0);
                                      send(connfd, time, strlen(time), 0);
                                  }
                              }
                          } 
                      }

                      else if(strcmp("GET",token)==0)  
                      {
                          token=strtok(NULL," ");
                          token1=strtok(NULL," ");
                          
                          cout<<"Filename given is: "<<token<<endl;

                          if(strcmp(token1,"HTTP/1.1")!=0)
                              send(connfd, http400, strlen(http400), 0);
                          else
                          {
                              if(strcmp(token,"/")!=0)
                              {                                  
                                  for(int i=0;i<strlen(token)-1;i++)
                                     token[i]=token[i+1]; 

                                  token2=strtok(token,"/");
                                  
                                  while(token2!=NULL)
                                  {
                                    strcpy(token3,token2);
                                    token2=strtok(NULL,"/");
                                  }
                          
                                  if( (token3[0]=='1') && (token3[1]=='1') )
                                      send(connfd, http403, strlen(http403), 0);
                                  else
                                  {
                                      if((fp=fopen(token,"r"))==NULL)
                                          send(connfd, http404, strlen(http404), 0);
                                      else
                                      {
                                          send(connfd, http200, strlen(http200), 0);                                        
                                          send(connfd, a, 1, 0);
                                          while( (b=getc(fp)) != EOF )
                                          {
                                            a[0]=b;
                                            send(sockfd, a, 1, 0);
                                            a={0};
                                          }                                       
                                      }
                                  }
                              }
                              else
                              {
                                  if((fp=fopen("index.html","r"))==NULL)
                                      send(connfd, http404, strlen(http404), 0);
                                  else
                                  {
                                      send(connfd, http200, strlen(http200), 0);                                      
                                      send(connfd, a, 1, 0);
                                      while( (b=getc(fp)) != EOF )
                                      {
                                        a[0]=b;
                                        send(sockfd, a, 1, 0);
                                        a={0};
                                      }                                              
                                  }
                              }                                  
                          } 
                      }

                      else if(strcmp("PUT",token)==0)  
                      {
                      }
                      
                      else if(strcmp("DELETE",token)==0)  
                      {
                          token=strtok(NULL," ");
                          token1=strtok(NULL," ");
                          
                          cout<<"Filename given is: "<<token<<endl;

                          if(strcmp(token1,"HTTP/1.1")!=0)
                              send(connfd, http400, strlen(http400), 0);
                          else
                          {
                              if(strcmp(token,"/")!=0)
                              {                                  
                                  for(int i=0;i<strlen(token)-1;i++)
                                     token[i]=token[i+1]; 

                                  token2=strtok(token,"/");
                                  
                                  while(token2!=NULL)
                                  {
                                    strcpy(token3,token2);
                                    token2=strtok(NULL,"/");
                                  }
                          
                                  if( (token3[0]=='1') && (token3[1]=='1') )
                                      send(connfd, http403, strlen(http403), 0);
                                  else
                                  {
                                      if((fp=fopen(token,"r"))==NULL)
                                          send(connfd, http404, strlen(http404), 0);
                                      else
                                      {
                                          send(connfd, http200, strlen(http200), 0);
                                          remove(token);
                                      }
                                  }
                              }
                              else
                              {
                                  if((fp=fopen("index.html","r"))==NULL)
                                      send(connfd, http404, strlen(http404), 0);
                                  else
                                  {
                                      send(connfd, http200, strlen(http200), 0);
                                      remove(token);                                          
                                  }
                              }                                  
                          }            
                      } 
                      
                      else
                      {
                          send(connfd, http405, 20, 0);
                      }
                }

                exit(0); //kill
                close(connfd);

         }
        }
}
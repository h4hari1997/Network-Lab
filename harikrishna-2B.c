#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>

#define PORT 4321


int server()
{
	char buffer[1024]={0};
	char c;
	char number[1024]={0};
	int socket_fd,newsocket_fd,bindval,listval,accval,readval,writeval,servlen;
	
	socket_fd=socket(AF_INET,SOCK_STREAM,0);

	if(socket_fd<0)
	{
		printf("Socket Failed\n");
		return -1;
	}
	
  	struct sockaddr_in addr; 
        bzero(&addr,sizeof(addr));
	servlen = sizeof(addr); 
	 
 	addr.sin_family = AF_INET; 
   	addr.sin_addr.s_addr = INADDR_ANY; 
   	addr.sin_port = htons(PORT); 

	bindval=bind(socket_fd,(struct sockaddr*)&addr,servlen);
	
	
	if(bindval<0)
	{
		printf("Binding Failed\n");
		return -1;
	}
		
	listval=listen(socket_fd,3);
	
	if(listval<0)
	{
		printf("Listening Failed\n");
		return -1;
	}
		
	int i=0;

	while(1)
	{
		int new_socketfd=accept(socket_fd,(struct sockaddr*)&addr,(socklen_t*)&servlen);

		if(new_socketfd<0)
		{
			printf("Accept Failed\n");
			return -1;
		}
		
		FILE *fp1,*fp2;
		
		readval=read(new_socketfd,buffer,sizeof(buffer));

		fp1=fopen("RollS.txt","w+");
		fputs(buffer,fp1);
		fseek(fp1,0,SEEK_SET);
	
		while((c=getc(fp1))!='\n')
		{
			if(isdigit(c))
			{
				number[i]=c;
				i++;
			}
			else
				continue;
		}
		
		send(new_socketfd,number,strlen(number),0);
		fclose(fp1);
	}
	return 0; 	
} 


int client()
{
	int clientfd,conval,readval,writeval,pval,i=0;
	char buffer[1024]={0};
	char number[1024]={0};
	char c;
	
	clientfd=socket(AF_INET,SOCK_STREAM,0);

	if (clientfd < 0) 
	{ 
		printf("\n Socket Failed \n"); 
		return -1; 
	}
	
	struct sockaddr_in serv_addr,client;
	bzero(&serv_addr,sizeof(serv_addr));	

	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(PORT);

	pval=inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr.s_addr);	
	
	if(pval<0)
	{
		printf("Invalid Address/Address not supported");
		return -1;
	}
	
	conval=connect(clientfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	
	if(conval<0)
	{
		printf("Connection Failed\n");	
		return -1;
	}

	FILE *fp1,*fp2;
	fp1=fopen("RollC.txt","r");
	
	while((c=getc(fp1))!=EOF)
	{
		buffer[i]=c;
		i++;
	}

	write(clientfd,buffer,strlen(buffer));
	fclose(fp1);
	
	readval=read(clientfd,number,1024);

	fp2=fopen("NumberC.txt","w");
	fputs(number,fp2);	
	fclose(fp2);
	
	close(clientfd);
	return 0; 
}




int main(int argc,char **argv)
{
	if(strcmp(argv[1],"server")==0)
		server();
	else if(strcmp(argv[1],"client")==0)
		client();
	return 0;
}



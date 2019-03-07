#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 1234

int main() 
{ 
    	char buffer[1024] = {0}; 
    	char *message1 = "This number belongs to NITC S6/S8 batch"; 
    	char *message2="OK";
       
    	int listen_fd = socket(AF_INET,SOCK_STREAM,0);

   	/*if (listen_fd == 0) 
    	{ 
        	perror("socket failed"); 
        	exit(EXIT_FAILURE); 
    	}*/

  	 struct sockaddr_in address; 
	 int addrlen = sizeof(address); 
 	 address.sin_family = AF_INET; 
   	 address.sin_addr.s_addr = INADDR_ANY; 
   	 address.sin_port = htons(PORT); 
       
   	int val_bind=bind(listen_fd, (struct sockaddr *)&address,sizeof(address));

    	/*if (val_bind<0) 
    	{ 
        	perror("bind failed"); 
        	exit(EXIT_FAILURE); 
    	}*/ 

	int l=listen(listen_fd,3);
    	
	/*if (l< 0) 
    	{ 
        	perror("listen"); 
        	exit(EXIT_FAILURE); 
    	}*/
 
	while(1)
	{
    	int conn_fd = accept(listen_fd,(struct sockaddr *)&address,(socklen_t*)&addrlen);

    	/*if (conn_fd<0) 
    	{ 
        	perror("accept"); 
        	exit(EXIT_FAILURE); 
    	}*/

	while(1)
	{	
		int valread;
    		valread = read(conn_fd,buffer,1024);
		
			if(valread<=0)
			{
			close(conn_fd);
			break;
			}

 
  	
		if(!strcmp(buffer,"Hello:B160282CS"))
    		{
			send(conn_fd , message1 , strlen(message1) , 0 ); 
		} 
		else	
    			send(conn_fd , message2 , strlen(message2) , 0 ); 		    
	}
	}
	return 0; 	
} 

----------------------------

#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 

int main() 
{ 
	char buf[1024] = {0}; 
	
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);

	/*if (client_socket < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	}*/

	struct sockaddr_in serv_addr; 
	bzero(&serv_addr,sizeof(serv_addr));

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	

	int val_inet=inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

	/*if(val_inet<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	}*/

	int val_connect=connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	
	/*if (val_connect<0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	}*/
	
	char *message1;
	int val_read;
       	do
	{
		printf("\nEnter the message:");
		scanf("%s",message1);
		send(client_socket,message1,strlen(message1),0); 
		val_read=read(client_socket,buf,1024); 
		printf("%s\n",buf);
	}while(strcmp(buf,"This number belongs to NITC S6/S8 batch"));
	close(client_socket);
	return 0; 
} 


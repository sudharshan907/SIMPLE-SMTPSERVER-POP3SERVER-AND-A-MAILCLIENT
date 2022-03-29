#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<time.h>
#include<ctype.h>

#define IP_ADDR "127.0.0.1"

struct Login{
	char uname[20];
	char pwd[20];
};

struct Login cred[3];

void manage_mail(int sockID)
{
	char msg[256], mail[1024], user[20];
	int bytes = recv(sockID, user, sizeof(user), 0);

	strcat(user, "/mymailbox.mail");
	FILE *fp;
	fp = fopen(user, "r");
	char list[1024];
	bzero(list, sizeof(list));
	int i=1;char num[1];

	while(fgets(msg, sizeof(msg), fp)!=NULL)
	{
		if(strncmp(msg, "From:", 5)==0)
		{

		  num[0]='0'+i;
	    strcat(list,num);
	    strcat(list,".");
	    i++;

			char *token1, *token2;
			const char s[2] = " ";
			token1 = strtok(msg, s);
			token2 = strtok(NULL, s);
			strcat(list, token2);
		}
		else if(strncmp(msg, "Received:", 9)==0)
		{
			strcat(list, msg);
			strcat(list, "\n");
		}
		else if(strncmp(msg,"Subject:",8)==0){
		 strcat(list,msg);
		// strcat(list,"\n");
		}

	}
	//printf("list=%ld\n",strlen(list));
	//printf("%s",list);
	fflush(stdout);
	send(sockID, list, strlen(list), 0);   //this is preview of all mails of connected user.
	fclose(fp);
//****************************************************************************************************
	FILE *fp2;
	fp2 = fopen(user, "r");  //this will not give seg fault bcz user is already verified in authentication at first.
	char list1[1024];        //might give seg fault if that file is deleted manually after authentication.
	bzero(list1, sizeof(list1));
	char sender[20];
	//int attempts=1;
	//while(attempts>=0)

	int g;
		while((g = recv(sockID, sender, sizeof(sender), 0))<0);
	       sender[g]='\0';
	       fflush(stdout);
		//g=recv(sockID,sender,sizeof(sender),0);
		//  printf("received=(%d)\n%s",g,sender);
		int flag1=0;
		//printf("detect\n");
		for(int i=0; i<3; i++)
		{
			//sender[0] = toupper(sender[0]);
			if(strcmp(sender, cred[i].uname)==0)
			{
				flag1=1;
				break;
			}
		}
		//sender[0] = tolower(sender[0]);
		//printf("detect1\n");
		if(flag1==0)
		{
			//attempts -= 1;
			sprintf(msg, "INVALID MAIL-ID\n");
			send(sockID, msg, sizeof(msg), 0);
			//continue;
			return;
		}

	//printf("detect3\n");
	/*
	flow will reach here in 2 cases.1)recvd user is valid(present in list) or 2)2 invalid receives in client side after 2 invalids it is going to 3 options menu,so here also directing to 3 options menu by returning or stopping this fn.
	*/

       //flow will reach this line only if user is valid(no mails case or some mails case)
       fflush(stdout);
       	int found=0;
	      fflush(stdout);
           	//printf("detect4\n");

	//printf("now flow is here1");
	 fflush(stdout);
	while(fgets(msg, sizeof(msg), fp2)!=NULL)
	{
		if(strncmp(msg, "From:", 5)==0)
		{
			char *token1, *token2, *token3;
			const char s[2] = "@";
			token1 = strtok(msg, s);  //token1="From: <username>"
			const char s1[2] = " ";
			token2 = strtok(token1, s1);  //token2="From:"
			token3 = strtok(NULL, s1);    //token3=<username>
			if(strcmp(sender, token3)==0)
			{
				found=1;
			}
		}
	}
	fflush(stdout);
	if(found==0)
	{
		sprintf(msg, "NO MAILS TO SHOW\n");
		send(sockID, msg, sizeof(msg), 0);
		return;
	}
	else{
	sprintf(msg, "mails present");
		send(sockID, msg, sizeof(msg), 0);

	}
	fflush(stdout);
	//printf("now flow is here");
	fflush(stdout);  //if this is not there this print work is waiting in buffer,why i dont know
	FILE *fp3;
	fp3 = fopen(user, "r");
	//char list[1024];
	bzero(list, sizeof(list));
	bzero(msg,sizeof(msg));
	//sender[] k
	char kback[256]; char number[1];
	i=1;//for numbering
	while(fgets(msg, sizeof(msg), fp3)!=NULL)
	{
	bzero(kback,sizeof(kback));
	strcpy(kback,msg);
		if(strncmp(msg, "From:", 5)==0)
		{

		char *token1, *token2, *token3;
			const char s[2] = "@";
			token1 = strtok(msg, s);  //token1="From: <username>"
			const char s1[2] = " ";
			token2 = strtok(token1, s1);  //token2="From:"
			token3 = strtok(NULL, s1);    //token3=<username>
			if(strcmp(sender, token3)==0)
			{
			/*
				  number[0]='0'+i;
	                        strcat(list,number);
	                        strcat(list,".");
	                        i++;
			*/
				strcat(list,kback);
				while(1){
				bzero(msg,sizeof(msg));
				fgets(msg,sizeof(msg),fp3);
				if(strcmp(msg,".\n")==0)
				break;
				strcat(list,msg);
				}
				strcat(list,"\n");
			}
		}

	}
	//printf("list=%ld\n",strlen(list));
	//printf("%s",list);
	send(sockID, list, strlen(list), 0);   //these are all mails of connected user.
	fclose(fp3);
//          printf("waiting to receive one of 3 options\n");
}

int main()
{

	int PORT;
	printf("Enter port number: ");
	scanf("%d", &PORT);

	// create a server socket
	int server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if(server_socket < 0)
	{
		printf("Error in connection\n");
		exit(1);
	}
	printf("Server socket created\n");

	//define server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = inet_addr(IP_ADDR);

	//bind the socket to specified IP and port
	int bind_status = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	if(bind_status < 0)
	{
		printf("Error in binding\n");
		exit(1);
	}
	printf("Binded to port %d\n", PORT);

	//listen to connections
	int listen_status = listen(server_socket, 5);
	if(listen_status==0)
		printf("Listening\n");
	else
	{
		printf("Error in binding\n");
		exit(1);
	}
	//printf("***********************\n");

	char buf[60];
	int i=0;
	FILE *fp;
	fp = fopen("userlogincred.txt", "r");
	if(fp==NULL){
	printf("file open error");
	return 0 ;
	}
	while(1)
	{
		char buf[60];
		if(fgets(buf, 60, fp) == NULL)
			break;
		const char s[2] = " ";
		char *token1, *token2;

		token1 = strtok(buf,s);
		token2 = strtok(NULL, s);

		strcpy(cred[i].uname, token1);
		strcpy(cred[i].pwd, token2);
		i++;
	}

	//accept connections
	while(1)
	{
		int socketID = accept(server_socket, NULL, NULL);
                printf("[+]Client connected\n");
		char str[50];
		sprintf(str, "Connection is set up\n");
		send(socketID, str, sizeof(str), 0);

		while(1)
		{
			char uname[20];
			char pwd[20];
			char msg[50];

			int bytes = recv(socketID, uname, sizeof(uname), 0);
			uname[bytes] = '\0';

			int i=0;
			int flag=0;
			while(i<3)
			{
				if(strcmp(uname, cred[i].uname)==0)
				{
					flag=1;
					break;
				}
				i++;
			}

			if(flag==0)
			{
				//bzero(msg, sizeof(msg));
				sprintf(msg, "Incorrect Username\n");
				send(socketID, msg, sizeof(msg), 0);
				continue;
			}

			//bzero(msg, sizeof(msg));
			strcpy(msg, "Correct Username; Enter password: ");
			fflush(stdout);
			send(socketID, msg, sizeof(msg), 0);
			bytes = recv(socketID, pwd, sizeof(pwd), 0);
			pwd[bytes] = '\0';
			if(strcmp(pwd, cred[i].pwd)==-10)  //note that we are only checking cred[i],specific user passwd.
			{
				bzero(msg, sizeof(msg));
				sprintf(msg, "Authentication successful\n");
				send(socketID, msg, sizeof(msg), 0);
				break;
			}
			else
			{
				bzero(msg, sizeof(msg));
				sprintf(msg, "Incorrect Password\n");
				send(socketID, msg, sizeof(msg), 0);
				continue;
			}
		}
		char command[20];
		while(1)
		{
			recv(socketID, command, sizeof(command), 0);
			if(strcmp(command, "quit")==0)
				break;
			manage_mail(socketID);
		}
		 printf("[-]Client disconnected\n");
		close(socketID);
	}
}

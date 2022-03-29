#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<ctype.h>

#define IP_ADDR "127.0.0.1"
char user[20];
int aliveflag=1; //to stop client after q is entered in manage_mail option.
int managefnflag=1;

void manage_mail(int sockID)
{
printf("\n");
	send(sockID, user, strlen(user), 0);
	char list[1024];
	//  int bytes = recv(sockID, list, strlen(list), 0);
	int bytes;    //for recv() of large strings single recv is not working dont know why.so used a while loop as below.
		while((bytes=recv(sockID,list, sizeof(list), 0))<0);
		list[bytes] = '\0';
		fflush(stdout);   //if ignored this line,recv is not working properly in these cases of while() receiving.

	//  printf("list=%ld\n",strlen(list));
	printf("%s", list);
//***************************************************************************************************
	int a,b;
		char msg[256], sender[20];
		fflush(stdout);
		printf("Enter sender Mail ID\n");
		//scanf("%s", sender);
		//fgets(sender,sizeof(sender),stdin);
		scanf("%[^\n]%*c",sender);
		//printf("scanned=%s",sender);
		a=send(sockID, sender, strlen(sender), 0);
		//sender[a]='\0';
		//printf("sent=(%d)\n%s",a,sender);
		fflush(stdout);   //dont remove this,some unusual behavior is happening
		//sleep(1);
		//printf("detect\n");
		b=recv(sockID, msg, sizeof(msg), 0);
		msg[b]='\0';
		//printf("detect1\n");
		//printf("received=(%d)\n%s",b,msg);
		printf("\n%s",msg);
		if(strcmp(msg, "INVALID MAIL-ID\n")==0)
		{
			//printf("%s\n", msg);
			return ;
		}
		else if(strcmp(msg, "NO MAILS TO SHOW\n")==0)
		{
			printf("no mails to show from this sender as of now\n");;
			return ;
		}
		else if(strcmp(msg,"mails present")==0)
		{
			char list1[1024];
			bytes = recv(sockID, list1, sizeof(list1), 0);
			//printf("here1");
			printf("\n%s\n", list1);
			fflush(stdout);
			char c,dummy;
			//printf("here");
			char com[50];
			while(1){
			printf("\nenter single character:");
			c=getchar();
			dummy=getchar();
			if(c=='q'){
			//actually we should return to options menu again and based on option we should connect again to p.c or
			//s.c server ,but i didnot make connections dynamic so just terminating entire process here.
	  printf("server:goodbye\n");
	  bzero(com,sizeof(com));
	  strcpy(com,"quit");
			send(sockID,com,sizeof(com), 0);
			aliveflag=0;
			//managefnflag=0;
			break;
			}
			else{   //other than 'q'
			//fflush(stdout);
			printf("\n%s", list1);
			continue;
			}
			}
		}


}
int check_line_format(char *line)//should check whether the line contains <some char's(>=1)@<some char's(>=1)> or not.
{                                //did not understand the logic in this fn code,my own logic is,check @ is present or not
	const char s[2] = " ";       //if not there,no.if there,see index if in==0 || in==n-1(last char) no.else yes.
	char *token;
	token = strtok(line, s);  //token="From:" or "To:"
	token = strtok(NULL, s);  //token="<username>@localhost" i guess not sure abt this.
	int n = strlen(token), l=0, r=0, flag=0;
    	for(int i=0;i<n;i++)
	{
        	if(token[i]=='@')
		{
            		flag = 1;
            		continue;
        	}
        	if(flag==0)
           		l++;
        	else
            		r++;
        	if (r>= 1)
            		break;
    	}
    	return l>0 && r>0;
}

int check_mail_format(char email[1024]) //just checks whether from,to,subject -3 lines are there in exact order
{                                       //and for from:,to: lines checks format X@Y or not(calling check_line_format())
	char mail[1024];                    //even if message body of mail is empty this fn will return crct format(1).
	strcpy(mail, email);                //as asked in pdf.
	int flag=0;
	const char s[3] = "\n";
	char *token;
	char *mail1 = mail;
	token = strtok_r(mail1, s, &mail1);
	while(token != NULL)
	{
		char token1[256];
		strcpy(token1, token);
		switch(flag)  //other cases of flag>2 will not occur bcz after 3 checks,function will be returned directly.
		{
			case 0: if((strncmp(token1, "From:", 5)==0) && check_line_format(token1))
					flag=1;
				else
					return 0;
				break;

			case 1: if((strncmp(token1, "To:", 3)==0) && check_line_format(token1))
					flag=2;
				else
					return 0;
				break;

			case 2: if(strncmp(token1, "Subject:", 8)==0) //didnot call lineformat here bcz in ass pdf they mentioned only
					return 1;                             //for from: and to: lines
				break;
		}
		token = strtok_r(NULL, s, &mail1);
	}
}

int check_sender(char msg[256])   //just checks whether entered username is same as his username or not that's it.
{                                 //in first line of sending mail format.
	char line[256];
	strcpy(line, msg);

	const char s1[2] = "@";
	char *token1,*token2,*token3;
	token1 = strtok(line, s1);
	const char s2[2] = " ";
	token2 = strtok(token1, s2);
	token3 = strtok(NULL, s2);
	if(strcmp(user, token3)!=0)
		return 0;
	else
		return 1;
}


void send_mail(int sockID)
{
	while(1)  //this loop is only for facilitating reattempt of typing correct format again for user
	{         //funny thing is he cant escape from this loop until he types the correct format and sends mail to someone.
		char mail[1024];
		char msg[256];
		bzero(mail, sizeof(mail));
		bzero(msg, sizeof(msg));
		printf("Enter mail body\n");
		while(1)
		{
			scanf("%[^\n]%*c",msg);
			strcat(mail, msg);
			strcat(mail, "\n");
			if(strncmp(msg, "From:", 5)==0)   //note that there should be space between From: and
			{                                 //<username> ,same with To: and Subject:
				if(check_sender(msg)!=1)
				{

	       printf("you are not allowed to send a mail to someone(valid users) with random name\n");
	       printf("please change name in the 'From: ..' line to your actual username\n");
					break;
				}
			}

			if(strcmp(msg, ".")==0)   //dont think that any full stop can break this scanning loop,
				break;                //only full stop in entire line itself can stop this scanning.
		}
		if(!check_mail_format(mail))
		{
			printf("Incorrect format\n");
			continue;    //actually here we should again go to 3 options menu by returning the fn here.
		}                    //but i am simply going to enter mail body again step(if returned here,
		else                 //seg fault is coming in server,so i avoided it.
		{
			send(sockID, mail, sizeof(mail), 0);
			break;
		}
	}
	printf("Mail sent successfully\n");
}

int open_connection(int PORT)
{
        //create a socket
        int client_socket = socket(PF_INET, SOCK_STREAM, 0);
        if(client_socket < 0)
        {
                printf("Error in connection\n");
                exit(1);
        }

        //specify an address for socket
        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);
        server_address.sin_addr.s_addr = inet_addr(IP_ADDR);

        //Establish a connection using connect()
        int connect_status = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
        if(connect_status < 0)
        {
                printf("Connection failed\n");
                exit(1);
        }
        char str[50];
        int bytes = recv(client_socket, str, sizeof(str), 0);
        str[bytes] = '\0';
        printf("%s", str);
        int sockID = client_socket;
	return sockID;
}

int main()
{

	/*
	int PORT;
	printf("Enter port number: ");
	scanf("%d", &PORT);

	//create a socket
	int client_socket = socket(PF_INET, SOCK_STREAM, 0);
	if(client_socket < 0)
	{
		printf("Error in connection\n");
		exit(1);
	}

	//specify an address for socket
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = inet_addr(IP_ADDR);

	//Establish a connection using connect()
	int connect_status = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	if(connect_status < 0)
	{
		printf("Connection failed\n");
		exit(1);
	}
	char str[50];
	int bytes = recv(client_socket, str, sizeof(str), 0);
	str[bytes] = '\0';
	printf("%s", str);
	int sockID = client_socket;
	*/
	int PORT;
	printf("Enter port number: ");
	scanf("%d", &PORT);
	int sockID = open_connection(PORT); // pop3 port

	while(1)
	{
		char uname[20];
		char pwd[20];
		char msg[50];

		printf("Enter Username: ");
		scanf("%s", uname);
		send(sockID, uname, sizeof(uname), 0);
		int bytes;
		while((bytes=recv(sockID, msg, sizeof(msg), 0))<0);
		msg[bytes] = '\0';
		printf("%s", msg);
		fflush(stdout);

		if(strcmp(msg, "Incorrect Username\n")==0)
		{
			printf("Retry\n");
			continue;
		}
		strcpy(user, uname);
		user[0] = tolower(user[0]);
		scanf("%s", pwd);
		send(sockID, pwd, sizeof(pwd), 0);
		bytes = recv(sockID, msg, sizeof(msg), 0);
		msg[bytes] = '\0';
		printf("%s",msg);

		if(strcmp(msg, "Incorrect Password\n")==0)
		{
			printf("Retry\n");
			continue;
		}
		else
			break;
	}
	//close(sockID);
	//printf("Connection to POP3 server is closed\n");

	char command[20];
	//char c;
	while(1)
	{

		printf("1.send Mail\n2.manage Mail\n3.quit\n");
		printf("Enter command\n");
	        scanf(" %[^\n]%*c", command);
		int flag=0;
		if(strcmp(command, "quit")==0)
		{
		printf("server:goodbye\n");
			send(sockID,command,sizeof(command), 0);
			break;
		}
		else if(strcmp(command, "send mail")==0)
		{
			//int sockID = open_connection(7001); //smtp server
			send(sockID, command, sizeof(command), 0);
			send_mail(sockID);
			//close(sockID); //connection closed
		}
		else if(strcmp(command, "manage mail")==0)
		{
			//int sockID = open_connection(7002); //pop3 server
			send(sockID, command, sizeof(command), 0);
			manage_mail(sockID);
			if(aliveflag==0)
			break;
			//close(sockID);
		}
		else
			printf("Invalid command\n");
	}
	close(sockID);
}


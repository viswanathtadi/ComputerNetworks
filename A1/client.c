#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#define MAX 250
#define PORT 40000
#define SA struct sockaddr
int checkend(char* s)
{
	int n=strlen(s);
	for(int i=0;i<n-2;i++)
	{
		if(s[i]=='x' && s[i+1]=='x' && s[i+2]=='x')
		return 1;
	}
	return 0;
}

void currentUser(int sockfd)
{
	char cmd[MAX];
	char in[MAX];
	char out[MAX];
	bzero(in,sizeof(in));
	read(sockfd,in,sizeof(in));
	printf("%s\n",in);
	int i,j;
	while(1)
	{
		printf("current>");
		bzero(cmd,sizeof(cmd));
		fgets(cmd,MAX,stdin);
		cmd[strlen(cmd)-1]='\0';
		if(cmd[0]=='R')
		{
			bzero(out,sizeof(out));
			strcpy(out,"READM");
			write(sockfd,out,sizeof(out));
			bzero(in,sizeof(in));
			read(sockfd,in,sizeof(in));
			printf("%s",in);
		}
		else if(cmd[0]=='D' && cmd[1]=='e')
		{
			bzero(out,sizeof(out));
			strcpy(out,"DELM");
			write(sockfd,out,sizeof(out));
			bzero(in,sizeof(in));
			read(sockfd,in,sizeof(in));
			printf("%s",in);
		}
		else if(cmd[0]=='S')
		{
			bzero(out,sizeof(out));
			strcpy(out,cmd);
			out[1]='E';out[2]='N';out[3]='D';out[4]=' ';
			out[strlen(out)]=' ';
			do
			{
				bzero(cmd,sizeof(cmd));
				printf("Type Message>");
				fgets(cmd,MAX,stdin);
				cmd[strlen(cmd)-1]='\t';
				strcat(out,cmd);
			}while(checkend(cmd)==0);
			write(sockfd,out,sizeof(out));
			bzero(in,sizeof(in));
			read(sockfd,in,sizeof(in));
			printf("%s",in);
		}
		else
		{
			bzero(out,sizeof(out));
			strcpy(out,"DONEU");
			write(sockfd,out,sizeof(out));
			bzero(in,sizeof(in));
			read(sockfd,in,sizeof(in));
			printf("%s",in);
			break;
		}
		printf("\n");
	}
	return;
}
int main()
{
	int sockfd,connfd;
	struct sockaddr_in servaddr,cli;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd==-1)
	{
		printf("socket creation failed\n");
		exit(0);
	}
	printf("socket created successfully\n");
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);
	if(connect(sockfd,(SA*)&servaddr,sizeof(servaddr))!=0)
	{
		printf("connection with server failed\n");
		exit(0);
	}
	printf("connected to server\n");
	char cmd[MAX];
	char in[MAX];
	char out[MAX];
	int i,j;
	while(1)
	{	
		printf("general>");
		bzero(cmd,sizeof(cmd));
		fgets(cmd,MAX,stdin);
		cmd[strlen(cmd)-1]='\0';
		if(cmd[0]=='L')
		{
			//Listusers
			bzero(out,sizeof(out));
			strcpy(out,"LSTU");
			write(sockfd,out,sizeof(out));
			bzero(in,sizeof(in));
			read(sockfd,in,sizeof(in));
			printf("%s",in);
		}
		else if(cmd[0]=='A')
		{
			//Adduser
			bzero(out,sizeof(out));
			strcpy(out,"ADDU");
			i=4;j=7;
			while(j<strlen(cmd))
			{
				out[i]=cmd[j];
				i++;j++;
			}
			write(sockfd,out,sizeof(out));
			bzero(in,sizeof(in));
			read(sockfd,in,sizeof(in));
			printf("%s",in);
		}
		else if(cmd[0]=='S')
		{
			//Setuser
			bzero(out,sizeof(out));
			strcpy(out,"USER");
			i=4;j=7;
			while(j<strlen(cmd))
			{
				out[i]=cmd[j];
				i++;j++;
			}
			write(sockfd,out,sizeof(out));
			bzero(in,sizeof(in));
			read(sockfd,in,sizeof(in));
			printf("%s\n",in);
			if(in[0]=='S')
			currentUser(sockfd);
		}
		else
		{
			//Quit
			bzero(out,sizeof(out));
			strcpy(out,"QUIT");
			write(sockfd,out,sizeof(out));
			bzero(in,sizeof(in));
			read(sockfd,in,sizeof(in));
			printf("%s",in);
			break;
		}
		printf("\n");
	}
	close(sockfd);
	return 0;
}

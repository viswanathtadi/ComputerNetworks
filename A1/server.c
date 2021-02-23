#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <time.h>
#define MAX 250 
#define PORT 40000 
#define SA struct sockaddr 

void substring(char s[], char sub[], int p, int l) {
   int c = 0;
   
   while (c < l) {
      sub[c] = s[p+c];
      c++;
   }
   sub[c] = '\0';
}

void lstu(int sockfd)
{
	char out[MAX];
	char buf[MAX];
	bzero(out,MAX);
	bzero(buf,MAX);
	FILE *fp = fopen("users.txt","r");
	while(fgets(buf,MAX,fp)!=NULL)
	{
		if(buf[strlen(buf)-1]=='\n')buf[strlen(buf)-1]='\0';
		buf[strlen(buf)]=' ';
		strcat(out,buf);
		bzero(buf,MAX);
	}
	write(sockfd,out,sizeof(out));
	return;
}
void addu(int sockfd,char* in)
{
	char userfile[MAX];
	char user[MAX];
	char out[MAX];
	bzero(user,MAX);
	bzero(out,MAX);
	substring(in,user,5,strlen(in)-5);
	strcpy(userfile,user);
	strcat(userfile,".txt");
	FILE *fp = fopen(user,"r");
	if(fp==NULL)
	{
		strcpy(out,"New user created");
		write(sockfd,out,sizeof(out));
		fp = fopen(userfile,"w");
		fclose(fp);
		fp = fopen("users.txt","a");
		fputs(user,fp);
		fputs("\n",fp);
		fclose(fp);
	}
	else
	{
		strcpy(out,"User already exists");
		write(sockfd,out,sizeof(out));
	}
	return;
}
void userf(int sockfd,char* user,char* userfile)
{
	char lines[MAX][MAX];
	for(int i=0;i<MAX;i++)bzero(lines[i],MAX);
	int i=0;
	FILE *fp=fopen(userfile,"r");
	while(fgets(lines[i],MAX,fp)!=NULL)i++;
	fclose(fp);
	char in[MAX];
	char out[MAX];
	char buf[MAX];
	int nmes=i;
	bzero(out,MAX);
	strcpy(out,"User has xy messages in his/her inbox");
	out[9]=(char)(nmes/10 + 48);
	out[10]=(char)(nmes%10 + 48);
	write(sockfd,out,sizeof(out));
	i=0;
	for(;;)
	{
		bzero(in,MAX);
		read(sockfd,in,sizeof(in));
		if(in[0]=='R')
		{
			bzero(out,MAX);
			if(nmes==0)
			{
				strcpy(out,"Empty inbox");
				write(sockfd,out,sizeof(out));
			}
			else
			{
				while(1)
				{
					if(lines[i][0]!='\0')break;
					i++;i=i%MAX;
				}
				strcpy(out,lines[i]);
				out[strlen(out)-1]='\0';out[strlen(out)-2]='\0';
				for(int i=0;i<MAX;i++)if(out[i]=='\t')out[i]='\n';
				for(int i=MAX-1;i>=0;i--)if(out[i]=='\n'){out[i]='\0';break;}
				write(sockfd,out,sizeof(out));
				i++;i=i%MAX;
			}
		}
		else if(in[0]=='D' && in[1]=='E')
		{
			bzero(out,MAX);
			if(nmes==0)
			{
				strcpy(out,"Empty inbox");
				write(sockfd,out,sizeof(out));
			}
			else
			{
				while(1)
				{
					if(lines[i][0]!='\0')break;
					i++;i=i%MAX;
				}
				bzero(lines[i],MAX);
				strcpy(out,"Message deleted");
				write(sockfd,out,sizeof(out));
				i++;i=i%MAX;nmes--;
			}
		}
		else if(in[0]=='S')
		{
			char receiver[MAX];
			bzero(receiver,MAX);
			int j;
			for(j=5;j<MAX;j++)
			{
				if(in[j]==' ')break;
			}
			substring(in,receiver,5,j-5);
			char rfile[MAX];
			bzero(rfile,MAX);
			strcpy(rfile,receiver);
			strcat(rfile,".txt");
			
			char buf[MAX];
			char line[MAX];
			bzero(buf,MAX);
			
			bzero(line,MAX);
			strcpy(line,"From: ");
			strcat(line,user);
			line[strlen(line)]='\t';
			strcat(buf,line);
			
			bzero(line,MAX);
			strcpy(line,"To: ");
			strcat(line,receiver);
			line[strlen(line)]='\t';
			strcat(buf,line);
			
			time_t t;
			time(&t);
			bzero(line,MAX);
			strcpy(line,ctime(&t));
			line[strlen(line)-5]='I';line[strlen(line)-4]='S';
			line[strlen(line)-3]='T';line[strlen(line)-2]=' ';
			line[strlen(line)-1]='\0';
			strcat(line,"2021\t");
			strcat(buf,line);
			
			bzero(line,MAX);
			strcpy(line,"Subject: ");
			char mess[MAX];
			bzero(mess,MAX);
			substring(in,mess,j+1,strlen(in)-j-1);
			strcat(line,mess);
			strcat(buf,line);
			
			
			FILE* rfp = fopen(rfile,"r");
			bzero(out,MAX);
			if(rfp!=NULL)
			{
				fclose(rfp);
				rfp = fopen(rfile,"a");
				fputs(buf,rfp);
				fputs("\n",rfp);
				fclose(rfp);
				strcpy(out,"Message sent");
				write(sockfd,out,sizeof(out));
			}
			else
			{
				strcpy(out,"Invalid user");
				write(sockfd,out,sizeof(out));
			}
		}
		else
		{
			bzero(out,MAX);
			strcpy(out,"User session ended");
			write(sockfd,out,sizeof(out));
			break;
		}
	}
	fp=fopen(userfile,"w");
	for(int i=0;i<MAX;i++)
	{
		if(lines[i][0]!='\0')
			fputs(lines[i],fp);
	}
	return;
}
void func(int sockfd)
{
	char in[MAX];
	char out[MAX];
	char buf[MAX];
	bzero(out,MAX);
	strcpy(out,"HELLO");
	for(;;)
	{
		bzero(in,MAX);
		read(sockfd,in,sizeof(in));
		if(strcmp(in,"LSTU")==0)
		{
			lstu(sockfd);
		}
		else if(in[0]=='A' && in[1]=='D' && in[2]=='D' && in[3]=='U')
		{
			addu(sockfd,in);
		}
		else if(in[0]=='U' && in[1]=='S' && in[2]=='E' && in[3]=='R')
		{
			char userfile[MAX];
			char user[MAX];
			char out[MAX];
			bzero(user,MAX);
			bzero(out,MAX);
			substring(in,user,5,strlen(in)-5);
			strcpy(userfile,user);
			strcat(userfile,".txt");
			FILE *fp = fopen(userfile,"r");
			if(fp==NULL)
			{
				strcpy(out,"User does not exist");
				write(sockfd,out,sizeof(out));	
			}
			else
			{
				strcpy(out,"Selected user ");
				strcat(out,user);
				write(sockfd,out,sizeof(out));	
				userf(sockfd,user,userfile);
			}
		}
		else if(in[0]=='Q' && in[1]=='U' && in[2]=='I' && in[3]=='T')
		{
			bzero(out,MAX);
			strcpy(out,"Disconnected");
			write(sockfd,out,sizeof(out));
			return;
		}
		else
		{
			bzero(out,MAX);
			strcpy(out,"Invalid command");
			write(sockfd,out,sizeof(out));
		}
	}
	return;
}

// Driver function 
int main() 
{ 
	FILE *fp = fopen("users.txt","r");
	if(fp==NULL)
	{
		fp=fopen("users.txt","w");
		fclose(fp);
	}
	int sockfd, connfd, len; 
	struct sockaddr_in servaddr, cli; 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n"); 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 
	len = sizeof(cli); 
	
	// Accept the data packet from client and verification 
	connfd = accept(sockfd, (SA*)&cli, &len); 
	if (connfd < 0) { 
		printf("server acccept failed...\n"); 
		exit(0); 
	} 
	else
		printf("server acccept the client...\n"); 
	// Function for chatting between client and server 
	func(connfd); 

	// After chatting close the socket 
	close(sockfd); 
} 


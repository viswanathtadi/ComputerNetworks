#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define ll long long
using namespace std;
using std::cout; using std::endl;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;
std::chrono::high_resolution_clock::time_point start;
int sockfd;
struct sockaddr_in servaddr, cliaddr;
const int MAXLINE = 10000;
const  int RPORT = 11020;
unsigned int slen;
int N=8;

string decToBinary(int n)
{
    string ans="";
    for (int i = N-1; i >= 0; i--) {
        int k = n >> i;
        if (k & 1)
            ans += "1";
        else
            ans += "0";
    }
    return ans;
}
int binaryToDec(string n)
{
    string num = n;
    int dec_value = 0;
    int base = 1;
    int len = num.length();
    for (int i = len - 1; i >= 0; i--) {
        if (num[i] == '1')
            dec_value += base;
        base = base * 2;
    }
    return dec_value;
}

string myRecv()
{
    char buffer[MAXLINE] = {'\0'};
    int n;
	slen = sizeof(servaddr);
	n = recvfrom(sockfd, (char *)buffer, MAXLINE,MSG_WAITALL, ( struct sockaddr *) &servaddr,&slen);
	buffer[n] = '\0';
    string recv_string(buffer);
    //cout<<"received "<<binaryToDec(recv_string.substr(0,N))<<endl;
    return recv_string;
}

void mySend(string s)
{
    //cout<<"sending "<<binaryToDec(s)<<endl;
    const char* sc = s.c_str();
    sendto(sockfd, (const char *)sc, strlen(sc),MSG_CONFIRM, (const struct sockaddr *) &servaddr,slen);
}

double generate_random()
{
    return rand() / double(RAND_MAX);
}

bool debug;
int recvport;
int maxpackets;
float error_rate;
int expected_seqnum = 0;
int total_acks = 0;
int windowsize;

void myexit()
{
    cout<<total_acks<<endl;
    exit(0);
}

int main(int argc,char** argv)
{
    //default values
    recvport = RPORT;
    start = std::chrono::high_resolution_clock::now();
    debug=false;
    maxpackets = 10;
    error_rate = 0.00001;
    windowsize = 3;
    for(int i=1;i<argc;i++)
    {
        if(strcmp(argv[i],"-d")==0)
        {
            debug = true;
        }
        else if(strcmp(argv[i],"-e")==0)
        {
            error_rate = stof(argv[i+1]);
            i+=1;
        }
        else if(strcmp(argv[i],"-p")==0)
        {
            recvport = stoi(string(argv[i+1]));
            i+=1;
        }
        else if(strcmp(argv[i],"-n")==0)
        {
            maxpackets = stoi(string(argv[i+1]));
            i+=1;
        }
        else if(strcmp(argv[i],"-w")==0)
        {
            windowsize = stoi(string(argv[i+1]));
            i+=1;
        }
        else
        {
            cerr<<"input error"<<endl;
            exit(0);
        }
    }
    //udp setup
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
	
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_addr.s_addr = INADDR_ANY;
	cliaddr.sin_port = htons(recvport);
	
	if ( bind(sockfd, (const struct sockaddr *)&cliaddr,
			sizeof(cliaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
    int act_seqnum=0;
    while(true)
    {
        string recv_string = myRecv();
        if(recv_string == "STOP")
        {
            myexit();
        }
        int cur_seqnum = binaryToDec(recv_string.substr(0,N));
        auto timenow = std::chrono::high_resolution_clock::now() - start;
        long long elapsed = std::chrono::duration_cast<std::chrono::microseconds>(timenow).count();

        if(cur_seqnum == expected_seqnum && generate_random()>error_rate)
        {
            mySend(decToBinary(cur_seqnum));
            total_acks += 1;
            expected_seqnum += 1;
            expected_seqnum %= (windowsize+1);
            if(debug == true)
            {
                cout<<"Seq# "<<cur_seqnum<<" Time received: "<<elapsed/1000<<":"<<elapsed%1000<<" Packet dropped : False"<<endl;
            }
            act_seqnum += 1;
        }
        else if(cur_seqnum == expected_seqnum && debug == true)
        {
            cout<<"Seq# "<<cur_seqnum<<" Time received: "<<elapsed/1000<<":"<<elapsed%1000<<" Packet dropped : True"<<endl;
        }
        else if(cur_seqnum != expected_seqnum && debug == true)
        {
            cout<<"Seq# "<<cur_seqnum<<" Time received: "<<elapsed/1000<<":"<<elapsed%1000<<" Packet dropped : Out Of Order"<<endl;
        }
        if(total_acks >= maxpackets)
        {
            myexit();
        }
    }
    return 0;
}
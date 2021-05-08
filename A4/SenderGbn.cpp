#include <bits/stdc++.h>
#include <thread>
#include <ctime>
#include <time.h>
#include <chrono>
#include <ratio>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <mutex>

#define ll long long
using namespace std;
using std::cout; using std::endl;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;
std::chrono::high_resolution_clock::time_point start;
int N=8;
int rtt_multiplier = 20;

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

bool debug;
string recvip;
int recvport;
int packetlength;
int packetgenrate;
int maxpackets;
int windowsize;
int buffersize;
vector<pair<int,string>> packets;
map<int,ll> lastsent;
map<int,ll> firstsent;
map<int,int> retransmission;
map<int,ll> time_generated;
mutex lock_packets;
ll cur_timeout;
int base_packet = -1;
int total_acks = 0;
int sockfd;
struct sockaddr_in cliaddr,servaddr;
const int PORT = 11030;
const  int RPORT = 11020;
const int MAXLINE = 2048;
unsigned int clen=sizeof(cliaddr);

void mySend(string s,string d="")
{
    const char* sc = s.c_str();
    sendto(sockfd, (const char *)sc, strlen(sc),MSG_CONFIRM, (const struct sockaddr *) &cliaddr,sizeof(cliaddr));
    //cout<<"sender sending "<<d<<endl;
}

string myRecv()
{
    char buffer[MAXLINE];
    int n;
    n = recvfrom(sockfd, (char *)buffer, MAXLINE,MSG_WAITALL, (struct sockaddr *) &servaddr,&clen);
    buffer[n] = '\0';
    string recv_string(buffer);
    ll timenow = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
    //cout<<"sender received "<<binaryToDec(recv_string.substr(0,N))<<" at "<<timenow<<endl;
    
    return recv_string;
}

void our_exit()
{
    int total_transmissions = 0;
    for(int i=0;i<maxpackets;i++)
    {
        //makecout<<i<<" "<<retransmission[i]<<endl;
        total_transmissions += retransmission[i];
    }
    cout<<total_transmissions<<" "<<total_acks<<endl;
    cout<<"PacketGenRate       : "<<packetgenrate<<endl;
    cout<<"packetlength        : "<<packetlength<<endl;
    if(total_acks!=0)
        cout<<"RetransmissionRatio : "<<(1.0*total_transmissions/total_acks)<<endl;
    cout<<"AverageRTT          : "<<cur_timeout/rtt_multiplier<<endl;
    mySend("STOP");
    exit(0);
}

void generatePacket()
{
    int cur_seqnum = 0;
    while(true)
    {
        for(int i=0;i<packetgenrate;i++)
        {
            lock_packets.lock();
            if(packets.size()<buffersize && cur_seqnum<maxpackets)
            {
                string payload(8*packetlength-N, '0');
                string this_seqnum = decToBinary(cur_seqnum%(windowsize+1));
                packets.push_back(make_pair(cur_seqnum,this_seqnum + payload));
                auto timenow = std::chrono::high_resolution_clock::now() - start;
                long long elapsed = std::chrono::duration_cast<std::chrono::microseconds>(timenow).count();
                time_generated[cur_seqnum] = elapsed;
                cur_seqnum += 1;
            }
            lock_packets.unlock();
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void sendFirstPacket()
{
    while(true)
    {
        lock_packets.lock();
        auto mybase_packet = base_packet;
        auto mypackets = packets;
        lock_packets.unlock();
        for(auto packet:mypackets)
        {
            ll timenow = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
            if((packet.first <= windowsize+mybase_packet))
            {
                if(firstsent.find(packet.first)==firstsent.end())
                {
                    retransmission[packet.first]=1;
                    lastsent[packet.first] = timenow;
                    firstsent[packet.first] = timenow;
                    mySend(packet.second,to_string(packet.first));
                }
            }
        }
    }
}

void sendPacket()
{
    while(true)
    {
        //cout<<endl;
        lock_packets.lock();
        int mybase_packet = base_packet;
        auto mypackets = packets;
        lock_packets.unlock();
        ll timenow = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
        if(mypackets.empty())
            continue;
        if((firstsent.find(mypackets[0].first)!=firstsent.end()) && (mypackets[0].first <= windowsize+mybase_packet) && ((mypackets[0].first<10 && (timenow - lastsent[mypackets[0].first] > 100000ll)) ||
            (mypackets[0].first>=10 && (timenow - lastsent[mypackets[0].first] > max(cur_timeout,5000ll))) ))
        { 
            for(int i=0;i<mypackets.size();i++)
            {
                auto repacket = mypackets[i];
                if(repacket.first > windowsize+mybase_packet || firstsent.find(repacket.first)==firstsent.end())
                    break;
                retransmission[repacket.first] += 1;
                ll timenow = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
                lastsent[repacket.first] = timenow;
                mySend(repacket.second,to_string(repacket.first));
                if(retransmission[repacket.first] > 7)
                {
                    //cout<<timenow<<" "<<cur_timeout<<endl;
                    //cout<<"sent "<<repacket.first<<" this many time "<<retransmission[repacket.first]<<endl;
                    our_exit();
                }
            }
        }
    }
}

void recvPacket()
{
    total_acks = 0;
    while(true)
    {
        
        string recv_string = myRecv();
        lock_packets.lock();
        int recv_seqnum = binaryToDec(recv_string.substr(0,N));
        int act_seqnum=recv_seqnum;
        for(auto it:packets)
        {
            if(it.first%(windowsize+1)==recv_seqnum)
            {
                act_seqnum = it.first;
                break;
            }
        }
      
        if(retransmission[act_seqnum]==0)
        {            
            cout<<"Seq# "<<recv_seqnum<<" Old duplicate packet ack"<<endl;
            lock_packets.unlock();
            continue;
        }
        ll timenow = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
        ll this_rtt = (timenow-firstsent[act_seqnum]);
        if(debug == true)
        {
            cout<<"Seq# "<<recv_seqnum<<" Time generated: "<<time_generated[act_seqnum]/1000<<":"<<time_generated[act_seqnum]%1000<<" RTT:"<<this_rtt<<" Number of attempts:"<<retransmission[act_seqnum]<<endl;
        }
        while(!packets.empty() && packets[0].first <= act_seqnum)
        {
            //cout<<"removing "<<packets[0].first<<endl;
            cur_timeout = (cur_timeout * total_acks + rtt_multiplier * this_rtt)/(total_acks+1);
            int temp = packets[0].first;
            packets.erase(packets.begin());
            base_packet = temp;
            total_acks += 1;
            if(total_acks >= maxpackets)
                our_exit();
        }
        lock_packets.unlock();
    }
}

int main(int argc,char** argv)
{
    //default values
    start = std::chrono::high_resolution_clock::now();
    rtt_multiplier = 2;
    debug=false;
    recvip = "127.0.0.1";
    recvport = RPORT;
    packetlength = 256;
    packetgenrate = 20;
    maxpackets = 10;
    windowsize = 3;
    buffersize = 10;
    for(int i=1;i<argc;i++)
    {
        if(strcmp(argv[i],"-d")==0)
        {
            debug = true;
        }
        else if(strcmp(argv[i],"-s")==0)
        {
            recvip = string(argv[i+1]);
            i+=1;
        }
        else if(strcmp(argv[i],"-p")==0)
        {
            recvport = stoi(string(argv[i+1]));
            i+=1;
        }
        else if(strcmp(argv[i],"-r")==0)
        {
            packetgenrate = stoi(string(argv[i+1]));
            i+=1;
        }
        else if(strcmp(argv[i],"-l")==0)
        {
            packetlength = stoi(string(argv[i+1]));
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
        else if(strcmp(argv[i],"-b")==0)
        {
            buffersize = stoi(string(argv[i+1]));
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

	memset(&cliaddr, 0, sizeof(cliaddr));
	
	// Filling server information
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_port = htons(recvport);
	cliaddr.sin_addr.s_addr = INADDR_ANY;

    //call threads
    thread generatePacket_thread(generatePacket);
    thread sendFirstPacket_thread(sendFirstPacket);
    thread recvPacket_thread(recvPacket);
    thread sendPacket_thread(sendPacket);
    generatePacket_thread.join();
    sendPacket_thread.join();
    recvPacket_thread.join();
    sendFirstPacket_thread.join();
    return 0;
}

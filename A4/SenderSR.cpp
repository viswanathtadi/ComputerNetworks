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
int N1 = 10;
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

double generate_random()
{
    return rand() / double(RAND_MAX);
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

void mySend(string s)
{
    const char* sc = s.c_str();
    sendto(sockfd, (const char *)sc, strlen(sc),MSG_CONFIRM, (const struct sockaddr *) &cliaddr,sizeof(cliaddr));
    //cout<<"sender sending "<<binaryToDec(s.substr(0,N))<<endl;
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
    //cout<<total_transmissions<<" "<<total_acks<<endl;
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
                string payload(8*(generate_random()*(packetlength-40)+40)-N, '0');
                //payload += decToBinary(cur_seqnum,N1);
                string this_seqnum = decToBinary(cur_seqnum%(2*windowsize));
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
                    //cout<<"send "<<packet.first<<" "<<binaryToDec(packet.second.substr(0,N))<<endl;
                    retransmission[packet.first]=1;
                    lastsent[packet.first] = timenow;
                    firstsent[packet.first] = timenow;
                    mySend(packet.second);
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
        //for(auto it:mypackets)
        //    cout<<it.first<<" ";
        //cout<<endl;
        for(int i=0;i<mypackets.size();i++)
        {
            if((firstsent.find(mypackets[i].first)!=firstsent.end()) && (mypackets[i].second!="") && (mypackets[i].first <= windowsize+mybase_packet) && ((mypackets[i].first<10 && (timenow - lastsent[mypackets[i].first] > 300000ll)) ||
            (mypackets[i].first>=10 && (timenow - lastsent[mypackets[i].first] > max(cur_timeout,5000ll))) ))
            {
                auto repacket = mypackets[i];
                if(repacket.first > windowsize+mybase_packet || firstsent.find(repacket.first)==firstsent.end())
                    break;
                retransmission[repacket.first] += 1;
                ll timenow = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
                lastsent[repacket.first] = timenow;
                //cout<<"resend "<<repacket.first<<" "<<binaryToDec(repacket.second.substr(0,N))<<endl;
                mySend(repacket.second);
                if(retransmission[repacket.first] > 10)
                {
                    //cout<<"failed coz "<<repacket.first<<" "<<timenow<<endl;
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
        //int act_act_seqnum = binaryToDec(recv_string.substr(recv_string.length()-N1,N1));
        int act_seqnum=recv_seqnum;
        //cout<<"recvside ";
        for(auto &it:packets)
        {
            if(it.first%(2*windowsize)==recv_seqnum)
            {
                //cout<<"emptied "<<it.first<<endl;
                act_seqnum = it.first;
                it.second = "";
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
        //cout<<"recv "<<act_seqnum<<" "<<act_act_seqnum<<" "<<timenow<<endl;
        ll this_rtt = (timenow-firstsent[act_seqnum]);
        cur_timeout = (cur_timeout * total_acks + rtt_multiplier * this_rtt)/(total_acks+1);
        if(debug == true)
        {
            cout<<"Seq# "<<recv_seqnum<<" Time generated: "<<time_generated[act_seqnum]/1000<<":"<<time_generated[act_seqnum]%1000<<" RTT:"<<this_rtt<<" Number of attempts:"<<retransmission[act_seqnum]<<endl;
        }
        /*for(auto it:packets)
        {
            cout<<it.first<<" "<<it.second<<endl;
        }*/
        while(!packets.empty() && packets[0].second == "")
        {
            //cout<<"remv "<<packets[0].first<<endl;
            base_packet = packets[0].first;
            packets.erase(packets.begin());
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
    rtt_multiplier = 2;
    start = std::chrono::high_resolution_clock::now();
    debug=false;
    recvip = "127.0.0.1";
    recvport = RPORT;
    packetlength = 256;
    packetgenrate = 20;
    maxpackets = 10;
    windowsize = 3;
    buffersize = 10;
    N = 8;
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
        else if(strcmp(argv[i],"-n")==0)
        {
            N = stoi(string(argv[i+1]));
            i+=1;
        }
        else if(strcmp(argv[i],"-L")==0)
        {
            packetlength = stoi(string(argv[i+1]));
            i+=1;
        }
        else if(strcmp(argv[i],"-R")==0)
        {
            packetgenrate = stoi(string(argv[i+1]));
            i+=1;
        }
        else if(strcmp(argv[i],"-N")==0)
        {
            maxpackets = stoi(string(argv[i+1]));
            i+=1;
        }
        else if(strcmp(argv[i],"-W")==0)
        {
            windowsize = stoi(string(argv[i+1]));
            i+=1;
        }
        else if(strcmp(argv[i],"-B")==0)
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

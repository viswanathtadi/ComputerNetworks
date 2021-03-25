#include<bits/stdc++.h>
using namespace std;
#define SS 0
#define CA 1
#define INF 1e18
bool isReceived(double ps)
{
    double r = ((double) rand() / (RAND_MAX));
    return r>ps;
}

double acked(int phase,double cwnd,double rwnd,double km,double kn,double mss)
{
    if(phase == SS)
    {
        return min(cwnd+km*mss,rwnd);
    }
    else if(phase == CA)
    {
        return min(cwnd+kn*mss*(mss/cwnd),rwnd);
    }
    return -1;
}

double timeout(double cwnd,double kf)
{
    return max(1.0,kf*cwnd);
}

int main(int argc,char** argv)
{
    double ki=1,km=1,kn=1,kf=0.5,ps=0.01;
    int T=20;
    FILE* outfile;
    for(int i=1;i<argc;i+=2)
    {
        if(argv[i][1]=='i')
        {
            string s(argv[i+1]);
            cout<<s<<endl;
            ki = stod(s);
            cout<<ki<<endl;
        }
        if(argv[i][1]=='m')
        {
            string s(argv[i+1]);
            km = stod(s);
        }
        if(argv[i][1]=='n')
        {
            string s(argv[i+1]);
            kn = stod(s);
        }
        if(argv[i][1]=='f')
        {
            string s(argv[i+1]);
            kf = stod(s);
        }
        if(argv[i][1]=='s')
        {
            string s(argv[i+1]);
            ps = stod(s);
        }
        if(argv[i][1]=='T')
        {
            string s(argv[i+1]);
            T = stoi(s);
        }
        if(argv[i][1]=='o')
        {
            outfile = fopen(argv[i+1],"w");
        }
    }
    

    double cwnd;            // in KB
    const double rwnd=1000; // in KB
    double ssthresh;        // in KB
    int phase;              // SS , CA
    double mss=1;           

    cwnd = ki*mss;          // Initialising window size
    ssthresh = INF;         // No threshold during the start
    phase = SS;             // Begin with slow-start
    int acksdone = 0;

    while(acksdone<T)
    {
        double cwnddup = cwnd;
        for(int j=0;j<cwnddup;j+=1)
        {
            acksdone+=1;
            if(isReceived(ps))
            {
                cwnd = acked(phase,cwnd,rwnd,km,kn,mss);
            }
            else
            {
                ssthresh = cwnd/2;
                cwnd = timeout(cwnd,kf);
                phase = SS;
                break;
            }
            if(cwnd >= ssthresh)
            {
                phase = CA;
            }
        }
        int plot=ceil(cwnd);
        fprintf(outfile,"%d\n",plot);
    }
    return 0;
}
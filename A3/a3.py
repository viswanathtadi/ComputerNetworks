import threading
import socket
import sys
import time
import random
id = 0
infile = "infile.txt"
outfile = "outfile-0.txt"
hi = 1
lsai = 5
spfi = 20
base = 10002
seq_num = 0

def minDistance(dist,queue):
    minimum = int(1e9)+1
    min_index = -1
    for i in range(len(dist)):
        if dist[i] < minimum and i in queue:
            minimum = dist[i]
            min_index = i
    return min_index

def dijkstra():
    n = num_routers
    dist = [int(1e9)] * n
    parent = [-1] * n
    dist[id] = 0
    queue = []
    graph = [i[:] for i in adj]
    for i in range(n):
        queue.append(i)
    while queue:
        u = minDistance(dist,queue)
        queue.remove(u)
        for i in range(n):
            if i in queue:
                if dist[u] + graph[u][i] < dist[i]:
                    dist[i] = dist[u] + graph[u][i]
                    parent[i] = u
    for i in range(num_routers):
        if i == id:
            continue
        path = ""
        cur = i
        while cur!=-1:
            path = str(cur)+","+path
            cur = parent[cur]
        print(i,path,dist[i])

def do_spf():
    while True:
        dijkstra()
        time.sleep(spfi)

def do_hello():
    while True:
        msg = "HELLO " + str(id)
        bytesToSend = str.encode(msg)
        for i in neighbours:
            addressPort = ("127.0.0.1",base + i) 
            UDPSocket.sendto(bytesToSend, addressPort)
        time.sleep(hi)

def do_lsa():
    global seq_num
    while True:
        msg = "LSA " + str(id) + " " + str(seq_num) + " " + str(len(neighbours))
        for i in neighbours:
            msg += " " + str(i) + " " + str(neighbours[i])
        bytesToSend = str.encode(msg)
        for i in neighbours:
            addressPort = ("127.0.0.1",base + i) 
            UDPSocket.sendto(bytesToSend, addressPort)
        seq_num += 1
        time.sleep(lsai)

def do_recv():
    global adj
    global neighbours
    global neighbour_seqnum
    while True:
        msg = UDPSocket.recvfrom(1024)
        msg = format(msg[0])
        msg = msg[2:]
        msg = msg[:-1]
        msg = msg.split()
        if msg[0] == "HELLO":
            recvid = int(msg[1])
            cost = random.randint(incoming[recvid][0],incoming[recvid][1])
            msg = "HELLOREPLY " + str(id) + " " + str(recvid) + " " + str(cost)
            bytesToSend = str.encode(msg)
            addressPort = ("127.0.0.1",base + recvid)
            UDPSocket.sendto(bytesToSend, addressPort)

        elif msg[0] == "HELLOREPLY":
            neighbours[int(msg[1])] = int(msg[3])
            adj[id][int(msg[1])] = int(msg[3])
        
        elif msg[0] == "LSA":
            srcid = int(msg[1])
            curseqnum = int(msg[2])
            if srcid not in neighbour_seqnum or neighbour_seqnum[srcid] < curseqnum:
                if srcid in neighbour_seqnum:
                    neighbour_seqnum[srcid] = curseqnum
                for i in range(int(msg[3])//2 +1):
                    adj[srcid][int(msg[2*i+4])] = int(msg[2*i+5])
                msg = " ".join(msg)
                bytesToSend = str.encode(msg)
                for i in neighbours:
                    if i==srcid:
                        continue
                    addressPort = ("127.0.0.1",base + i) 
                    UDPSocket.sendto(bytesToSend, addressPort)

n = len(sys.argv)
for i in range(n//2):
    if sys.argv[2*i+1][1] == "i":
        id = int(sys.argv[2*i+2])
    if sys.argv[2*i+1][1] == "f":
        infile = sys.argv[2*i+2]
    if sys.argv[2*i+1][1] == "o":
        outfile = sys.argv[2*i+2]
    if sys.argv[2*i+1][1] == "h":
        hi = int(sys.argv[2*i+2])
    if sys.argv[2*i+1][1] == "a":
        lsai = int(sys.argv[2*i+2])
    if sys.argv[2*i+1][1] == "s":
        spfi = int(sys.argv[2*i+2])
f = open(infile,"r")
curline = f.readline()
incoming = {}
neighbours = {}
neighbour_seqnum = {}
[num_routers,num_links] = [int(i) for i in curline.split()]
for i in range(num_links):
    curline = f.readline()
    [inode,onode,l,h] = [int(i) for i in curline.split()]
    if onode == id:
        incoming[inode] = (l,h)
    if inode == id:
        neighbours[onode] = int(1e9)
        neighbour_seqnum[onode] = -1
UDPSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
UDPSocket.bind(("127.0.0.1", base+id))
begin_time = time.time()
adj = [[int(1e9)]*num_routers for i in range(num_routers)]
hello_thread = threading.Thread(target=do_hello,daemon=True)
recv_thread = threading.Thread(target=do_recv,daemon=True)
lsa_thread = threading.Thread(target=do_lsa,daemon=True)
spf_thread = threading.Thread(target=do_spf,daemon=True)
hello_thread.start()
recv_thread.start()
lsa_thread.start()
spf_thread.start()
while(time.time() <= begin_time + 40):
    pass
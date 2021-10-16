import matplotlib.pyplot as plt
import sys
f = open(sys.argv[1])
l = f.read().split("\n")
l.pop()
l = [int(i) for i in l]
plt.plot(l)
plt.xlabel("UpdateCount")
plt.ylabel("CWND")
plt.savefig(sys.argv[1][:-3]+"png")

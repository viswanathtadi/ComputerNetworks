import matplotlib.pyplot as plt
import sys

f1 = open(sys.argv[1])
l1 = f1.read().split("\n")
l1.pop()
l1 = [int(i) for i in l1]
plt.plot(l1,color="red")

f2 = open(sys.argv[2])
l2 = f2.read().split("\n")
l2.pop()
l2 = [int(i) for i in l2]
plt.plot(l2,color="blue")

plt.xlabel("UpdateCount")
plt.ylabel("CWND")
plt.savefig(sys.argv[1][:-3] + "," + sys.argv[2][:-3] + ".png")
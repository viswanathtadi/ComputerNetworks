import matplotlib.pyplot as plt
f = open("output.txt")
l = f.read().split("\n")
l.pop()
l = [int(i) for i in l]
plt.plot(l)
plt.xlabel("UpdateCount")
plt.ylabel("CWND")
plt.show()
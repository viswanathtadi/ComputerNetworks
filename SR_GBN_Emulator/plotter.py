import matplotlib.pyplot as plt
xpoints = [0.1,0.001]
ypoints = [490,66]
rypoints = [1.097,1.013]
plt.xlabel("error rate")
plt.ylabel("RTT")
for i in range(1):
    plt.plot(xpoints,ypoints[2*i:2*i+2])
    plt.savefig("es"+"png20")
    plt.clf()
import os
import sys
for ki in (1,4):
    for km in (1,1.5):
        for kn in (0.5,1):
            for kf in (0.1,0.3):
                for ps in (0.01,0.0001):
                    name = str(ki)+","+str(km)+","+str(kn)+","+str(kf)+","+str(ps)
                    cmd = "./cw -i "+str(ki)+" -m "+str(km)+" -n "+str(kn)+" -f "+str(kf)+" -s "+str(ps)+" -T "+str(1000)+" -o "+name+".txt"
                    os.system(cmd)
                    cmd = "python3 visualise.py "+name+".txt"
                    os.system(cmd)
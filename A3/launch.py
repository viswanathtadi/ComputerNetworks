import os
import sys
routers = sys.argv[1]
infile = sys.argv[2]
outfile = sys.argv[3]
hi = sys.argv[4]
lsai = sys.argv[5]
spfi = sys.argv[6]
for i in range(int(routers)):
    cmd = "python3 a3.py -i "+str(i)+" -f "+infile+" -o "+outfile+\
        " -h "+hi+ " -a "+lsai+ " -s "+spfi+" &"
    #print(cmd)
    os.system(cmd)
import os
import subprocess

for error_rate in [0.001,0.00001,0.0000001]:
    for packet_length in [256,1500]:
        for gen_rate in [20,300]:
            print("./rsrp -p 11020 -N 1000 -n 8 -W 50 -B 100 -e " + str(error_rate))
            print("./ssrp -s 127.0.0.1 -p 11020 -N 1000 -n 8 -W 50 -B 100 -L "+str(packet_length)+" -R "+str(gen_rate))
            out = subprocess.call( "./rsrp -p 11020 -N 1000 -n 8 -W 50 -B 100 -e " +\
                 str(error_rate) + " & ./ssrp -s 127.0.0.1 -p 11020 -N 1000 -n 8 -W 50 -B 100 -L "\
                     +str(packet_length)+" -R "+str(gen_rate), shell = True )
            print("")

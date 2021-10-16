import os
import subprocess

for error_rate in [0.001,0.00001,0.0000001]:
    for packet_length in [256,1500]:
        for gen_rate in [20,300]:
            print("./rgbn -p 11020 -n 1000 -w 50 -e " + str(error_rate))
            print("./sgbn -s 127.0.0.1 -p 11020 -n 1000 -w 50 -b 100 -l "+str(packet_length)+" -r "+str(gen_rate))
            out = subprocess.call( "./rgbn -p 11020 -n 1000 -w 50 -e " + \
                str(error_rate) + " & ./sgbn -s 127.0.0.1 -p 11020 -n 1000 -w 50 -b 100 -l "\
                    +str(packet_length)+" -r "+str(gen_rate), shell = True )
            print("")

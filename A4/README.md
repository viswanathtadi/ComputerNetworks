# Instructions to setup environment-

### Files and subfolders in this folder.
- `SenderGbn.cpp`
- `ReceiverGbn.cpp`
- `SenderSR.cpp`
- `ReceiverSR.cpp`
- `plots`
- `Makefile`
- `README.md`
- `REPORT.pdf`

### Generate executables.
- Run `make` to generate all executables.

### Run gbn simulation.
- Run `./rgbn -d -e error_rate -p recvport -n maxpackets -w windowsize` and `./sgbn -d -s recvip -p recvport -r packetgenrate -l packetlength -n maxpackets -w windowsize -b buffersize` on different terminals.
- The code is tested with 11020 receiver port and 11030 sender port.

### Run srp simulation.
- Run `./rsrp -d -e error_rate -p recvport -N maxpackets -n seq_num_length -W windowsize -B buffersize` and `./ssrp -d -s recvip -p recvport -n seq_num_length -R packetgenrate -L packetlength -N maxpackets -W windowsize -B buffersize` on different terminals.
- The code is tested with 11020 receiver port and 11030 sender port.

### Cleaning instructions.
- Run `make clean` to delete all executables. 	
- Run `make free` to free the ports. 	
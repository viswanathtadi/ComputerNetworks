all:
		@g++ -w -std=c++11 -pthread SenderGbn.cpp -o sgbn 
		@g++ -w -std=c++11 -pthread ReceiverGbn.cpp -o rgbn
		@g++ -w -std=c++11 -pthread SenderSR.cpp -o ssrp
		@g++ -w -std=c++11 -pthread ReceiverSR.cpp -o rsrp

gbn:
		@g++ -w -std=c++11 -pthread SenderGbn.cpp -o sgbn 
		@g++ -w -std=c++11 -pthread ReceiverGbn.cpp -o rgbn

sr:
		@g++ -w -std=c++11 -pthread SenderSR.cpp -o ssrp
		@g++ -w -std=c++11 -pthread ReceiverSR.cpp -o rsrp

clean:
		-@rm sgbn
		-@rm rgbn
		-@rm ssrp
		-@rm rsrp
		
free:
		-@fuser -k 11030/udp
		-@fuser -k 11020/udp       

cleane:
		-@rm sgbn
		-@rm rgbn
		-@rm ssrp
		-@rm rsrp
		-@rm *.txt
		-@rm *.png

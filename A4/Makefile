gbn:
		@g++ -w -std=c++11 -pthread SenderGbn.cpp -o sgn 
		@g++ -w -std=c++11 -pthread ReceiverGbn.cpp -o rgbn

sr:
		@g++ -w -std=c++11 -pthread SenderSR.cpp -o ssr
		@g++ -w -std=c++11 -pthread ReceiverSR.cpp -o rsr

clean:
		-@rm sgbn
		-@rm rgbn
		-@rm ssr
		-@rm rsr
		
free:
		-@fuser -k 11030/udp
		-@fuser -k 11020/udp       

cleane:
		-@rm sgbn
		-@rm rgbn
		-@rm ssr
		-@rm rsr
		-@rm *.txt
		-@rm *.png

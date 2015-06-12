all:	pcsc.h pcsc.cpp prepcard.cpp
	c++ -o prepcard -I. -I/usr/include/PCSC prepcard.cpp pcsc.cpp -lpcsclite
	scp * root@fktnuc11:/root/hzdr-card/

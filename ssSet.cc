extern "C" {
#include "csapp.h"
}
#include <iostream>
#include <string>
#include <bitset>

#define BUFFER 1024

//typedef basic_string<unsigned char> ustring;	// own unsigned char string type to form final buffer

int main(int argc, char** argv) {
	int clientfd, port;
	uint8_t type = 0;
	uint32_t secretKey, valueLen;
	std::string host, varName, value, padding = "";
	unsigned char finalBuf[BUFFER];
	rio_t rio;

	if (argc != 6) {
		std::cerr << "usage: " << argv[0] << "<MachinceName> <TCPport> <SecretKey> <variableName> <value>\n";
		exit(1);
	}

	host = argv[1];
	port = atoi(argv[2]);
	secretKey = atoi(argv[3]);
	varName = argv[4];
	value = argv[5];

	Rio_readinitb(&rio, clientfd);
	
	std::bitset<32> x(secretKey);
	std::cout << x << '\t' << secretKey << '\n';
	uint32_t sk = htonl(secretKey);			// convert key to net byte order
	memcpy(finalBuf, &sk, 4);
	std::bitset<32> y(sk);
	std::cout << y << '\t' << sk << '\n';
	std::bitset<8> a(finalBuf[0]);
	std::bitset<8> b(finalBuf[1]);
	std::bitset<8> c(finalBuf[2]);
	std::bitset<8> d(finalBuf[3]);
	std::cout << a << '\t' << b << '\t' << c << '\t' << d << '\n';

	//Rio_writen(clientfd, &secretKey, 4);	// sends key to server (4 B)
	type = htonl(type);						// convert type to net byte order
	//Rio_writen(clientfd, &type, 1);			// send key to server (1 B)
	padding = "123";
	//Rio_writen(clientfd, &padding, 3);		// send padding (3 B)

	if (varName.length() > 15)				// if varName is > 15 B, then just cut off the rest
		varName = varName.substr(0, 15);
	//Rio_writen(clientfd, const_cast<char *>(varName.c_str()), varName.length()+1);	// send varName as a c string
	
	if (value.length() > 99)				// if value is > 99 B, then just cut off the rest
		value = value.substr(0, 99);
	valueLen = value.length();				
	valueLen = htonl(valueLen);				// convert valueLen to net byte order
	//Rio_writen(clientfd, &valueLen, 4);		// send valueLen (4 B)

	valueLen = ntohl(valueLen);
	//Rio_writen(clientfd, const_cast<char *>(value.c_str()), valueLen);	// send value as a c string

	return 0;
}

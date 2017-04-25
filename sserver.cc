extern "C" {
#include "csapp.h"
}
#include "sserver.h"
#include <iostream>
#include <string>
#include <stdint.h>

#define BUFFER 1024
int clientfd;
rio_t rio;
char pad[] = {'1', '2', '3'};

int simpleSet(std::string host, uint32_t port, uint32_t secretKey, std::string varName, std::string value, uint32_t valueLen) {
	char type = 0;
	char retValue;

	clientfd = Open_clientfd(const_cast<char *>(host.c_str()), port);	// connect to server
	Rio_readinitb(&rio, clientfd);

	////////////////////////  SEND DATA TO SERVER ////////////////////////////////////
	secretKey = htonl(secretKey);										// convert key to net byte order
	Rio_writen(clientfd, &secretKey, 4);								// sends key to server (4 B)
	Rio_writen(clientfd, &type, 1);										// send key to server (1 B)
	Rio_writen(clientfd, pad, 3);										// send padding (3 B)

	if (varName.length() > 15)											// if varName is > 15 B, then just cut off the rest
		varName = varName.substr(0, 15);								// keep first 15 B
	else {
		varName += "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	}
	Rio_writen(clientfd, const_cast<char *>(varName.c_str()), 16);	// send varName as a c string
	
	if (valueLen > 99) {												// if value is > 99 B, then just cut off the rest
		value = value.substr(0, 99);									// keep first 99 B
		valueLen = 100;
	}
	valueLen = htonl(valueLen);											// convert valueLen to net byte order
	Rio_writen(clientfd, &valueLen, 4);									// send valueLen (4 B)

	valueLen = ntohl(valueLen);											// convert valueLen back to host byte order to use
	Rio_writen(clientfd, const_cast<char *>(value.c_str()), valueLen);	// send value as a c string

	//////////////////////  RECEIVE DATA FROM SERVER ///////////////////////////////
	Rio_readnb(&rio, &retValue, 1);										// read return value from server
	Rio_readnb(&rio, pad, 3);

	if (retValue == -1) { 												// print failed if server retruned -1
		std::cout << "FAILED\n";
		return -1;
	}
	return 0;

}	// int simpleSet

int simpleGet(std::string host, uint32_t port, uint32_t secretKey, std::string varName, std::string value, uint32_t* resultLen) {
	char type = 1, retValue;
	
	clientfd = Open_clientfd(const_cast<char*>(host.c_str()), port);
	Rio_readinitb(&rio, clientfd);

	//  SEND DATA TO SERVER //
	secretKey = htonl(secretKey);
	Rio_writen(clientfd, &secretKey, 4);
	Rio_writen(clientfd, &type, 1);
	Rio_writen(clientfd, pad, 3);

	if (varName.length() > 15)
		varName = varName.substr(0,15);
	else 
		varName += "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	Rio_writen(clientfd, const_cast<char*>(varName.c_str()), 16);

	//	RECEIVE DATA FROM SERVER //
	Rio_readnb(&rio, &retValue, 1);
	Rio_readnb(&rio, pad, 3);

	if (!retValue) {
		Rio_readnb(&rio, &resultLen, 4);
		*resultLen = ntohl(*resultLen);
		if (*resultLen > 100)
			*resultLen = 100;
		char result[BUFFER];
		Rio_readnb(&rio, &result, *resultLen);
		value = result;
		std::cout << value << '\n';
		return 0;
	}
	
	std::cout << "FAILED\n";
	return -1;

}	// int simpleGet()

int simpleDigest(std::string host, uint32_t port, uint32_t secretKey, std::string data, uint32_t dataLen, std::string result, uint32_t resultLen) {

}	// int simpleDigest()

int simpleRun(std::string host, uint32_t port, uint32_t secretKey, std::string request, std::string result, uint32_t resultLen) {

}	// int simpleRun()

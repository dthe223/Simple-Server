extern "C" {
#include "csapp.h"
}
#include "sserver.h"
#include <iostream>
#include <string>
#include <stdint.h>

#define BUFFER 1024
#define INT_SIZE 4
#define PAD_SIZE 3
#define VAR_NAME_MAX 16
#define VALUE_MAX 100
#define PROG_REQ_MAX 8

int clientfd;
rio_t rio;
char pad[] = {'1', '2', '3'};

int simpleSet(std::string host, uint32_t port, uint32_t secretKey, std::string varName, std::string value, uint32_t valueLen) {
	char type = 0;
	char retValue = 11;

	clientfd = Open_clientfd(const_cast<char *>(host.c_str()), port);	// connect to server
	Rio_readinitb(&rio, clientfd);

	////////////////////////  SEND DATA TO SERVER ////////////////////////////////////
	secretKey = htonl(secretKey);										// convert key to net byte order
	Rio_writen(clientfd, &secretKey, INT_SIZE);								// sends key to server (4 B)
	Rio_writen(clientfd, &type, 1);										// send key to server (1 B)
	Rio_writen(clientfd, pad, PAD_SIZE);										// send padding (3 B)

	if (varName.length() > VAR_NAME_MAX-1)											// if varName is > 15 B, then just cut off the rest
		varName = varName.substr(0, VAR_NAME_MAX-1);								// keep first 15 B
	else {
		varName += "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	}
	Rio_writen(clientfd, const_cast<char *>(varName.c_str()), VAR_NAME_MAX);	// send varName as a c string
	
	if (valueLen > VALUE_MAX-1) {												// if value is > 99 B, then just cut off the rest
		value = value.substr(0, VALUE_MAX-1);									// keep first 99 B
		valueLen = VALUE_MAX;
	}
	valueLen = htonl(valueLen);											// convert valueLen to net byte order
	Rio_writen(clientfd, &valueLen, INT_SIZE);									// send valueLen (4 B)

	valueLen = ntohl(valueLen);											// convert valueLen back to host byte order to use
	Rio_writen(clientfd, const_cast<char *>(value.c_str()), valueLen);	// send value as a c string

	//////////////////////  RECEIVE DATA FROM SERVER ///////////////////////////////
	Rio_readnb(&rio, &retValue, 1);										// read return value from server
	Rio_readnb(&rio, pad, PAD_SIZE);

	if (retValue != 0) { 												// print failed if server retruned -1
		std::cout << "FAILED\n";
		return -1;
	}
	return 0;

}	// int simpleSet

int simpleGet(std::string host, uint32_t port, uint32_t secretKey, std::string varName, std::string value, uint32_t* resultLen) {
	char type = 1, retValue = 11;
	
	clientfd = Open_clientfd(const_cast<char*>(host.c_str()), port);		// connect to server
	Rio_readinitb(&rio, clientfd);

	//  SEND DATA TO SERVER //
	secretKey = htonl(secretKey);
	Rio_writen(clientfd, &secretKey, INT_SIZE);	// send secretKey
	Rio_writen(clientfd, &type, 1);			// send type
	Rio_writen(clientfd, pad, PAD_SIZE);			// send padding

	if (varName.length() > VAR_NAME_MAX-1)				// if longer than max of 15 bytes, only take first 15 B
		varName = varName.substr(0,VAR_NAME_MAX-1);
	else 									// else, add padding up to 15 bytes
		varName += "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	Rio_writen(clientfd, const_cast<char*>(varName.c_str()), VAR_NAME_MAX);	// send variable name
	
	//	RECEIVE DATA FROM SERVER //
	Rio_readnb(&rio, &retValue, 1);				// read return value
	Rio_readnb(&rio, pad, PAD_SIZE);					// read padding
	if (retValue != 0) {	// if key isn't right or retValue is not 0, then fail
		std::cerr << "FAILED\n";
		return -1;
	} else if (retValue == 0) {					
		uint32_t resLen;
		Rio_readnb(&rio, &resLen, INT_SIZE);			// read length of result
		resLen = ntohl(resLen);
		if (resLen > VALUE_MAX)						// if legnth is greater than 100, equate to 100
			resLen = VALUE_MAX;
		char result[BUFFER];
		Rio_readnb(&rio, &result, resLen);		// read result into buffer
		value = result;
		std::cout << value << '\n';				// output result
		return 0;
	}

}	// int simpleGet()

int simpleDigest(std::string host, uint32_t port, uint32_t secretKey, std::string data, uint32_t dataLen, std::string result, uint32_t resultLen) {

}	// int simpleDigest()

int simpleRun(std::string host, uint32_t port, uint32_t secretKey, std::string request, std::string result, uint32_t* resultLen) {
	char type = 3, retValue = 11;

	clientfd = Open_clientfd(const_cast<char*>(host.c_str()), port);	// connect to server
	Rio_readinitb(&rio, clientfd);

	// SEND DATA TO SERVER //
	secretKey = htonl(secretKey);
	Rio_writen(clientfd, &secretKey, INT_SIZE);	// send secretkey 
	Rio_writen(clientfd, &type, 1);			// send type
	Rio_writen(clientfd, pad, PAD_SIZE);			// send padding

	if (request.length() > PROG_REQ_MAX-1)				// if longer than 7 B, then cut down to 7 B
		request = request.substr(0,PROG_REQ_MAX-1);
	else									// if shorter, then add padding 
		request += "\0\0\0\0\0\0\0\0";
	Rio_writen(clientfd, const_cast<char*>(request.c_str()), PROG_REQ_MAX);	// send request

	// RECEIVE DATA FROM SERVER //
	Rio_readnb(&rio, &retValue, 1);			// read return value
	Rio_readnb(&rio, pad, PAD_SIZE);				// read padding
	if (retValue != 0) {					// failed if retValue is not 0
		std::cerr << "FAILED\n";
		return -1;
	} else {				
		uint32_t resLen;
		Rio_readnb(&rio, &resLen, INT_SIZE);		// read length of result
		resLen = ntohl(resLen);
		if (resLen > VALUE_MAX)					// if greater than 100, set equal to 100
			resLen = VALUE_MAX;
		char resBuf[BUFFER];		
		Rio_readnb(&rio, &resBuf, resLen);	// read result into buffer
		result = resBuf;
		std::cout << result << '\n';		// output result
		return 0;
	}
}	// int simpleRun()

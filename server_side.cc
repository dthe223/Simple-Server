#include "csapp.h"
#include <iostream>
#include <cstdlib>
#include <map>

int main(int argc, char **argv) {
	uint32_t secretKey, clientSecretKey, requestType;
	int listenfd, connfd, port, retVal;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
	rio_t rio;
	std::string clientReqType = clientDetails = clientCompletion = "";
	std::map<std::string, std::string> serverVariables;

	if (argc != 3) {
		std::cerr << "usage: " << argv[0] << " <port> <secretkey>\n";
		exit(1);
	}

	port = atoi(argv[1]);
	secretKey = atoi(argv[2]);
	
	listenfd = Open_listenfd(port);
	while (true) {
		
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

		Rio_readnb(&rio, &clientSecretKey, 4);		// read first 4 bytes for secret key
		clientSecretKey = ntohl(clientSecretKey);	// convert from network byte order to host byte order
		
		if (clientSecretKey != secretKey) {	// if keys don't match, deny conn
			std::cout << "Secret Key = " << clientSecretKey << '\n';
			Close(connfd);
			continue;
		}

		Rio_readnb(&rio, &requestType, 1);	// read next 1 byte for request type
		requestType = ntohl(requestType);

		Rio_readnb(&rio, NULL, 3);	// next 3 bytes are for padding

		if (requestType == 0) {			// SET
			clientReqType = "Set";
			std::string varName = "", value = "";
			uint32_t valueLen;
			ssize_t varNameLen = Rio_readlineb(&rio, &varName, 15); // read variable name until null char, to max len of 15 bytes
			Rio_readnb(&rio, &valueLen, 4);	// next 4 bytes -> len of var
			valueLen = ntohl(valueLen);
			if (valueLen > 100) {	// len can't exceed 100 bytes
				retVal = -1;
				Rio_writen(&rio, &ret
			} else {
				Rio_readnb(&rio, &value, valueLen); // get var value until EOF to max of valueLen bytes
				serverVariables[varName] = value; // store value in varName key in map

				Rio_writen(connfd, "0123", 4);	// send "0" for success then next 3 bytes random for padding
			}
		} else if (requestType == 1) {	// GET
			clientReqType = "Get";

		} else if (requestType == 2) {	// DIGEST
			clientReqType = "Digest";
		} else if (requestType == 3) {	// RUN
			clientReqType = "Run";
		} else {						// INVALID TYPE
			itoa(requestType, clientReqType, 10); // req type int->str
			clientReqType = clientReqType + " : Invalid Type";
		}	// invalid type

		std::cout << "Secret Key = " << clientSecretKey << '\n';
		std::cout << "Request Type = " << clientReqType << '\n';
		std::cout << "Detail = " << clientDetails << '\n';
		std::cout << "Completion = " << clientCompletion << '\n';
		Close(connfd);

	}	// while (true)
}	// int main()

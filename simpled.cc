extern "C" { 
#include "csapp.h"
}
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <map>

int main(int argc, char **argv) {
	uint32_t secretKey, clientSecretKey;
	uint8_t requestType;
	int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
	rio_t rio;
	std::string clientReqType, clientDetails, clientCompletion;
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


	///////////////		SET			///////////////

		if (requestType == 0) {
			clientReqType = "Set";
			std::string varName = "", value = "", retVal = "";
			uint32_t valueLen;
			ssize_t varNameLen = Rio_readlineb(&rio, &varName, 16); 		// read variable name until null char, to max len of 15 bytes + (\0)
			if (varName[varName.size()-1] != '\0' && varNameLen > 0) 		// if varName is longer than 15 bytes, transaction failed
				clientCompletion = "Failure";
		
			Rio_readnb(&rio, &valueLen, 4);			// next 4 bytes -> len of var
			valueLen = ntohl(valueLen);
			if (valueLen > 100)			// if value is > 100 bytes, return 1 (failure) then 3 bytes of padding
				clientCompletion = "Failure";

			if (Rio_readnb(&rio, &value, valueLen) == valueLen && clientCompletion != "Failure") { 		// if bytes read == valueLen, and not failure
				serverVariables[varName] = value; 		// store value in varName key in map
				clientCompletion = "Success";
				std::string cmd = "export " + varName + "=" + value;
				system(const_cast<char *>(cmd.c_str()));	// user system() here to assign evn variable
				retVal = "0123";
				Rio_writen(connfd, &retVal, 4);			// send 0 (success) then 3 bytes for padding
			} else {		// if didn't read valueLen num of bytes
				retVal = "1234";
				Rio_writen(connfd, &retVal, 4);			// send 1 (failure) then 3 bytes for padding
			}
			clientCompletion = varName + ": " + value;			// print out details, for success or failure


	///////////////		GET			///////////////
	
		} else if (requestType == 1) {
			clientReqType = "Get";


	///////////////		DIGEST		///////////////

		} else if (requestType == 2) {
			clientReqType = "Digest";


	///////////////		RUN			///////////////
	
		} else if (requestType == 3) {
			clientReqType = "Run";


	///////////////	 INVALID TYPE  ///////////////
	
		} else {						
			std::stringstream convert;	// to convert req type from int to string
			convert << requestType;
			clientReqType = convert.str();
			clientReqType = clientReqType + " : Invalid Type";
		}	// invalid type

		std::cout << "Secret Key = " << clientSecretKey << '\n';
		std::cout << "Request Type = " << clientReqType << '\n';
		std::cout << "Detail = " << clientDetails << '\n';
		std::cout << "Completion = " << clientCompletion << '\n';
		Close(connfd);

	}	// while (true)
	return 0;
}	// int main()

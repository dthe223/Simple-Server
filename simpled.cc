extern "C" { 
#include "csapp.h"
}
#include "sserver.h"
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <map>

int main(int argc, char **argv) {
	uint32_t secretKey, clientSecretKey, valueLen;
	char requestType;
	int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
	rio_t rio;
	char retValue;
	char varName[16];
	std::string clientReqType, clientDetails, clientCompletion, padding, envName, envValue;
	char pad[3];
	pad[0] = '1'; pad[1] = '2'; pad[2] = '3';

	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <port> <secretkey>\n";
		exit(1);
	}

	std::cout << "----------------------------------------\n";
	port = atoi(argv[1]);
	secretKey = atoi(argv[2]);
	
	listenfd = Open_listenfd(port);										// open server to listen on port until Ctrl-c
	while (true) {
		
		clientlen = sizeof(clientaddr);
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		Rio_readinitb(&rio, connfd);

		Rio_readnb(&rio, &clientSecretKey, 4);							// read first 4 bytes for secret key
		clientSecretKey = ntohl(clientSecretKey);						// convert from network byte order to host byte order
		if (clientSecretKey != secretKey) {								// if keys don't match, deny conn
			std::cout << "Secret Key = " << clientSecretKey << '\n';
			std::cout << "----------------------------------------\n";
			retValue = -1;
			Rio_writen(connfd, &retValue, 1);							// return -1 for failure
			Close(connfd);
			continue;
		}
		clientCompletion = "Success";

		Rio_readnb(&rio, &requestType, 1);								// read next 1 byte for request type
		//requestType = ntohl(requestType);
		Rio_readnb(&rio, pad, 3);										// next 3 bytes are for padding

	///////////////		SET			///////////////

		if (requestType == 0) {
			clientReqType = "Set";
			
			Rio_readnb(&rio, &varName, 16); 	// read variable name until null char, to max len of 16 bytes
			envName = varName;
			if (envName.size() >= 16)  // don't need to make substr b/c it fails anyway and not added
				clientCompletion = "Failure";
		
			Rio_readnb(&rio, &valueLen, 4);								// next 4 bytes -> len of var
			valueLen = ntohl(valueLen);
			if (valueLen > 99)											// if value is > 100 bytes, return 1 (failure) then 3 bytes of padding
				clientCompletion = "Failure";
			
			if (clientCompletion != "Failure") {		// if bytes read == valueLen, and not failure
				char value[1024];
				Rio_readnb(&rio, &value, valueLen);
				envValue = value;
				clientCompletion = "Success";
				if (setenv(const_cast<char*>(envName.c_str()),const_cast<char*>(envValue.c_str()),1) != 0) { // if setenv!=0 -> unsuccessful
					retValue = -1;
					Rio_writen(connfd, &retValue, 1);
				} else {
					retValue = 0;
					Rio_writen(connfd, &retValue, 1);							// send 0 (success) 
				}
			} else {													// if didn't read valueLen num of bytes
				retValue = -1;
				Rio_writen(connfd, &retValue, 1);						// send 1 (failure)
			}
			Rio_writen(connfd, pad, 3);							// send 3 bytes for padding

			clientDetails = varName;					// print out details, for success or failure


	///////////////		GET			///////////////
	
		} else if (requestType == 1) {
			clientReqType = "Get";
			Rio_readnb(&rio, &varName, 16);				// read variable name
			envName = varName;							// varName    char[] -> str
			if (envName.size() >= 16)					// check to see if longer than max
				clientCompletion = "Failure";
			
			char* varLocation = getenv(const_cast<char*>(envName.c_str()));
			if (clientCompletion == "Failure" || varLocation == NULL) {
				clientCompletion = "Failure";
				retValue = -1;
			} else {
				retValue = 0;
				envValue = varLocation;					// varLocation   char* -> str
			}
			Rio_writen(connfd, &retValue, 1);			// send return value
			Rio_writen(connfd, pad, 3);					// send padding

			if (clientCompletion != "Failure") {		// get value if found
				clientCompletion = "Success";
				valueLen = envValue.size();
				if (valueLen >= 100) {
					valueLen = 100;
				}
				valueLen = htonl(valueLen);
				Rio_writen(connfd, &valueLen, 4);		// send value length
				valueLen = ntohl(valueLen);
				Rio_writen(connfd, const_cast<char*>(envValue.c_str()), valueLen);	// send value of env Var
			}
			clientDetails = envName;


	///////////////		DIGEST		///////////////

		} else if (requestType == 2) {
			clientReqType = "Digest";


	///////////////		RUN			///////////////
	
		} else if (requestType == 3) {
			clientReqType = "Run";
			//int simpleRun(std::string host, int TCPport, uint32_t secretKey, std::string request, std::string result, uint32_t resultLen) {
			char request[8];
			Rio_readnb(&rio, request, 8);								// read in max of 8 bytes for command
			std::string cmd = request;								// request   char[] -> string
			if (cmd.size() >= 8 || (cmd != "inet" && cmd != "hosts" && cmd != "uptime")) {
				clientCompletion = "Failure";							// if longer than 7, failed
				retValue = -1;
				Rio_writen(connfd, &retValue, 1);
				Rio_writen(connfd, pad, 3);
			} else {
				clientCompletion = "Success";
				retValue = 0;
				clientDetails = cmd;
				//Rio_writen(connfd, &retValue, 1);						// send the return value
				//Rio_writen(connfd, pad, 3);							// send the padding
				if (cmd == "inet") {
					cmd = "/sbin/ifconfig -a";
				} else if (cmd == "hosts") {
					cmd = "/bin/cat /etc/hosts";
				} else {
					cmd = "/usr/bin/uptime";
				}
				FILE* fp;
				fp = popen(const_cast<char*>(cmd.c_str()), "r");	// popen to redirect output
				std::string output = "",temp;							// str to store output that's redirected
				char line[128];
				while (fgets(line,sizeof(line),fp)) {
					temp = line;
					output += temp;
				}
				uint32_t strlength = output.size();
				std::cout << output << '\n';
				Rio_writen(connfd, &retValue, 1);
				Rio_writen(connfd, pad, 3);
				strlength = htonl(strlength);
				Rio_writen(connfd, &strlength, 4);
				char* stuff = new char[output.length() +1];
				std::strcpy(stuff, output.c_str());
				if (strlength > 99)
					Rio_writen(connfd, stuff, 100);
				else
					Rio_writen(connfd, stuff, strlength);

				pclose(fp);
			}



	///////////////	 INVALID TYPE  ///////////////
	
		} else {						
			std::stringstream convert;									// to convert req type from int to string
			convert << requestType;
			clientReqType = convert.str();
			clientReqType = clientReqType + " : Invalid Type";
		}	// invalid type

		std::cout << "Secret Key = " << clientSecretKey << '\n';		// prints all the connection details
		std::cout << "Request Type = " << clientReqType << '\n';
		std::cout << "Detail = " << clientDetails << '\n';
		std::cout << "Completion = " << clientCompletion << '\n';
		std::cout << "----------------------------------------\n";

		Close(connfd);

	}	// while (true)
	std::cout << "----------------------------------------\n\n";
	return 0;
}	// int main()

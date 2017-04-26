#include "sserver.h"
#include <iostream>
#include <string>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	uint32_t secretKey, port, resultLength = 0;
	std::string host, request, result;

	if (argc != 5) {
		std::cerr << "Usage: " << argv[0] << " <MachineName> <TCPport> <SecretKey> <program>\n";
		exit(1);
	}

	host = argv[1];
	port = atoi(argv[2]);
	secretKey = atoi(argv[3]);
	request = argv[4];

	int retVal = simpleRun(host, port, secretKey, request, result, &resultLength);

	if (!retVal)
		return 0;
	
	return -1;

}	// int main()

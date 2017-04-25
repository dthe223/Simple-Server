#include "sserver.h"
#include <iostream>
#include <string>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	uint32_t secretKey, port, resultLength = 0;
	std::string host, varName, value;

	if (argc != 5) {
		std::cerr << "Usage: " << argv[0] << " <MachineName> <TCPport> <SecretKey> <variableName>\n";
		exit(1);
	}

	host = argv[1];
	port = atoi(argv[2]);
	secretKey = atoi(argv[3]);
	varName = argv[4];

	int retVal = simpleGet(host, port, secretKey, varName, value, &resultLength);

	if (!retVal)
		return 0;
	
	return -1;

}	// int main()

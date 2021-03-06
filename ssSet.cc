#include "sserver.h"
#include <iostream>
#include <string>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	uint32_t secretKey, valueLen, port;
	std::string host, varName, value;

	if (argc != 6) {
		std::cerr << "Usage: " << argv[0] << " <MachineName> <TCPport> <SecretKey> <variableName> <value>\n";
		exit(1);
	}

	host = argv[1];
	port = atoi(argv[2]);
	secretKey = atoi(argv[3]);
	varName = argv[4];
	value = argv[5];
	valueLen = value.length();

	int retVal = simpleSet(host, port, secretKey, varName, value, valueLen);

	if (!retVal)
		return 0;
	
	return -1;

}	// int main()

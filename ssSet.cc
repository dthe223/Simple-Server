extern "C" {
#include "csapp.h" }
#include <iostream>
#include <string>

int main(int argc, char** argv) {
	int clientfd, port;
	std::string host;
	rio_t rio;

	if (argc != 6) {
		std::cerr << "usage: " << argv[0] << "<MachinceName> <TCPport> <SecretKey> <variableName> <value>\n";
		exit(1);
	}

	host = argv[1];
	port = atoi(argv[2]);

	clientfd = Open_clientfd(host, port);
	Rio_readinitb(&rio, clientfd);


}

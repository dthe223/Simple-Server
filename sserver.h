#ifndef SSERVER_H
#define SSERVER_H

#include <string>
#include <stdint.h>

int simpleSet(std::string host, uint32_t port, uint32_t secretKey, std::string varName, std::string value, uint32_t valueLen);

int simpleGet(std::string host, uint32_t port, uint32_t secretKey, std::string varName, std::string value, uint32_t* resultLen);

int simpleDigest(std::string host, uint32_t port, uint32_t secretKey, std::string data, uint32_t dataLen, std::string result, uint32_t resultLen);

int simpleRun(std::string host, uint32_t port, uint32_t secretKey, std::string request, std::string result, uint32_t* resultLen);

#endif

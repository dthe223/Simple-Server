# Simple-Server

- Many services are provided over the Internet by means of communication between clients and servers. These services include the secure shell, the network time service, and e-mail. Many "cloud" services such as file storage also fit into this category.  
- In this project, you will write your own service called simple. The goal of this project is to help you understand the basics about network programming. The project consists of two parts.  
- There are four client applications that make various requests to the server.  
- Uses the RIO library calls (such as open_clientfd(), open_listenfd(), Rio_writen(), Rio_readnb()), which can be found in the files 'csapp.c' and 'csapp.h' [here](http://csapp.cs.cmu.edu/public/code.html).

# Description

Simple server listens for incoming TCP connections using the IP address INADDR_ANY and a TCPport number specified as a command-line parameter to the server. The server only responds to requests that include a SecretKey: an integer used to prevent unauthorized access to the server (like a PIN number for a bank account). To run the server, one might type `simpled 5678 987654`, which would start the simple server listening for connections on the local machine at TCP port 5678, and would use 987654 as the SecretKey to prevent unauthorized access.  

The simple server accepts incoming requests from clients. Each request is in a new connection to the server. In other words, the server should handle only one request per connection. The details of the protocol used to exchange information between the clients and the server is described below. The SecretKey is verified on every transaction.

The server prints out the following information for every valid request it receives:

- Secret key = SecretKey  
   where SecretKey is the secret key contained in the request.
- Request type = type 
   where type is one of set, get, digest, run.
- Detail = detail 
   where detail is specific to the particular request type, as shown below.
- Completion = status 
   where status is either success or failure.
If the secret key is wrong, SecretKey line is the only think printed, and the server immediately closes the connection to the client.

# Assumptions

The programs assumes the following:
- Machine Names (DNS names or dotted decimal IP addresses) have a maximum length of 40 characters.
- Data can be either binary or text.
- Your server does not provide service unless the client includes your server's SecretKey in its requests. The server and the clients get the SecretKey from the command line.  
  - The SecretKey is an unsigned integer in the range 0 to 2<sup>32</sup>-1.

# Simple-Server

- Many services are provided over the Internet by means of communication between clients and servers. These services include the secure shell, the network time service, and e-mail. Many "cloud" services such as file storage also fit into this category.  
- In this project, you will write your own service called simple. The goal of this project is to help you understand the basics about network programming. The project consists of two parts.  
- There are four client applications that make various requests to the server.  
- Uses the RIO library calls (such as open_clientfd(), open_listenfd(), Rio_writen(), Rio_readnb()), which can be found in the files 'csapp.c' and 'csapp.h' [here](http://csapp.cs.cmu.edu/public/code.html).

# Assumptions

The programs assumes the following:
- Machine Names (DNS names or dotted decimal IP addresses) have a maximum length of 40 characters.
- Data can be either binary or text.
- Your server does not provide service unless the client includes your server's SecretKey in its requests. The server and the clients get the SecretKey from the command line.
  -- The SecretKey is an unsigned integer in the range 0 to 2[^32]-1.

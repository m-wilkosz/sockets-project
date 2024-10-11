# Network Sockets
[![](https://skillicons.dev/icons?i=java,c)](https://skillicons.dev)

Simple client-server architecture implementation. Project done for undergraduate computer networks course.

### Description
The software handles a text file transmission from the client (implemented in Java) to the server (implemented in C), where the server decrypts it, searches for a specified pattern using a string-matching algorithm (Knuth-Morris-Pratt), and then returns the pattern occurrence count back to the client.

### Setup
To run server:
```
gcc server.c -o server
./server
```
To run client:
```
javac client.java
java client

# Network Sockets Project
[![](https://skillicons.dev/icons?i=java,c)](https://skillicons.dev)

Project for computer networks course.

### Description
Project comprises of server implemented in C and client implemented in Java. Communication is carried out using network sockets. Client encrypts input, provided by user, using Vigenère cipher and server decrypts it.

### Technologies
The project was created with:
- Java 11
- C 17

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

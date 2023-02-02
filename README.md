# Network Sockets Project
[![](https://skills.thijs.gg/icons?i=java,c)](https://skills.thijs.gg)

Project for computer networks course.

### Description
Project comprises of server implemented in C and client implemented in Java.
Client encrypts input, provided by user, using Vigenère cipher and server decrypts it.

### Technologies
Project is created with:
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

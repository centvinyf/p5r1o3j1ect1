CS 513 Project 1
Text ChatRoullette

How to compile:
- unzip the folder to the target directory.
- in terminal, type "make" to run the makefile.
  makefile will compile the server and client.
- in terminal, type "make clean" to remove the execute file.

How to run the code:
- in terminal, type "./server" to run the server.
- in terminal, type "./client" to run the client.
  if the program runs successfully, nothing should show up.

How to use client:
- first, type CONNECT and press enter. program will ask you to enter hostname.
- second, type the hostname of the machine that runs the server.
- to start chatting, type CHAT and wait for another client.
- if another client also starts the chat, you will be automatically paired with he/her.
- if chatmate misbehaves, type FLAG to report and he/her will no longer be able to chat with you.
- to quit the chat channel, type QUIT while in session.
- to exit out the program, type EXIT.
- type HELP to show list of commands that are available.

How to use admin:
- after successfully running server, you can type the following commands as administrator:
- STATS: display number of clients in the chat queue, number of clients chatting, total number of users flagged
- THROWOUT: throw out a target client from a channel and destroy the channel they were using
- BLOCK: prevent a client from opening a chat channel
- UNBLOCK: allows blocked client to open a chat channel
- CLEAR: clean the ternimal command

ACKNOWLEDGEMENT:
Some of the code is based on Beej's Guide
http://beej.us/guide/bgnet/output/html/multipage/index.html
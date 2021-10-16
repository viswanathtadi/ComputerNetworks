# Instructions to setup environment-

### References
- The basic setup for this code is taken from geeksforgeeks.

### Files in this folder.
- server.c
- client.c
- README.md
- Makefile
- Report.pdf

### Generating required executables.
- Run "make" in terminal to build essential executables
- This will create server and client.
- Run "./emailserver portnum" and "./client ipaddr portnum" on seperate terminals.
  Note:Run "./emailserver" first.

### Navigating in client terminal.
You will get messages that client is connected to server.
- "general>"      preceedings mean that no user is currently selected.
- "current>"      preceedings mean that a specific user is selected.
- "Type Message>" preceedings mean that the current user can type a message.

### List of commands in general>
-  ListUsers
-  Adduser <userid>
-  SetUser <userid>
-  Quit

### List of commands in current>
- Read
- Delete
- Send <receiverid>
- Done

### Note
- All input commands are case sensitive.Please use them exactly as specified above.
- Dont create a user with name "users" as it is reserved for handling names.

### Cleaning instructions.
- Run "make clean" to remove executables.This does not remove user-related data so that you can resume at the same state after running  "make" again.
- Run "make cleane" to remove executables and all user-related data.Use this command only if you are sure to delete all users. 	

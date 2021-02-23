Instructions to setup environment-

1)In this directory (ASSIGNMENT1),you will find the following files.
     i)  server.c
     ii) client.c
     iii)README.md
     iv) Makefile
     v)  Report.pdf
     
2)Run "make" in terminal to build essential executables.
  This will create server and client.
  Run "./server" and "./client" on seperate terminals.
  Note:Run "./server" first and they will always use port number 40,000.

3)	Navigate to client terminal.
	You will get messages that client is connected to server.
	
	"general>" preceedings mean that no user is currently selected.
	
	"current>" preceedings mean that a specific user is selected.
	
	"Type Message>" preceedings mean that the current user can type a message.
	
	Note:
	i) All input commands are case sensitive.Please use them exactly as specified in Assignment1.pdf in moodle.
	ii)Dont create a user with name "users" as it is reserved for handling names.
	
4)  Run "make clean" to remove executables.This does not remove user-related data so that you can resume at the same state after running  "make" again.

	Run "make cleane" to remove executables and all user-related data.Use this command only if you are sure to delete all users. 

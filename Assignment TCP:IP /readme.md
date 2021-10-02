
Name : keshav bansal
Roll number: 2019101019

How to run:

1) First compile and run the server file.
	gcc -o server server.c
	./server
2) Compile and run the Client file.
	gcc -o client client.c
	./client

implementation:

Client code:
1. In every iteration of the outer loop, I am printing the client prompt and taking the  input through getline and parsing the string by tabs and space delimiters.
2. Then I am sending the command to the server and receiving the acknowledgement for the same.
3. Then I am sending the names of the files one by one and getting  an acknowledgement from server if it is valid or invalid and if it is valid I am receiving the size of the file to calculate percentage read after this.
4. Then I am receiving the file in the form of chunks which I have taken to be of the size "99999 bytes". And after receiving every chunk I am sending an acknowledgement and writing it to the file of the same name after creating it on the client side and  breaking the loop if the end of the file occurs. 


Server code:

1. In every iteration, server will receive the command from the client and will send an acknowledgement.
2. It will then check if the file is valid or not and send the size of the file to the client if it is valid.
3. Now, it will read the contents of the file in chunks of 99999 bytes and send it to the client and receive an acknowledgement from the client for the same.
4. if the number of bytes read are zero then it will send stop signal to the client.


/*
 ============================================================================
 Name        : clientTCP.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif


#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"

#define NO_ERROR 0
void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char *argv[]) {
	//...
#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR)
	{
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif
	int my_socket;
	my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(my_socket<0)
	{
		printf("***Error: creation socket***");
		return -1;
	}

	//Ci connettiamo al server
	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr("127.0.0.1");
	sad.sin_port = htons(6666);
	int connected = connect(my_socket, (struct sockaddr*) &sad, sizeof(sad));
	if (connected<0)
	{
		printf("***Error: connect()***");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	//Ricezione e stampa avvenuta connessione
	int byte_rcvd = 0;
	char buf[BUFFERSIZE];
	byte_rcvd = recv(my_socket, buf, BUFFERSIZE-1, 0);
	if(byte_rcvd <= 0)
	{
		printf("***Errore: recv()***");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}
	buf[byte_rcvd]='\0';
	puts(buf);

	//Invio stringhe
	messaggio msg = {"",""};

	puts("Scrivi la prima stringa che verrà fatta tutta in maiuscolo:");
	scanf("%s", msg.string1);
	puts("Scrivi la seconda stringa che verrà fatta tutta in minuscolo:");
	scanf("%s", msg.string2);

	int sended = send(my_socket, &msg, sizeof(msg), 0);
	if(sended != sizeof(msg))
	{
		printf("***Errore: send() (s1)***");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	puts("Invio riuscito.");
	puts("\nRicevuto:");

	byte_rcvd = recv(my_socket, &msg, sizeof(msg), 0);
	if(byte_rcvd <= 0)
	{
		printf("***Errore: recv() (msg)***");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}
	puts(msg.string1);
	puts(msg.string2);

	system("pause");
	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end

/*
 ============================================================================
 Name        : serverTCP.c
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

#define TRUE 1

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "protocol.h"

#define NO_ERROR 0
void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

void strUpper(char* s);

void strLower(char* s);


int main(int argc, char *argv[]) {
	printf("Avviato\n");
#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("***Error: WSAStartup()***\n");
		return 0;
	}
#endif
	int my_socket;
	my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(my_socket<0)
	{
		printf("***Error: creation socket***\n");
		return -1;
	}

	//Assegnazione dell'indirizzo alla socket
	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family=AF_INET;
	sad.sin_addr.s_addr = inet_addr("127.0.0.1");
	sad.sin_port = htons(6666);
	int binded = bind(my_socket,(struct sockaddr*) &sad, sizeof(sad));
	if(binded<0)
	{
		printf("***Error: binding***\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	//Ci mettiamo in ascolto
	if(listen(my_socket, 6)<0)
	{
		printf("***Error: listen()***\n");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	//Accettare richiesta
	struct sockaddr_in cad;
	int c_socket;
	int c_len;

	while (TRUE)
	{
		c_len = sizeof(cad);
		c_socket = accept(my_socket, (struct sockaddr*) &cad, &c_len);
		if(c_socket<0)
		{
			printf("***Error: accept()***\n");
			closesocket(c_socket);
			clearwinsock();
			return -1;
		}

		printf("Servendo il client %s...\n", inet_ntoa(cad.sin_addr));
		char* conferma = "Connessione avvenuta";
		int sended = send(c_socket, conferma, strlen(conferma), 0);
		if (sended  != strlen(conferma))
		{
			printf("***Errore: invio non riuscito correttamente***");
			closesocket(c_socket);
			clearwinsock();
			return -1;
		}

		messaggio msg = {"",""};

		int byte_recv = recv(c_socket, &msg, sizeof(msg), 0);
		if(byte_recv < 0)
		{
			printf("***Errore: recv()***");
			closesocket(c_socket);
			clearwinsock();
			return -1;
		}
		puts("\nRicevuto:");
		puts(msg.string1);
		puts(msg.string2);

		strUpper(msg.string1);
		strLower(msg.string2);

		sended = send(c_socket, &msg, sizeof(msg), 0);
		if (sended  != sizeof(msg))
		{
			printf("***Errore: invio (msg) non riuscito correttamente***");
			closesocket(c_socket);
			clearwinsock();
			return -1;
		}
	}

	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end


void strUpper(char* s)
{
	int len = strlen(s);
	for(int i=0;i<len;i++)
		s[i]=toupper(s[i]);
}


void strLower(char* s)
{
	int len = strlen(s);
	for(int i=0;i<len;i++)
		s[i]=tolower(s[i]);
}

/*
 ============================================================================
 Name        : clientUDP.c
 Author      : Rodolfo Pio Sassone
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
#include <string.h>

#define MAX_BUFF 1024
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
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif
	int my_socket;
	my_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(my_socket<0)
	{
		printf("***Error: creation socket***\n");
		return -1;
	}
	char input[512] = {""};


	//Ricevi da tastiera nome server:porta
	puts("Inserisci il nome del server e la porta (nome:porta): ");
	scanf("%s", input);

	char separator[2] = ":";
	char* t;
	char* token[2];
	int i=0;

	//Tokenization
	t = strtok(input, separator);

	while(t != NULL && i < 2)
	{
		token[i]=t;
		i++;
		t=strtok(NULL, separator);
	}

	if (i!=2)
	{
		puts("Error: Indirizzo non valido***");
		closesocket(my_socket);
		clearwinsock();
		return-1;
	}

	char hostname[512] = {""};
	int port = 0;

	strcpy(hostname, token[0]);
	port = atoi(token[1]);

	//Info su server dal nome
	struct hostent* h = gethostbyname(hostname);
	if(h == NULL)
	{
		puts("***Error: gethostbyname()***");
		system("pause");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}
	struct sockaddr_in sad;
	struct in_addr* addr = (struct in_addr*) h->h_addr;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr = *addr;
	sad.sin_port = htons(port);

	//Inviamo primo messaggio
	char* first = "Helo";
	int sended = sendto(my_socket, first, strlen(first), 0,(struct sockaddr*) &sad, sizeof(sad));
	if(sended != strlen(first))
		puts("***Error: Messaggio non inviato correttamente***");

	//Secondo messaggio
	char buff[MAX_BUFF];
	memset(buff,0,sizeof(buff));
	puts("Scrivi la stringa da inviare al server: ");
	scanf("%s", buff);
	sended = sendto(my_socket, buff, strlen(buff), 0,(struct sockaddr*) &sad, sizeof(sad));
	if(sended != strlen(buff))
		puts("***Error: Messaggio non inviato correttamente***");

	//Riceviamo risposta
	memset(buff,0,sizeof(buff));
	struct sockaddr_in fromAddr;
	memset(&fromAddr, 0, sizeof(fromAddr));
	int fromAddrSize = sizeof(fromAddr);
	recvfrom(my_socket, buff, MAX_BUFF, 0,(struct sockaddr*) &fromAddr, &fromAddrSize);
	if(fromAddr.sin_addr.s_addr != sad.sin_addr.s_addr)
	{
		puts("***Error: Ricevuto pacchetto da sorgente sconosciuta.");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}

	memset(h,0,sizeof(*h));
	h = gethostbyaddr((char*) &fromAddr.sin_addr, 4, AF_INET);
	if(h == NULL)
	{
		puts("***Error: gethostbyaddr()***");
		closesocket(my_socket);
		clearwinsock();
		return -1;
	}
	char* serverName = h->h_name;
	struct in_addr* serverAddr = (struct in_addr*) h->h_addr;
	printf("Stringa \"%s\" ricevuta dal server: %s \t %s\n", buff, serverName, inet_ntoa(*serverAddr));

	system("pause");
	closesocket(my_socket);
	clearwinsock();
	return 0;
} // main end


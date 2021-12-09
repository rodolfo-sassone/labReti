/*
 ============================================================================
 Name        : serverUDP.c
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
//...
#define TRUE 1
#define MAX_BUFF 1024
#define NO_ERROR 0
void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}


void eliminateVowels(char* s);

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
	puts("Avviato");
	int my_socket;
	my_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

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

	struct sockaddr_in cad;
	char buff[MAX_BUFF];
	while(TRUE)
	{
		int c_len = sizeof(cad);
		memset(&cad, 0 , c_len);
		int byte_rcv = recvfrom(my_socket, buff, MAX_BUFF, 0,(struct sockaddr*) &cad, &c_len);

		struct hostent* c = gethostbyaddr((char*) &cad.sin_addr, 4, PF_INET);
		if(c == NULL)
		{
			puts("***Error: gethostbyaddr()***");
			continue;
		}

		printf("Nome mittente: %s\nIndirizzo mittente: %s\nMessaggio: %s\n", c->h_name, inet_ntoa(cad.sin_addr), buff);

		memset(buff,0,sizeof(buff));
		memset(&cad, 0 , c_len);
		byte_rcv = recvfrom(my_socket, buff, MAX_BUFF, 0,(struct sockaddr*) &cad, &c_len);

		printf("Ricevuto: %s\n", buff);

		eliminateVowels(buff);

		printf("Modificato: %s\n", buff);

		int sended = sendto(my_socket, buff, strlen(buff), 0,(struct sockaddr*) &cad, sizeof(cad));
		if(sended != strlen(buff))
		{
			puts("***Error: sendto()***");
			continue;
		}

	}

	closesocket(my_socket);
	clearwinsock();
	return 0;
}


void eliminateVowels(char* s)
{
	int i=0;
	while(i<strlen(s))
	{
		if(s[i]=='a' || s[i]=='e' || s[i]=='i' || s[i]=='o' || s[i]=='u')
		{
			for(int j=i;j<strlen(s);j++)
				s[j]=s[j+1];
		}
		else
			i++;
	}
}


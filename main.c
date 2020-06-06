/*
	Create a TCP socket
*/

#include <stdio.h>
#include <winsock2.h>

//#pragma comment(lib,"ws2_32.lib") //Winsock Library

int sock_start();

int main(int argc, char *argv[]) {
	WSADATA wsa;
	SOCKET s;
	char *hostname, *message, server_reply[2000];
	int recv_size, svr_port;

	hostname = "pop.gmail.com";
	svr_port = 995;

	if (sock_start(&wsa, &s, &hostname, &svr_port))
		return 1;

	//Send some data
	message = "NOOP";
	if (send(s, message, strlen(message), 0) < 0) {
		puts("Send failed");
		return 1;
	}
	puts("Data Send\n");

	//Receive a reply from the server
	if ((recv_size = recv(s, server_reply, 2000, 0)) == SOCKET_ERROR) {
		puts("recv failed");
	}

	puts("Reply received\n");

	//Add a NULL terminating character to make it a proper string before printing
	server_reply[recv_size] = '\0';
	puts(server_reply);

	system("pause");
	return 0;
}

int get_adr(char **hostname, char **svr_adr) {

	struct hostent *he;
	struct in_addr **addr_list;
	int i = 0;

	if ( (he = gethostbyname(*hostname)) == NULL) {
		//gethostbyname failed
		printf("gethostbyname failed : %d", WSAGetLastError());
		return 1;
	}

	//Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
	addr_list = (struct in_addr **) he->h_addr_list;

	while(addr_list[i]!=NULL) i++;
	svr_adr = addr_list[i-1];

	return 0;
}

int sock_start(WSADATA *wsa, SOCKET *s, char **hostname, int *svr_port) {

	struct sockaddr_in server;
	char *svr_adr;

	puts("\n Initialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), wsa) != 0) {
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	puts("Initialised.");

	get_adr(hostname, &svr_adr);

	//Create a socket
	if ((*s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket : %d", WSAGetLastError());
	}

	puts("Socket created.");

	server.sin_addr.s_addr = inet_addr(svr_adr);
	server.sin_family = AF_INET;
	server.sin_port = htons(*svr_port);

	printf("1\n");
	printf("connecting to %d", svr_adr);
	printf("2\n");

	//Connect to remote server
	if (connect(*s, (struct sockaddr *)&server, sizeof(server)) < 0) {
		puts("connection failed");
		return 1;
	}

	puts("Connected");
	return 0;
}

void sock_end(SOCKET *s) {
	closesocket(*s);
	WSACleanup();
}

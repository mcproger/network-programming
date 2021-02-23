/*
time_server.c

Dumb server, sends local time to client
For educational purposes.
*/

#include "common.h"

int main() {
#if defined(_WIN32)
    WSDATA d;

    if (WSAStartup(MAKEWORD(2, 2), &d) {
        fprintf(stderr, "Faield to initialize\n");
        return 1;
    }
#endif
    printf("Ready to use socket API.\n");

    printf("Configuring local address... \n");
    struct addrinfo required_socket_type_info;

    memset(&required_socket_type_info, 0, sizeof(required_socket_type_info)); // set 0 as default values

    required_socket_type_info.ai_family = AF_INET; // IPv4
    required_socket_type_info.ai_socktype = SOCK_STREAM; // TCP
    required_socket_type_info.ai_flags = AI_PASSIVE; // use wildcard address

    struct addrinfo *bind_address;

    // fill bind_address with info about address, port, etc
    getaddrinfo(0, "8085", &required_socket_type_info, &bind_address);

    printf("Creating socket... \n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() call failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() call failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    freeaddrinfo(bind_address);

    printf("Listening... \n");
    int max_queued_connections = 10;
    if (listen(socket_listen, max_queued_connections) < 0) {
        fprintf(stderr, "listen() call failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Waiting for connections...\n");
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);

    // gets new socket for client communications. Also fills client_address info through accept()

    // QUESTION port for bind client socket?
    SOCKET socket_client = accept(socket_listen, (struct sockaddr *) &client_address, &client_len);
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() call failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Client is connected...\n");
    char address_buffer[100];

    // writes info about client to addrees_buffer. Fifth and sixth is optional and required for service name output.
    // We don't care about it
    getnameinfo((struct sockaddr *)&client_address, client_len, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
    printf("%s \n", address_buffer);

    printf("Reading request... \n");
    char request[1024];
    int bytes_recieved = recv(socket_client, request, 1024, 0);
    if (bytes_recieved <= 0) {
        printf("Connection terminated by client... \n");
    }

    printf("%.*s", bytes_recieved, request);
    printf("Sending response... \n");
    const char *response = "HTTP/1.1 200 OK\r\n Connection: close\r\n Content-Type: text/plain\r\n\r\n Local Time is: ";
    int bytes_send = send(socket_client, response, strlen(response), 0);
    printf("Sent %d bytes of %d\n", bytes_send, (int)strlen(response));

    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char *time_message = asctime(timeinfo);

    bytes_send = send(socket_client, time_message, strlen(time_message), 0);
    printf("Sent %d bytes of %d\n", bytes_send, (int)strlen(time_message));

    printf("Closing connection...\n");
    CLOSESOCKET(socket_client);

/*
At this point, we could call accept() on socket_listen to accept additional connections.
That is exactly what a real server would do.
However, as this is just a quick example program, we will instead close the listening socket too and terminate the program:
*/
#if defined(_WIN32)
    WSACleanup();
#endif
printf("Finished");
return 0;
}

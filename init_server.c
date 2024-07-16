#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <errno.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

#include <stdio.h>
#include <string.h>

SOCKET init_server(int port) {
    SOCKET server_socket;
    struct sockaddr_in server_addr;
    
    #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
            printf("WSAStartup failed\n");
            return INVALID_SOCKET;
        }
    #endif

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Error creating socket\n");
        #ifdef _WIN32
            WSACleanup();
        #endif
        return INVALID_SOCKET;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        closesocket(server_socket);
        #ifdef _WIN32
            WSACleanup();
        #endif
        return INVALID_SOCKET;
    }

    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed\n");
        closesocket(server_socket);
        #ifdef _WIN32
            WSACleanup();
        #endif
        return INVALID_SOCKET;
    }

    printf("Server initialized on port %d\n", port);
    return server_socket;
}

SOCKET accept_connection(SOCKET server_socket) {
    SOCKET client_socket;
    struct sockaddr_in client_addr;
    #ifdef _WIN32
        int client_addr_len = sizeof(client_addr);
    #else
        socklen_t client_addr_len = sizeof(client_addr);
    #endif

    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket == INVALID_SOCKET) {
        printf("Accept failed\n");
        return INVALID_SOCKET;
    }

    printf("New connection accepted\n");
    #ifdef _WIN32
        Sleep(100);  // Sleep for 100 milliseconds on Windows
    #else
        usleep(100000);  // Sleep for 100 milliseconds on Unix-like systems
    #endif
    return client_socket;
}

void handle_connection(SOCKET client_socket) {
    char buffer[1024] = {0};
    char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello from C Web Server!";

    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("Received request:\n%s\n", buffer);

    send(client_socket, response, strlen(response), 0);
    printf("Response sent.\n");

    closesocket(client_socket);
}

int read_request(SOCKET client_socket, char* buffer, int buffer_size) {
    int bytes_received = recv(client_socket, buffer, buffer_size - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';  // Null-terminate the received data
        return bytes_received;
    } else if (bytes_received == 0) {
        printf("Connection closed by client\n");
    } else {
        printf("recv failed\n");
    }
    return bytes_received;
}

int send_response(SOCKET client_socket, const char* response, int response_length) {
    int total_sent = 0;
    int remaining = response_length;
    int sent;

    while (total_sent < response_length) {
        sent = send(client_socket, response + total_sent, remaining, 0);
        if (sent == SOCKET_ERROR) {
            printf("Send failed: %d\n", errno);
            return SOCKET_ERROR;
        }
        total_sent += sent;
        remaining -= sent;
    }

    printf("Response sent successfully. Total bytes: %d\n", total_sent);
    return total_sent;
}

int main() {
    SOCKET server_socket, client_socket;
    int port = 8080;

    server_socket = init_server(port);
    if (server_socket == INVALID_SOCKET) {
        printf("Server initialization failed\n");
        return 1;
    }

    printf("Server is running on port %d\n", port);

    while(1) {
        client_socket = accept_connection(server_socket);
        if (client_socket != INVALID_SOCKET) {
            handle_connection(client_socket);
        }
    }

    closesocket(server_socket);
    #ifdef _WIN32
        WSACleanup();
    #endif

    return 0;
}


void close_socket(SOCKET socket) {
    closesocket(socket);
}

typedef SOCKET (*init_server_func)(int);
typedef SOCKET (*accept_connection_func)(SOCKET);
typedef int (*read_request_func)(SOCKET, char*, int);
typedef int (*send_response_func)(SOCKET, const char*, int);
typedef void (*close_socket_func)(SOCKET);

void init_functions(init_server_func* init_server_ptr,
                    accept_connection_func* accept_connection_ptr,
                    read_request_func* read_request_ptr,
                    send_response_func* send_response_ptr,
                    close_socket_func* close_socket_ptr) {
    *init_server_ptr = init_server;
    *accept_connection_ptr = accept_connection;
    *read_request_ptr = read_request;
    *send_response_ptr = send_response;
    *close_socket_ptr = close_socket;
}
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/syscall.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Prepare address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to the address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 1) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connection
    if ((new_socket = accept4(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen, SOCK_NONBLOCK)) == -1) {
        perror("accept4");
        exit(EXIT_FAILURE);
    }

    // Set up pselect6
    fd_set readfds;
    struct timespec timeout;
    timeout.tv_sec = 5; // 5 second timeout
    timeout.tv_nsec = 0;
    int nfds = new_socket + 1;

    char buffer[BUFFER_SIZE];

    while (1) {
        // Clear the set and add new_socket to it
        FD_ZERO(&readfds);
        FD_SET(new_socket, &readfds);

        int activity = syscall(SYS_pselect6, nfds, &readfds, NULL, NULL, &timeout, NULL);
        if (activity == -1) {
            perror("pselect6");
            break;
        }

        if (activity == 0) {
            printf("Timeout reached\n");
            break;
        }

        if (FD_ISSET(new_socket, &readfds)) {
            memset(buffer, 0, BUFFER_SIZE);
            int valread = read(new_socket, buffer, BUFFER_SIZE);
            if (valread > 0) {
                printf("Data received: %s\n", buffer);
            } else {
                printf("Connection closed by client\n");
                break;
            }
        }
    }

    close(new_socket);
    close(server_fd);

    return 0;
}


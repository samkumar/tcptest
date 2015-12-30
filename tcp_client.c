#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "tcpstr.h"
#include "utils.h"

#define SERVER_IP "2001:470:83ae:2:0212:6d02:0000:4021"
#define SERVER_PORT 32067

/* This passage is taken from the abstract of the sMAP paper. */
#define LONG_RESPONSE "As more and more physical information becomes available, a critical problem is enabling the simple and efficient exchange of this data. We present our design for a simple RESTful web service called the Simple Measuring and Actuation Profile (sMAP) which allows instruments and other producers of physical information to directly publish their data. In our design study, we consider what information should be represented, and how it fits into the RESTful paradigm. To evaluate sMAP, we implement a large number of data sources using this profile, and consider how easy it is to use to build new applications. We also design and evaluate a set of adaptations made at each layer of the protocol stack which allow sMAP to run on constrained devices."

int main(int argc, char** argv) {
    int csock;
    struct sockaddr_in6 laddr;
    struct sockaddr_in6 faddr;

    char* data;
    int datalen;
    int error;

    /* Create socket. */
    if ((csock = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        error_exit("Could not create socket");
    }

    /* Bind socket to port 1024. */
    laddr.sin6_family = AF_INET6;
    laddr.sin6_port = htons(1024);
    laddr.sin6_flowinfo = 0;
    memcpy(&laddr.sin6_addr, &in6addr_any, sizeof(struct in6_addr));
    laddr.sin6_scope_id = 0;
    if (bind(csock, (struct sockaddr*) &laddr, sizeof(laddr)) < 0) {
        error_exit("Could not bind socket");
    }

    /* Connect to server. */
    faddr.sin6_family = AF_INET6;
    faddr.sin6_port = htons(SERVER_PORT);
    if (1 != inet_pton(AF_INET6, SERVER_IP, &faddr.sin6_addr)) {
        error_exit("Could not convert server address to network form");
    }
    faddr.sin6_flowinfo = 0;
    faddr.sin6_scope_id = 0;
    if (connect(csock, (struct sockaddr*) &faddr, sizeof(faddr)) < 0) {
        error_exit("Could not connect to server");
    }

    /* Read strings until server sends an empty string. */
    do {
        if (error = tcpstr_recv_string(csock, &data, &datalen)) {
            retval_exit(error, "Could not receive string");
        }
        printf("%.*s\n", datalen, data);
        free(data);
    } while (datalen);

    /* Send strings, and send an empty string at the end. */
    if (error = tcpstr_send_string(csock, LONG_RESPONSE, strlen(LONG_RESPONSE),
                                   &data, &datalen)) {
        retval_exit(error, "Could not send string");
    }
    printf("Got response: %.*s\n", datalen, data);
    free(data);

    if (error = tcpstr_send_string(csock, "", 0, &data, &datalen)) {
        retval_exit(error, "Could not send empty string");
    }
    printf("Got response: %.*s\n", datalen, data);
    free(data);

    /* Shut down the connection. */
    if (shutdown(csock, SHUT_RDWR) < 0) {
        error_exit("Could not shut down connection");
    }

    /* Close the file descriptor. */
    if (close(csock) < 0) {
        error_exit("Could not close the file descriptor");
    }

    printf("Closed socket\n");

    return 0;
}

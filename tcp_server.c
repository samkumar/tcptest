#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "tcpstr.h"
#include "utils.h"

#define LONG_STRING_1 "The Transmission Control Protocol (TCP) is a core protocol of the Internet protocol suite. It originated in the initial network implementation in which it complemented the Internet Protocol (IP). Therefore, the entire suite is commonly referred to as TCP/IP. TCP provides reliable, ordered, and error-checked delivery of a stream of octets between applications running on hosts communicating over an IP network. TCP is the protocol that major Internet applications such as the World Wide Web, email, remote administration and file transfer rely on. Applications that do not require reliable data stream service may use the User Datagram Protocol (UDP), which provides a connectionless datagram service that emphasizes reduced latency over reliability."

#define LONG_STRING_2 "6LoWPAN is an acronym of IPv6 over Low power Wireless Personal Area Networks. 6LoWPAN is the name of a concluded working group in the Internet area of the IETF.\nThe 6LoWPAN concept originated from the idea that \"the Internet Protocol could and should be applied even to the smallest devices,\" and that low-power devices with limited processing capabilities should be able to participate in the Internet of Things.\nThe 6LoWPAN group has defined encapsulation and header compression mechanisms that allow IPv6 packets to be sent and received over IEEE 802.15.4 based networks. IPv4 and IPv6 are the work horses for data delivery for local-area networks, metropolitan area networks, and wide-area networks such as the Internet. Likewise, IEEE 802.15.4 devices provide sensing communication-ability in the wireless domain. The inherent natures of the two networks though, are different."

void* server(void* csockbuf) {
    int* csockptr;
    int csock;
    char* data;
    int datalen;
    int error;
    
    csockptr = (int*) csockbuf;
    csock = *csockptr;
    free(csockbuf);

    /* Send strings, and send an empty string at the end. */
    if (error = tcpstr_send_string(csock, LONG_STRING_1, strlen(LONG_STRING_1),
                                   &data, &datalen)) {
        retval_exit(error, "Could not send first string");
    }
    printf("Got response: %.*s\n", datalen, data);
    free(data);

    if (error = tcpstr_send_string(csock, LONG_STRING_2, strlen(LONG_STRING_2),
                                   &data, &datalen)) {
        retval_exit(error, "Could not send second string");
    }
    printf("Got response: %.*s\n", datalen, data);
    free(data);

    if (error = tcpstr_send_string(csock, "", 0, &data, &datalen)) {
        retval_exit(error, "Could not send empty string");
    }
    printf("Got response: %.*s\n", datalen, data);
    free(data);
    
    /* Read strings until server sends an empty string. */
    do {
        if (error = tcpstr_recv_string(csock, &data, &datalen)) {
            retval_exit(error, "Could not receive string");
        }
        printf("%.*s\n", datalen, data);
        free(data);
    } while (datalen);

    /* Shut down the connection. */
    if (shutdown(csock, SHUT_RDWR) < 0) {
        error_exit("Could not shut down connection");
    }

    /* Close the file descriptor. */
    if (close(csock) < 0) {
        error_exit("Could not close the file descriptor");
    }

    printf("Closed socket\n");

    return NULL;
}

int main(int argc, char** argv) {
    int lstnsock;
    int csock;
    pthread_t newthread;
    int error;
    int* csockbuf;
    struct sockaddr_in6 laddr;

    /* Create socket. */
    if ((lstnsock = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        error_exit("Could not create socket");
    }

    /* Bind socket to port 32067. */
    laddr.sin6_family = AF_INET6;
    laddr.sin6_port = htons(32067);
    laddr.sin6_flowinfo = 0;
    memcpy(&laddr.sin6_addr, &in6addr_any, sizeof(struct in6_addr));
    laddr.sin6_scope_id = 0;
    if (bind(lstnsock, (struct sockaddr*) &laddr, sizeof(laddr)) < 0) {
        error_exit("Could not bind socket");
    }

    /* Listen on socket. */
    if (listen(lstnsock, 10) < 0) {
        error_exit("Could not listen on socket");
    }

    for (;;) {
        /* Accept the incoming connection. */
        if ((csock = accept(lstnsock, NULL, NULL)) < 0) {
            error_exit("Could not accept connection");
        }

        /* Spawn a new thread to handle the connection. */
        csockbuf = malloc(sizeof(int));
        if (!csockbuf) {
            retval_exit(-1, "Could not allocate fd buffer");
        }
        *csockbuf = csock;
        if (error = pthread_create(&newthread, NULL, server, csockbuf)) {
            retval_exit(error, "Could not create thread");
        }
        if (error = pthread_detach(newthread)) {
            retval_exit(error, "Could not detach thread");
        }
    }
}

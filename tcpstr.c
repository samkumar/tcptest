#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

int sendfull(int sockfd, char* buffer, int buflen) {
    int bytessent = 0;
    ssize_t sendrv;
    while (bytessent < buflen) {
        sendrv = send(sockfd, buffer + bytessent, buflen - bytessent, 0);
        if (sendrv < 0) {
            return errno;
        } else {
            bytessent += sendrv;
        }
    }
    return 0;
}

int recvfull(int sockfd, char* buffer, int buflen) {
    int bytesread = 0;
    ssize_t recvrv;
    while (bytesread < buflen) {
        recvrv = recv(sockfd, buffer + bytesread, buflen - bytesread, 0);
        if (recvrv < 0) {
            return errno;
        } else {
            bytesread += recvrv;
        }
    }
    return 0;
}

int tcpstr_send_string(int sockfd, char* str, size_t slen, char** bufptr,
                       int* buflenptr) {
    char lenbuf[5];
    *bufptr = NULL;
    snprintf(lenbuf, sizeof(lenbuf), "%04d", (int) slen);
    if (sendfull(sockfd, lenbuf, 4) < 0) {
        return errno;
    }
    if (slen && sendfull(sockfd, str, slen) < 0) {
        return errno;
    }
    *bufptr = malloc(2);
    if (!*bufptr) {
        return -1; // malloc failed
    }
    *buflenptr = 2;
    if (recvfull(sockfd, *bufptr, 2) < 0) {
        return errno;
    }

    return 0;
}

int tcpstr_recv_string(int sockfd, char** bufptr, int* buflenptr) {
    char lenbuf[5];
    size_t slen;
    *bufptr = NULL;
    if (recvfull(sockfd, lenbuf, 4) < 0) {
        return errno;
    }
    lenbuf[4] = '\0';
    slen = (size_t) strtol(lenbuf, NULL, 10);
    if (slen == 0) {
        *buflenptr = 0;
    } else {
        *bufptr = malloc(slen);
        if (!*bufptr) {
            return -1; // malloc failed
        }
        *buflenptr = (int) slen;
        if (recvfull(sockfd, *bufptr, slen)) {
            return errno;
        }
    }
    if (sendfull(sockfd, "ok", 2) < 0) {
        return errno;
    }

    return 0;
}

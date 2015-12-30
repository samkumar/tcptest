/* Sends the string STR with length SLEN on the connected TCP socket FD, using
   the TCPSTR protocol.
   Allocates a buffer to store the response. Stores a pointer to the buffer
   in BUFPTR and the length of the response (always 2 bytes) in BUFLENPTR.
   Returns the errno, if an error ocurred on the socket, or -1, if malloc
   failed. *BUFPTR needs to be freed (unless an error happened, in which
   case it is set to NULL). */
int tcpstr_send_string(int sockfd, char* str, size_t slen, char** bufptr,
                       int* buflenptr);

/* Receives a string from the connected TCP socket FD, using the TCPSTR
   protocol.
   Allocates a buffer to store the string. Stores a pointer to the buffer
   in BUFPTR and the length of the string (variable length) in BUFLENPTR.
   Returns the errno, if an error ocurred on the socket, or -1, if malloc
   failed. *BUFPTR needs to be freed (unless an error happened, in which
   case it is set to NULL). */
int tcpstr_recv_string(int sockfd, char** bufptr, int* buflenptr);

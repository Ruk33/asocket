#include <stdio.h> // printf
#include "asocket.h"

static char response[] = 
"HTTP/1.1 200 OK\r\n"
"Server: who knows?\r\n"
"Content-Type: text/html; charset=iso-8859-1\r\n"
"\r\n"
"hello! this is my response."
;

void handle_event(int socket,
                  enum asocket_event event,
                  void *read,
                  size_t len)
{
    printf("NEW EVENT FROM %d\n", socket);
    switch (event) {
        case ASOCKET_NEW_CONN:
        printf("NEW CONNECTION.\n\n");
        break;
        
        case ASOCKET_CLOSED:
        printf("CONNECTION CLOSED BY THE CLIENT.\n\n");
        break;
        
        case ASOCKET_READ:
        printf("BYTES READ (%u):\n%.*s\n\n", (unsigned int) len, (int) len, (char *) read);
        // this is just for html specs... ignore it.
        // if (len < 2)
        // return;
        // int last_two = *((char *) read + len - 2) + *((char *) read + len - 1);
        // if (last_two != '\r' + '\n')
        // return;
        // printf("END OF PACKET\n\n");
        break;
        
        case ASOCKET_CAN_WRITE:
        printf("CAN WRITE\n");
        unsigned long long written = asocket_write(socket, response, sizeof(response) - 1);
        printf("%u sent\n\n", (unsigned int) written);
        printf("connection closed by server.\n\n");
        asocket_close(socket);
        break;
        
        default:
        break;
    }
}

int main(void)
{
    int socket = asocket_port(8080);
    asocket_listen(socket, handle_event);
    return 0;
}

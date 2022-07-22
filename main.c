#include <stdio.h> // printf
#include <unistd.h> // close
#include <sys/socket.h> // send
#include "asocket.h"

static char response[] = 
"HTTP/1.1 404 Not Found\r\n"
"Date: Sun, 18 Oct 2012 10:36:20 GMT\r\n"
"Server: capo te conozco? atiendo bldos\r\n"
"Content-Type: text/html; charset=iso-8859-1\r\n"
"\r\n"
"capo, quien te conoce?."
;

void handle_event(int socket,
                  enum asocket_event event,
                  void *read,
                  size_t len)
{
    switch (event) {
        case ASOCKET_NEW_CONN:
        printf("oh! we got a new connection.\n");
        break;
        
        case ASOCKET_CLOSED:
        printf("well, the client closed the connection.\n");
        break;
        
        case ASOCKET_READ:
        printf("new bytes (%ld): %.*s.\n\n", len, (int)len, (char *) read);
        if (len < 2)
            return;
        int last_two = *((char *) read + len - 2) + *((char *) read + len - 1);
        if (last_two != '\r' + '\n')
            return;
        printf("end of packet!\n\n\n");
        size_t written = asocket_write(socket, response, sizeof(response) - 1);
        printf("%ld sent\n\n", written);
        close(socket);
        break;
        
        case ASOCKET_CAN_WRITE:
        printf("can write!\n");
        break;
        
        default:
        break;
    }
}

int main(int argc, char **argv)
{
    int socket = asocket_port(3005);
    asocket_listen(socket, handle_event);
    return 0;
}

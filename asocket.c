#include <string.h> // strcpy
#include <stddef.h> // size_t
#include <sys/epoll.h> // epoll_create1, epoll_wait, epoll_event, ...
#include <sys/socket.h> // socket, bind, listen
#include <sys/un.h> // sockaddr_un
#include <netinet/in.h> // sockaddr_in, INADDR_ANY, htons
#include <unistd.h> // unlink, close
#include "asocket.h"

int asocket_port(unsigned short port)
{
    int server = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (server == -1)
        goto abort;
    
    int reuse = 1;
    if (setsockopt(server,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &reuse,
                   sizeof(reuse)) == -1)
        goto abort;
    
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    if (bind(server,
             (struct sockaddr *) &address,
             sizeof(address)) == -1)
        goto abort;
    
    if (listen(server, SOMAXCONN) == -1)
        goto abort;
    
    return server;
    
    abort:
    close(server);
    return -1;
}

int asocket_sock(char *path)
{
    if (!path)
        return -1;
    
    int server = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server == -1)
        goto abort;
    
    int reuse = 1;
    if (setsockopt(server,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &reuse,
                   sizeof(reuse)) == -1)
        goto abort;
    
    struct sockaddr_un address = {0};
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, path);
    if (unlink(path) == -1)
        goto abort;
    
    if (bind(server,
             (struct sockaddr *) &address,
             sizeof(address)) == -1)
        goto abort;
    
    if (listen(server, SOMAXCONN) == -1)
        goto abort;
    
    return server;
    
    abort:
    close(server);
    return -1;
}

void asocket_listen(int server, asocket_handler *handler)
{
    static struct epoll_event events[256] = {0};
    static unsigned char read_buf[8192] = {0};
    
    if (!handler)
        return;
    
    int epoll_fd = 0;
    struct epoll_event event = {0};
    int ev_count = 0;
    
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
        goto abort;
    
    event.data.fd = server;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server, &event) == -1)
        goto abort;
    
    while (1) {
        ev_count = epoll_wait(epoll_fd,
                              events,
                              sizeof(events) / sizeof(*events),
                              -1);
        if (ev_count == -1)
            goto abort;
        for (int i = 0; i < ev_count; i += 1) {
            // only read and write, ignore the rest.
            if (!(events[i].events & EPOLLIN) &&
                !(events[i].events & EPOLLOUT))
                continue;
            
            // server
            if (events[i].data.fd == server) {
                while (1) {
                    int client = accept4(server, 0, 0, SOCK_NONBLOCK);
                    if (client == -1)
                        break;
                    event.data.fd = client;
                    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
                    if (epoll_ctl(epoll_fd,
                                  EPOLL_CTL_ADD,
                                  client,
                                  &event) == -1)
                        close(client);
                    else
                        handler(client, ASOCKET_NEW_CONN, 0, 0);
                }
                continue;
            }
            
            // clients.
            if (events[i].events & EPOLLIN) {
                while (1) {
                    ssize_t received = recv(events[i].data.fd,
                                            read_buf,
                                            sizeof(read_buf),
                                            0);
                    if (received == -1)
                        break;
                    if (received == 0) {
                        handler(events[i].data.fd, ASOCKET_CLOSED, 0, 0);
                        break;
                    }
                    handler(events[i].data.fd,
                            ASOCKET_READ,
                            read_buf,
                            (size_t) received);
                }
            }
            if (events[i].events & EPOLLOUT)
                handler(events[i].data.fd, ASOCKET_CAN_WRITE, 0, 0);
        }
    }
    
    abort:
    close(epoll_fd);
    close(server);
}

size_t asocket_write(int socket, void *buf, size_t n)
{
    size_t sent = 0;
    if (!buf)
        return 0;
    while (sent < n) {
        ssize_t tmp = send(socket, buf + sent, n - sent, 0);
        if (tmp == -1)
            break;
        sent += tmp;
    }
    return sent;
}


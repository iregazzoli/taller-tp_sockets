#ifndef SOCKET_H
#define SOCKET_H

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <string>

class Socket {
   private:
    int skt;
    bool closed;

    explicit Socket(int skt);
    static addrinfo *resolve_address(const char *hostname, const char *servname, int ai_flags, bool is_passive);
    static addrinfo *get_next_address(addrinfo *addr);
    static void free_address_list(addrinfo *addr_list);

   public:
    Socket(const char *hostname, const char *servname);
    explicit Socket(const char *servname);

    Socket(const Socket &) = delete;
    Socket &operator=(const Socket &) = delete;

    Socket(Socket &&);
    Socket &operator=(Socket &&);

    int sendall(const void *data, unsigned int sz, bool *was_closed);
    int recvall(void *data, unsigned int sz, bool *was_closed);

    Socket accept();

    void shutdown(int how);

    int close();

    ~Socket();
};

#endif

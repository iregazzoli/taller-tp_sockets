#include "Socket.h"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>

// TODO: for debbuging
#include <iostream>

Socket::Socket(int skt) : skt(skt), closed(false) {}

// Métodos privados para reemplazar la funcionalidad de Resolver
addrinfo *Socket::resolve_address(const char *hostname, const char *servname, int ai_flags, bool is_passive) {
    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = is_passive ? AI_PASSIVE : 0;
    hints.ai_protocol = 0;

    int s = getaddrinfo(hostname, servname, &hints, &result);
    if (s != 0) {
        throw std::runtime_error("getaddrinfo failed: " + std::string(gai_strerror(s)));
    }
    // TODO: debbuging
    std::cout << "getaddrinfo: " << hostname << ", " << servname << std::endl;

    return result;
}

addrinfo *Socket::get_next_address(addrinfo *addr) {
    return addr->ai_next;
}

void Socket::free_address_list(addrinfo *addr_list) {
    freeaddrinfo(addr_list);
}

// Implementación de los constructores y métodos públicos
Socket::Socket(const char *hostname, const char *servname) {
    addrinfo *addr_list = resolve_address(hostname, servname, 0, false);

    int s = -1;
    int skt = -1;
    this->closed = true;

    for (addrinfo *addr = addr_list; addr != nullptr; addr = get_next_address(addr)) {
        if (skt != -1)
            ::close(skt);

        skt = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (skt == -1) {
            continue;
        }
        // TODO: debbuging
        std::cout << "socket created: " << skt << std::endl;

        s = connect(skt, addr->ai_addr, addr->ai_addrlen);
        if (s == -1) {
            continue;
        }
        // TODO: debbuging
        std::cout << "connection successful" << std::endl;

        this->closed = false;
        this->skt = skt;
        break;
    }

    free_address_list(addr_list);

    if (this->closed) {
        throw std::runtime_error("socket construction failed (connect to " +
                                 std::string(hostname ? hostname : "") + ":" +
                                 std::string(servname ? servname : "") + ")");
    }
}

Socket::Socket(const char *servname) {
    addrinfo *addr_list = resolve_address(nullptr, servname, AI_PASSIVE, true);

    int s = -1;
    int skt = -1;
    this->closed = true;

    for (addrinfo *addr = addr_list; addr != nullptr; addr = get_next_address(addr)) {
        if (skt != -1)
            ::close(skt);

        skt = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (skt == -1) {
            continue;
        }

        s = bind(skt, addr->ai_addr, addr->ai_addrlen);
        if (s == -1) {
            continue;
        }

        s = listen(skt, 20);
        if (s == -1) {
            continue;
        }

        // TODO: debbuging
        std::cout << "listening on socket: " << skt << std::endl;

        this->closed = false;
        this->skt = skt;
        break;
    }

    free_address_list(addr_list);

    if (this->closed) {
        throw std::runtime_error("socket construction failed (listen on :" +
                                 std::string(servname ? servname : "") + ")");
    }
}

Socket::Socket(Socket &&other) : skt(other.skt), closed(other.closed) {
    other.closed = true;
}

Socket &Socket::operator=(Socket &&other) {
    this->close();
    this->skt = other.skt;
    this->closed = other.closed;
    other.closed = true;
    return *this;
}

int Socket::sendall(const void *data, unsigned int sz, bool *was_closed) {
    const char *buf = static_cast<const char *>(data);
    *was_closed = false;
    unsigned int sent = 0;

    while (sent < sz) {
        // the + sent is to update de pointer to the buffer's index
        int s = ::send(this->skt, buf + sent, sz - sent, MSG_NOSIGNAL);
        if (s < 0) {
            throw std::runtime_error("sendall failed: " + std::string(strerror(errno)));
        } else if (s == 0) {
            *was_closed = true;
            return sent;
        }
        // TODO: debbuging
        std::cout << "sent: " << s << " bytes" << std::endl;
        sent += s;
    }

    return sent;
}

int Socket::recvall(void *data, unsigned int sz, bool *was_closed) {
    char *buf = static_cast<char *>(data);
    unsigned int received = 0;

    while (received < sz) {
        // the + received is to update de pointer to the buffer's index
        int s = ::recv(this->skt, buf + received, sz - received, 0);
        if (s < 0) {
            throw std::runtime_error("recvall failed: " + std::string(strerror(errno)));
        } else if (s == 0) {
            *was_closed = true;
            return received;
        }
        // TODO: debbuging
        std::cout << "received: " << s << " bytes" << std::endl;
        received += s;
    }

    *was_closed = false;
    return received;
}

Socket Socket::accept() {
    int new_skt = ::accept(this->skt, nullptr, nullptr);
    if (new_skt == -1) {
        throw std::runtime_error("accept failed: " + std::string(strerror(errno)));
    }
    // TODO: debbuging
    std::cout << "accepted connection: " << new_skt << std::endl;
    return Socket(new_skt);
}

void Socket::shutdown(int how) {
    if (!this->closed) {
        ::shutdown(this->skt, how);
    }
}

int Socket::close() {
    if (!this->closed) {
        this->closed = true;
        return ::close(this->skt);
    }
    return 0;
}

Socket::~Socket() {
    this->close();
}

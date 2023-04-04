// client_main.cpp
#include <iostream>

#include "Client.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: ./client <ip/hostname server> <puerto/servicename> <archivo-acciones>" << std::endl;
        return 1;
    }

    const char *hostname = argv[1];
    const char *servname = argv[2];
    const char *actions_file = argv[3];

    try {
        Client client(hostname, servname);
        client.run(actions_file);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

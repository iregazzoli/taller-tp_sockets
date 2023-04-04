// server_main.cpp
#include <iostream>

#include "Server.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: ./server <puerto/servicename> <max-x> <max-y>" << std::endl;
        return 1;
    }

    const char *port_servicename = argv[1];
    int max_x = std::stoi(argv[2]);
    int max_y = std::stoi(argv[3]);

    try {
        Server server(port_servicename, max_x, max_y);
        server.run();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

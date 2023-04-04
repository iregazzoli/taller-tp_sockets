#include "Client.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

Client::Client(const char *hostname, const char *servname) : socket(hostname, servname) {}

void Client::shoot(int value) {
    char action_data[2] = {0x01, static_cast<char>(value)};
    bool was_closed = false;
    print_buffer(action_data, 2);
    socket.sendall(action_data, 2, &was_closed);
    std::cout << "Sent shooting action" << std::endl
              << std::flush;
}

void Client::move(int direction_x, int direction_y) {
    // std::cout << "about to send moving action with: " << direction_x << " and " << direction_y << std::endl; works...
    char action_data[3] = {0x02, static_cast<char>(direction_x), static_cast<char>(direction_y)};
    bool was_closed = false;
    print_buffer(action_data, 3);
    socket.sendall(action_data, 3, &was_closed);
    std::cout << "Sent moving action" << std::endl
              << std::flush;
}

void Client::reload() {
    char action_data = 0x03;
    bool was_closed = false;
    print_buffer(&action_data, 1);
    socket.sendall(&action_data, 1, &was_closed);
}

void Client::nop() {
    char action_data = 0x00;
    bool was_closed = false;
    print_buffer(&action_data, 1);
    socket.sendall(&action_data, 1, &was_closed);
}

void Client::run(const std::string &actions_file) {
    char welcome_msg[1024];
    bool was_closed = false;
    int bytes_received = socket.recvall(welcome_msg, sizeof(welcome_msg) - 1, &was_closed);
    welcome_msg[bytes_received] = '\0';
    std::cout << "Received welcome message: " << welcome_msg << std::endl;

    execute_actions_from_file(actions_file);
}

void Client::execute_actions_from_file(const std::string &filename) {
    std::ifstream input_file(filename);
    std::string line;

    while (std::getline(input_file, line)) {
        // for empty lines or comments
        if (line.empty() || std::isspace(line[0]) || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        std::string action;
        iss >> action;
        if (action == "Shooting") {
            int value;
            iss >> value;
            shoot(value);
        } else if (action == "Moving") {
            int x, y;
            iss >> x >> y;
            move(x, y);
        } else if (action == "Reload") {
            reload();
            std::cout << "Sent reload action" << std::endl
                      << std::flush;
        } else if (action == "NOP") {
            nop();
            std::cout << "Sent NOP action" << std::endl
                      << std::flush;
        }

        // Receive ACK
        bool was_closed = false;
        uint8_t ack;
        socket.recvall(&ack, 1, &was_closed);

        if (ack != 0x04) {
            std::cerr << "Error: Received incorrect ACK: " << std::hex << static_cast<int>(ack) << std::endl;
            break;
        }

        // Receive game state after each action
        char gameState[7];
        int bytes_received = socket.recvall(gameState, sizeof(gameState), &was_closed);

        if (bytes_received != sizeof(gameState)) {
            break;
        }

        uint8_t player_state;
        uint16_t x_net, y_net, rounds_net;

        memcpy(&player_state, &gameState[0], sizeof(player_state));
        memcpy(&x_net, &gameState[1], sizeof(x_net));
        memcpy(&y_net, &gameState[3], sizeof(y_net));
        memcpy(&rounds_net, &gameState[5], sizeof(rounds_net));

        uint16_t x = ntohs(x_net);
        uint16_t y = ntohs(y_net);
        uint16_t rounds = ntohs(rounds_net);

        print_game_state(player_state, x, y, rounds);
    }

    socket.shutdown(SHUT_RDWR);
    socket.close();
}

void Client::print_game_state(uint8_t player_state, uint16_t x, uint16_t y, uint16_t rounds) {
    std::cout << "Shooting? " << (player_state & 0x01) << std::endl;
    std::cout << "Moving? " << (player_state & 0x02) << std::endl;
    std::cout << "Reloading? " << (player_state & 0x04) << std::endl;
    std::cout << "Position? " << x << " " << y << std::endl;
    std::cout << "Rounds? " << rounds << std::endl;
}
// for debugging
void Client::print_buffer(const void *buf, size_t len) {
    const unsigned char *uchar_buf = static_cast<const unsigned char *>(buf);
    for (size_t i = 0; i < len; ++i) {
        printf("%02x ", uchar_buf[i]);
    }
    printf("\n");
}
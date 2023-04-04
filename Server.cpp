#include "Server.h"

#include <cstring>
#include <iostream>

Server::Server(const char* servname, int max_x, int max_y) : game(max_x, max_y), svr_socket(servname) {}

void Server::print_game_state() const {
    game.printState();
}

void Server::move_soldier(int x, int y) {
    game.move_soldier(x, y);
}

void Server::shoot() {
    game.shoot();
}

void Server::reload() {
    game.reload();
}

void Server::run() {
    Socket client_socket = svr_socket.accept();
    std::string welcome_msg = "Welcome to the game!";
    bool was_closed = false;

    // this is just to test the connection between client and server, i will delete it in final version.
    client_socket.sendall(welcome_msg.c_str(), welcome_msg.size(), &was_closed);
    std::cout << "Sent welcome msg" << std::endl;

    while (!was_closed) {
        // TODO: debugging remove later
        std::cout << "Server loop start" << std::endl
                  << std::flush;
        uint8_t action;
        // TODO: debugging remove later
        std::cout << "Waiting for action from client" << std::endl
                  << std::flush;

        if (was_closed) {
            // TODO: debugging remove later
            std::cout << "print for testing" << std::endl;
            break;
        }

        // receives 1 byte that represents the action bc protocol
        client_socket.recvall(&action, 1, &was_closed);  // here is the problem
        // TODO: the method is for debugging remove later
        print_buffer(&action, 1);
        // TODO: debugging remove later
        std::cout << "Received action: " << static_cast<int>(action) << std::endl
                  << std::flush;

        switch (action) {
            case 0x01: {
                char shooting_value;
                client_socket.recvall(&shooting_value, 1, &was_closed);
                if (shooting_value == 0x00) {
                    game.stop_shooting();
                } else if (shooting_value == 0x01) {
                    game.start_shooting();
                }
                // TODO: debugging remove later
                std::cout << "Received shooting action" << std::endl
                          << std::flush;
                break;
            }
            case 0x02: {
                char x, y;
                client_socket.recvall(&x, 1, &was_closed);
                client_socket.recvall(&y, 1, &was_closed);
                game.move_soldier(static_cast<int>(x), static_cast<int>(y));
                // TODO: debugging remove later
                std::cout << "Received moving action" << std::endl
                          << std::flush;
                break;
            }
            case 0x03: {
                game.reload();
                // TODO: debugging remove later
                std::cout << "Received reload action" << std::endl
                          << std::flush;
                break;
            }
            case 0x00: {
                // TODO: debugging remove later
                std::cout << "Received NOP action" << std::endl
                          << std::flush;
                break;
            }
            default: {
                // TODO: debugging remove later
                std::cerr << "Unknown action received: " << static_cast<int>(action) << std::endl;
            }
        }

        // TODO: debugging remove later, this whole ack is to check if the msg between server and client changes for some reason,
        //  and it does you can check that the server sends 0x04 but the client doesn't receive 0x04
        uint8_t ack = 0x04;
        client_socket.sendall(&ack, 1, &was_closed);
        std::cout << "Sent ACK to client: " << std::hex << static_cast<int>(ack) << std::endl;
        std::cout << "Connection closed? " << (was_closed ? "Yes" : "No") << std::endl;

        // Send state of game to client
        uint16_t x, y, rounds;
        x = game.get_x();
        y = game.get_y();
        rounds = game.get_rounds();
        uint8_t player_state = game.get_player_state();
        char gameState[7];

        uint16_t x_net = htons(x);
        uint16_t y_net = htons(y);
        uint16_t rounds_net = htons(rounds);

        memcpy(&gameState[0], &player_state, sizeof(player_state));
        memcpy(&gameState[1], &x_net, sizeof(x_net));
        memcpy(&gameState[3], &y_net, sizeof(y_net));
        memcpy(&gameState[5], &rounds_net, sizeof(rounds_net));

        client_socket.sendall(gameState, sizeof(gameState), &was_closed);
        // TODO: debugging remove later
        std::cout << "Sent game state to client." << std::endl
                  << std::flush;
        print_game_state();
        // TODO: debugging remove later
        std::cout << "Server loop end" << std::endl
                  << std::flush;
    }

    client_socket.shutdown(SHUT_RDWR);
    client_socket.close();
}

// TODO: the method is for debugging remove later
void Server::print_buffer(const void* buf, size_t len) {
    const unsigned char* uchar_buf = static_cast<const unsigned char*>(buf);
    for (size_t i = 0; i < len; ++i) {
        printf("%02x ", uchar_buf[i]);
    }
    printf("\n");
}

Server::~Server() {}

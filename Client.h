#ifndef CLIENT_H
#define CLIENT_H

#include <string>

#include "socket.h"

class Client {
   private:
    Socket socket;

   public:
    Client(const char *hostname, const char *servname);

    // Methods representing the possible actions a player can take
    void shoot(int value);
    void move(int direction_x, int direction_y);
    void reload();
    void nop();

    // Method to run the client's main loop
    void run(const std::string &actions_file);
    void execute_actions_from_file(const std::string &filename);
    void print_game_state(uint8_t player_state, uint16_t x, uint16_t y, uint16_t rounds);

    // TODO: the method is for debugging remove later
    void print_buffer(const void *buf, size_t len);
};

#endif  // CLIENT_H

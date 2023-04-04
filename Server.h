#ifndef SERVER_H
#define SERVER_H

#include "Game.h"
#include "socket.h"

class Server {
   private:
    Game game;
    Socket svr_socket;

   public:
    Server(const char* servname, int max_x, int max_y);
    ~Server();

    void print_game_state() const;
    void move_soldier(int x, int y);
    void shoot();
    void reload();
    // TODO: the method is for debugging remove later
    void print_buffer(const void* buf, size_t len);

    // Method to run the server's main loop
    void run();
};

#endif  // SERVER_H

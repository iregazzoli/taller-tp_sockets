#ifndef GAME_H
#define GAME_H

#include <string>

enum class SoldierStatus {
    Idle,
    Shooting,
    Moving,
    ShootingMoving,
    Reloading
};

struct GameStatus {
    SoldierStatus state;
    int position_x;
    int position_y;
    int rounds;
};

class Game {
   public:
    Game(int max_x, int max_y);
    ~Game();

    GameStatus get_state() const;
    void printState() const;
    void move_soldier(int x, int y);
    void shoot();
    void reload();
    void start_shooting();
    void stop_shooting();
    int get_x();
    int get_y();
    int get_rounds();
    uint8_t get_player_state();

   private:
    GameStatus status;
    void changeSoldierStatus(SoldierStatus newState);
    int max_x;
    int max_y;
};

#endif  // GAME_H

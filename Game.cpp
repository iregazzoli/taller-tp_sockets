#include "Game.h"

#include <iostream>

Game::Game(int maximun_x, int maximun_y)
    : max_x(maximun_x), max_y(maximun_y) {
    status.state = SoldierStatus::Idle;
    status.position_x = 0;
    status.position_y = 0;
    status.rounds = 10;
}

Game::~Game() {}

GameStatus Game::get_state() const {
    return status;
}

void Game::printState() const {
    std::cout << "Shooting? " << (status.state == SoldierStatus::Shooting || status.state == SoldierStatus::ShootingMoving ? 1 : 0) << std::endl;
    std::cout << "Moving? " << (status.state == SoldierStatus::Moving || status.state == SoldierStatus::ShootingMoving ? 1 : 0) << std::endl;
    std::cout << "Reloading? " << (status.state == SoldierStatus::Reloading ? 1 : 0) << std::endl;
    std::cout << "Position? " << status.position_x << " " << status.position_y << std::endl;
    std::cout << "Rounds? " << status.rounds << std::endl;
    std::cout << "\n"
              << std::endl;  // no idea why VS is putting this in an extra line
}

void Game::changeSoldierStatus(SoldierStatus newState) {
    status.state = newState;
}

void Game::move_soldier(int x, int y) {
    if (status.position_x + x >= 0 && status.position_x + x <= max_x &&
        status.position_y + y >= 0 && status.position_y + y <= max_y) {
        status.position_x += x;
        status.position_y += y;
    }
    changeSoldierStatus(SoldierStatus::Moving);
}

void Game::start_shooting() {
    shoot();
}

void Game::stop_shooting() {
    status.state = SoldierStatus::Idle;
}

void Game::shoot() {
    if (status.rounds > 0 && (status.state != SoldierStatus::Reloading)) {
        status.rounds--;
        if (status.state == SoldierStatus::Moving) {
            status.state = SoldierStatus::ShootingMoving;
        } else {
            status.state = SoldierStatus::Shooting;
        }
    }
    changeSoldierStatus(SoldierStatus::Shooting);
}

void Game::reload() {
    status.state = SoldierStatus::Reloading;
    status.rounds = 10;
}

int ::Game::get_rounds() {
    return status.rounds;
}

int Game::get_x() {
    return status.position_x;
}

int Game::get_y() {
    return status.position_y;
}

uint8_t Game::get_player_state() {
    switch (status.state) {
        case SoldierStatus::Idle:
            return 0x00;
        case SoldierStatus::Shooting:
            return 0x01;
        case SoldierStatus::Moving:
            return 0x02;
        case SoldierStatus::ShootingMoving:
            return 0x03;
        case SoldierStatus::Reloading:
            return 0x04;
    }
    return 0x00;  // just to avoid warning "warning: control reaches end of non-void function [-Wreturn-type]"
}

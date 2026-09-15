#ifndef PROJEKT1_GRA_GAME_HPP
#define PROJEKT1_GRA_GAME_HPP
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>

inline float f_rand_positive(const float min, const float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / (max - min));
}

inline float f_rand(const float min, const float max) {
    if (min <= 0.f && max >= 0.f) {
        float r1 = f_rand_positive(0.f, max);
        float r2 = -f_rand_positive(0.f, -min);
        if (rand() % 2 == 1) {
            return r1;
        }
        return r2;
    }
    else if (min <= 0.f && max <= 0.f) {
        return -f_rand_positive(-max, -min);
    }
    else return f_rand_positive(min, max);
}

enum asteroid_type {BIG, MEDIUM, SMALL};

struct Asteroid {
    sf::CircleShape asteroid;
    sf::Vector2f velocity;
    float size;
    asteroid_type type = BIG;
};

struct Projectile {
    sf::CircleShape bullet;
    sf::Vector2f velocity;
    int range = 400;
};

class Game {
private:
    // Window
    sf::RenderWindow window;
    unsigned int window_width = 700;
    unsigned int window_height = 700;
    // Game variables
    int hp = 3;
    bool paused = false;
    bool collisions = false;
    int safety_timer = 900;
    float bullet_size = 2.f;
    int shot_cooldown = 120;
    float bullet_speed = 1.f;
    int score = 0;
    // Player variables
    float thrust = 0.003f; // engine power
    float rotation_speed = 0.8f;
    float friction = 0.998f; // "air" resistance
    float player_size = 20.f; // 40x40 square
    sf::Vector2f velocity;
    //Font
    sf::Font font;
    // Game menu variables
    bool continue_pressed = true, exit_pressed = false;
    //Eng of game menu variables
    bool p_again_pressed = true, finish_pressed = false;

    // Game objects
    sf::CircleShape player;
    std::vector <Asteroid> asteroids;
    std::vector <Projectile> projectiles;

    // Texts
    sf::Text Score;
    sf::Text exit_button;
    sf::Text continue_button;
    sf::Text play_again_button;

    // Asteroids templates
    sf::CircleShape Big;
    sf::CircleShape Med;
    sf::CircleShape Small;

    // Bullet template
    sf::CircleShape Bullet;

    // Methods
    void processEvents();
    void update();
    void render();

public:
    // Constructor
    Game();

    // run method
    void run();

};

#endif
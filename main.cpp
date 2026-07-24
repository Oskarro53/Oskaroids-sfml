#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>

float f_rand_positive(const float min, const float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / (max - min));
}

float f_rand(const float min, const float max) {
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

struct Asteroid {
    sf::CircleShape asteroid;
    sf::Vector2f velocity;
    float size;
};

int main(){
    srand(std::time(nullptr));
    // ---Game variables---
    int hp = 3;
    bool paused = false;
    bool collisions = false;
    int safety_timer = 900;

    unsigned int window_width = 700;
    unsigned int window_height = 700;
    // Window declaration
    sf::RenderWindow window(sf::VideoMode({window_width, window_height}), "First window", sf::Style::Default);
    window.setFramerateLimit(300);

    // Player initialisation
    float player_size = 20.f; // 40x40 square
    sf::CircleShape player(player_size);
    player.setFillColor(sf::Color(0, 255, 0));
    player.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
    sf::FloatRect rc = player.getGlobalBounds();
    player.setOrigin(rc.height / 2.f, rc.width / 2.f);
    // Player properties
    sf::Vector2f velocity(0.f, 0.f);
    float thrust = 0.003f; // engine power
    float rotation_speed = 0.8f;
    float friction = 0.998f; // "air" resistance
    //float max_speed = 2.f;

    // Asteroids templates
    sf::CircleShape Big(1.8f * player_size); // Big asteroid
    Big.setFillColor(sf::Color::White);
    Big.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
    rc = Big.getGlobalBounds();
    Big.setOrigin(rc.width / 2.f, rc.height / 2.f);

    sf::CircleShape Med(0.9f * player_size); // Medium asteroid
    Med.setFillColor(sf::Color::White);
    Med.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
    rc = Med.getGlobalBounds();
    Med.setOrigin(rc.width / 2.f, rc.height / 2.f);

    sf::CircleShape Small(2 * player_size); // Small asteroid
    Small.setFillColor(sf::Color::White);
    Small.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
    rc = Small.getGlobalBounds();
    Small.setOrigin(rc.width / 2.f, rc.height / 2.f);

    // Vector of asteroids
    std::vector <Asteroid> asteroids;

    // ---GAME LOOP---
    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            // Dealing with events
            switch (event.type) {
                // Closing window
                case sf::Event::Closed:
                    window.close();
                    break;
                    // Others...

                default:
                    break;
            }
        }

        // ---Pressing keys---
        // Rotating the player
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player.rotate(-rotation_speed); // rotation left
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player.rotate(rotation_speed); // rotation right
        }

        sf::Vector2f acceleration(0.f, 0.f); // vector of acceleration
        // Using engine
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            float radians = player.getRotation() * 3.14159265359f / 180.f;
            acceleration.x = std::cos(radians) * thrust;
            acceleration.y = std::sin(radians) * thrust;
        }
        // Velocity
        velocity += acceleration;
        velocity *= friction;

        // Adding new asteroids
        if (asteroids.empty()) {
            for (int i = 0; i < 5; i++) {
                asteroids.push_back(
                {Big, {f_rand(-0.4f, 0.4f), f_rand(-0.4f, 0.4f)},
                    1.8f * player_size});
                asteroids[asteroids.size() - 1].asteroid.setPosition(static_cast<float>(rand() % (window_width + 1)),
                                                                     static_cast<float>(rand() % (window_height + 1)));
            }
        }

        // ---Clearing window---
        window.clear(sf::Color::Black);

        // ---Drawing and moving---
        player.move(velocity); // moving player

        // Checking safety of the player
        if (safety_timer != 0) {
            safety_timer--;
            collisions = false;
            player.setFillColor(sf::Color(0, 0, 255));
        }
        else {
            collisions = true;
            player.setFillColor(sf::Color(0, 255, 0));
        }

        // Reaching the map boundaries
        if (player.getPosition().x > window.getSize().x + player_size) {
            player.setPosition(player.getPosition().x - window.getSize().x - (2 * player_size), player.getPosition().y);
        }
        if (player.getPosition().x < -player_size) {
            player.setPosition(player.getPosition().x + window.getSize().x + (2 * player_size), player.getPosition().y);
        }
        if (player.getPosition().y > window.getSize().y + player_size) {
            player.setPosition(player.getPosition().x, player.getPosition().y - window.getSize().y - (2 * player_size));
        }
        if (player.getPosition().y < -player_size) {
            player.setPosition(player.getPosition().x, player.getPosition().y + window.getSize().y + (2 * player_size));
        }

        // Moving asteroids
        for (int i = 0; i < asteroids.size(); i++) {
            asteroids[i].asteroid.move(asteroids[i].velocity);
            // Reaching the map boundaries
            if (asteroids[i].asteroid.getPosition().x > window.getSize().x + asteroids[i].size) {
                asteroids[i].asteroid.setPosition(
                    asteroids[i].asteroid.getPosition().x - window.getSize().x - (2 * asteroids[i].size),
                    asteroids[i].asteroid.getPosition().y);
            }
            if (asteroids[i].asteroid.getPosition().x < -asteroids[i].size) {
                asteroids[i].asteroid.setPosition(
                    asteroids[i].asteroid.getPosition().x + window.getSize().x + (2 * asteroids[i].size),
                    asteroids[i].asteroid.getPosition().y);
            }
            if (asteroids[i].asteroid.getPosition().y > window.getSize().y + asteroids[i].size) {
                asteroids[i].asteroid.setPosition(asteroids[i].asteroid.getPosition().x,
                                                  asteroids[i].asteroid.getPosition().y - window.getSize().y - (
                                                      2 * asteroids[i].size));
            }
            if (asteroids[i].asteroid.getPosition().y < -asteroids[i].size) {
                asteroids[i].asteroid.setPosition(asteroids[i].asteroid.getPosition().x,
                                                  asteroids[i].asteroid.getPosition().y + window.getSize().y + (
                                                      2 * asteroids[i].size));
            }
            // Collision with player
            if (collisions) {
                float d = std::sqrt(
                    (player.getPosition().x - asteroids[i].asteroid.getPosition().x) * (
                        player.getPosition().x - asteroids[i].asteroid.getPosition().x) + (
                        player.getPosition().y - asteroids[i].asteroid.getPosition().y) * (
                        player.getPosition().y - asteroids[i].asteroid.getPosition().y));
                if (d <= player_size + asteroids[i].size) {
                    hp--;
                    player.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
                    player.setRotation(0.f);
                    velocity = {0.f, 0.f};
                    collisions = false;
                    safety_timer = 900;
                }
            }
        }

        window.draw(player);
        for (const auto& i : asteroids) {
            window.draw(i.asteroid);
        }
        // Refreshing the window
        window.display();
    }

    return 0;
}
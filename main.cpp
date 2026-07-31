#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>

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

int main(){
    srand(std::time(nullptr));
    // ---Game variables---
    int hp = 3;
    bool paused = false;
    bool collisions = false;
    int safety_timer = 900;
    float bullet_size = 2.f;
    int shot_cooldown = 120;
    float bullet_speed = 1.f;
    int score = 0;

    // ---Game menu variables---
    bool continue_pressed = true, exit_pressed = false;

    // ---Eng of game menu variables---
    bool p_again_pressed = true, finish_pressed = false;

    unsigned int window_width = 700;
    unsigned int window_height = 700;
    // Window declaration
    sf::RenderWindow window(sf::VideoMode({window_width, window_height}), "First window", sf::Style::Default);
    window.setFramerateLimit(300);

    // Loading font
    sf::Font font;
    if (!font.loadFromFile("BlackOpsOne-Regular.ttf")) {
        std::cout << "Font loading error";
        return 0;
    }

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

    sf::CircleShape Small(0.45f * player_size); // Small asteroid
    Small.setFillColor(sf::Color::White);
    Small.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
    rc = Small.getGlobalBounds();
    Small.setOrigin(rc.width / 2.f, rc.height / 2.f);

    // Vector of asteroids
    std::vector <Asteroid> asteroids;

    // Bullet template
    sf::CircleShape Bullet(bullet_size);
    Bullet.setFillColor(sf::Color::Red);
    rc = Bullet.getGlobalBounds();
    Bullet.setOrigin(rc.width / 2.f, rc.height / 2.f);

    // Vector of projectiles
    std::vector <Projectile> projectiles;

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

                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Escape) {
                        paused = !paused;
                    }
                    break;

                default:
                    break;
            }
        }
        // ---GAME---
        if (hp > 0) {
            if (!paused) {
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

                // Shooting
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && shot_cooldown == 0) {
                    float radians = player.getRotation() * 3.14159265359f / 180.f;
                    float sx = std::cos(radians) * bullet_speed;
                    float sy = std::sin(radians) * bullet_speed;
                    projectiles.push_back({Bullet, {sx, sy}});
                    projectiles[projectiles.size() - 1].bullet.setPosition(player.getPosition());
                    projectiles[projectiles.size() - 1].bullet.move(std::cos(radians) * player_size,
                        std::sin(radians) * player_size);
                    shot_cooldown = 120;
                }

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

                // ---Moving sprites---
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

                // Shot cooldown
                if (shot_cooldown != 0) {
                    shot_cooldown--;
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

                // Moving projectiles
                for (int i = 0; i < projectiles.size(); i++) {
                    if (projectiles[i].range != 0) {
                        projectiles[i].bullet.move(projectiles[i].velocity);
                        projectiles[i].range--;

                        // Reaching the map boundaries
                        if (projectiles[i].bullet.getPosition().x > window.getSize().x + bullet_size) {
                            projectiles[i].bullet.setPosition(
                                projectiles[i].bullet.getPosition().x - window.getSize().x - (2 * bullet_size),
                                projectiles[i].bullet.getPosition().y);
                        }
                        if (projectiles[i].bullet.getPosition().x < -bullet_size) {
                            projectiles[i].bullet.setPosition(
                                projectiles[i].bullet.getPosition().x + window.getSize().x + (2 * bullet_size),
                                projectiles[i].bullet.getPosition().y);
                        }
                        if (projectiles[i].bullet.getPosition().y > window.getSize().y + bullet_size) {
                            projectiles[i].bullet.setPosition(projectiles[i].bullet.getPosition().x,
                                                              projectiles[i].bullet.getPosition().y - window.getSize().y - (
                                                                  2 * bullet_size));
                        }
                        if (projectiles[i].bullet.getPosition().y < -bullet_size) {
                            projectiles[i].bullet.setPosition(projectiles[i].bullet.getPosition().x,
                                                              projectiles[i].bullet.getPosition().y + window.getSize().y + (
                                                                  2 * bullet_size));
                        }
                    }
                    else {
                        projectiles.erase(projectiles.begin() + i);
                    }
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

                    // Collision with projectiles
                    for (int j = 0; j < projectiles.size(); j++) {
                        float d_projectile = std::sqrt((projectiles[j].bullet.getPosition().x - asteroids[i].asteroid.getPosition().x) * (
                                projectiles[j].bullet.getPosition().x - asteroids[i].asteroid.getPosition().x) + (
                                projectiles[j].bullet.getPosition().y - asteroids[i].asteroid.getPosition().y) * (
                                projectiles[j].bullet.getPosition().y - asteroids[i].asteroid.getPosition().y));
                        if (d_projectile <= bullet_size + asteroids[i].size) {
                            projectiles.erase(projectiles.begin() + j);
                            score += 20;
                            sf::Vector2f old_position = asteroids[i].asteroid.getPosition();
                            switch (asteroids[i].type) {
                                case 0:
                                    asteroids.erase(asteroids.begin() + i);
                                    asteroids.push_back({Med, {f_rand(-0.4f, 0.4f), f_rand(-0.4f, 0.4f)},
                                        0.9f * player_size});
                                    asteroids[asteroids.size() - 1].type = MEDIUM;
                                    asteroids[asteroids.size() - 1].asteroid.setRotation(f_rand(0.f, 360.f));
                                    asteroids[asteroids.size() - 1].asteroid.setPosition(old_position);
                                    asteroids.push_back({Med, {f_rand(-0.4f, 0.4f), f_rand(-0.4f, 0.4f)},
                                        0.9f * player_size});
                                    asteroids[asteroids.size() - 1].type = MEDIUM;
                                    asteroids[asteroids.size() - 1].asteroid.setRotation(f_rand(0.f, 360.f));
                                    asteroids[asteroids.size() - 1].asteroid.setPosition(old_position);
                                    break;

                                case 1:
                                    asteroids.erase(asteroids.begin() + i);
                                    asteroids.push_back({Small, {f_rand(-0.4f, 0.4f), f_rand(-0.4f, 0.4f)},
                                        0.45f * player_size});
                                    asteroids[asteroids.size() - 1].type = SMALL;
                                    asteroids[asteroids.size() - 1].asteroid.setRotation(f_rand(0.f, 360.f));
                                    asteroids[asteroids.size() - 1].asteroid.setPosition(old_position);
                                    asteroids.push_back({Small, {f_rand(-0.4f, 0.4f), f_rand(-0.4f, 0.4f)},
                                        0.45f * player_size});
                                    asteroids[asteroids.size() - 1].type = SMALL;
                                    asteroids[asteroids.size() - 1].asteroid.setRotation(f_rand(0.f, 360.f));
                                    asteroids[asteroids.size() - 1].asteroid.setPosition(old_position);

                                case 2:
                                    asteroids.erase(asteroids.begin() + i);
                                    break;

                                default:
                                    break;
                            }
                        }
                    }

                    // Collision with player
                    if (collisions) {
                        float d_player = std::sqrt(
                            (player.getPosition().x - asteroids[i].asteroid.getPosition().x) * (
                                player.getPosition().x - asteroids[i].asteroid.getPosition().x) + (
                                player.getPosition().y - asteroids[i].asteroid.getPosition().y) * (
                                player.getPosition().y - asteroids[i].asteroid.getPosition().y));
                        if (d_player <= player_size + asteroids[i].size) {
                            hp--;
                            player.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
                            player.setRotation(0.f);
                            velocity = {0.f, 0.f};
                            collisions = false;
                            safety_timer = 900;
                        }
                    }
                }

                //---Drawing---
                window.draw(player);
                for (const auto& i : asteroids) {
                    window.draw(i.asteroid);
                }
                for (const auto& i : projectiles) {
                    window.draw(i.bullet);
                }
                // Refreshing the window
                window.display();
            }
            //---PAUSE MENU--
            else {
                window.clear(sf::Color::Black);
                sf::Text continue_button;
                continue_button.setFont(font);
                continue_button.setCharacterSize(40);
                continue_button.setFillColor(sf::Color::White);
                continue_button.setStyle(sf::Text::Bold);
                continue_button.setString("Continue");
                continue_button.setPosition(window.getSize().x / 2.f - 20.f, window.getSize().y / 2.f - 20.f);
                sf::Text exit_button;
                exit_button.setFont(font);
                exit_button.setCharacterSize(40);
                exit_button.setFillColor(sf::Color::White);
                exit_button.setStyle(sf::Text::Bold);
                exit_button.setString("Exit");
                exit_button.setPosition(window.getSize().x / 2.f - 20.f, window.getSize().y / 2.f + 20.f);

                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up) {
                        continue_pressed = true;
                        exit_pressed = false;
                    }
                    else if (event.key.code == sf::Keyboard::Down) {
                        continue_pressed = false;
                        exit_pressed = true;
                    }
                    else if (event.key.code == sf::Keyboard::Enter) {
                        if (exit_pressed) {
                            window.close();
                        }
                        else {
                            paused = false;
                        }
                    }
                }

                if (continue_pressed) {
                    continue_button.setFillColor(sf::Color::Green);
                    exit_button.setFillColor(sf::Color::White);
                }
                else {
                    exit_button.setFillColor(sf::Color::Red);
                    continue_button.setFillColor(sf::Color::White);
                }

                window.draw(exit_button);
                window.draw(continue_button);
                window.display();
            }
        }
        // ---End of game menu---
        else {
            window.clear(sf::Color::Black);
                sf::Text play_again_button;
                play_again_button.setFont(font);
                play_again_button.setCharacterSize(40);
                play_again_button.setFillColor(sf::Color::White);
                play_again_button.setStyle(sf::Text::Bold);
                play_again_button.setString("Play again");
                play_again_button.setPosition(window.getSize().x / 2.f - 20.f, window.getSize().y / 2.f - 20.f);
                sf::Text exit_button;
                exit_button.setFont(font);
                exit_button.setCharacterSize(40);
                exit_button.setFillColor(sf::Color::White);
                exit_button.setStyle(sf::Text::Bold);
                exit_button.setString("Exit");
                exit_button.setPosition(window.getSize().x / 2.f - 20.f, window.getSize().y / 2.f + 20.f);

                sf::Text Score;
                Score.setFont(font);
                Score.setCharacterSize(70);
                Score.setFillColor(sf::Color::White);
                Score.setStyle(sf::Text::Bold);
                Score.setString("Score: " + std::to_string(score));
                Score.setPosition(window.getSize().x / 2.f - 70.f, window.getSize().y / 2.f - 180.f);

                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up) {
                        p_again_pressed = true;
                        finish_pressed = false;
                    }
                    else if (event.key.code == sf::Keyboard::Down) {
                        p_again_pressed = false;
                        finish_pressed = true;
                    }
                    else if (event.key.code == sf::Keyboard::Enter) {
                        if (finish_pressed) {
                            window.close();
                        }
                        else {
                            score = 0;
                            hp = 3;
                            paused = false;
                            safety_timer = 900;
                            shot_cooldown = 120;
                            player.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
                            asteroids.clear();
                            projectiles.clear();
                        }
                    }
                }

                if (p_again_pressed) {
                    play_again_button.setFillColor(sf::Color::Green);
                    exit_button.setFillColor(sf::Color::White);
                }
                else {
                    exit_button.setFillColor(sf::Color::Red);
                    play_again_button.setFillColor(sf::Color::White);
                }

                window.draw(Score);
                window.draw(exit_button);
                window.draw(play_again_button);
                window.display();
        }
    }

    return 0;
}
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>

float f_rand(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / (max - min));
}

struct Asteroid {
    sf::CircleShape asteroid;
    sf::Vector2f velocity;
};

int main(){
    srand(std::time(nullptr));

    unsigned int window_width = 800;
    unsigned int window_heigth = 800;
    // Deklaracja klasy okno
    sf::RenderWindow window(sf::VideoMode({window_width, window_heigth}), "First window", sf::Style::Default);

    // Inicjalizacja glownej postaci
    float player_size = 20.f; // 40x40 square
    sf::CircleShape player(player_size);
    player.setFillColor(sf::Color(0, 255, 0));
    player.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
    sf::FloatRect rc = player.getGlobalBounds();
    player.setOrigin(rc.height / 2.f, rc.width / 2.f);
    // Wlasciwosci glownej postaci
    sf::Vector2f velocity(0.f, 0.f); // prędkość (wzdłóż współrzędnych)
    float thrust = 0.001f; // siła silnika
    float rotation_speed = 0.2f; // prędkość silnika
    float friction = 0.998f; // opór "powietrza"
    float max_speed = 2.f;

    // Szablony asteroid
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

    // Deklaracja wektora asteroid
    std::vector <Asteroid> asteroids;

    // Glowna petla
    while(window.isOpen()) {
        // Tworzenie obiektu zdarzenia
        sf::Event event;
        while(window.pollEvent(event)) {
            // Obsluga zdarzen
            switch (event.type) {
                // Zamknięcie okna
                case sf::Event::Closed:
                    window.close();
                    break;
                    // Pozostale...

                default:
                    break;
            }
        }
        // Ruch postaci
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player.rotate(-rotation_speed); //obrót w lewo
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player.rotate(rotation_speed); // obrót w prawo
        }

        sf::Vector2f acceleration(0.f, 0.f); // przyspieszenie
        // Przyspieszenie
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            float radians = player.getRotation() * 3.14159265359f / 180.f;
            acceleration.x = std::cos(radians) * thrust;
            acceleration.y = std::sin(radians) * thrust;
        }
        // Prędkość
        velocity += acceleration;
        velocity *= friction;

        // Asteroidy
        if (asteroids.empty()) {
            for (int i = 0; i < 5; i++) {
                asteroids.push_back({Big, {f_rand(0.5f, 2.f), f_rand(0.5f, 2.f)}});
                asteroids[asteroids.size() - 1].asteroid.setPosition(static_cast<float>(rand() % (window_width + 1)),
                                                                     static_cast<float>(rand() % (window_heigth + 1)));
            }
        }

        // Czyszczenie okna
        window.clear(sf::Color::Black);
        // draw everything here...
        // window.draw(...);
        player.move(velocity); // przemieszczenia gracza
        // Interakcja z krawędziami mapy
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

        window.draw(player);
        for (int i = 0; i < asteroids.size(); i++) {
            window.draw(asteroids[i].asteroid);
        }
        // Odswiezenie obrazu
        window.display();
    }

    return 0;
}
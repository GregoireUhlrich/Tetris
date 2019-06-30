#include <iostream>
#include <sstream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "block.h"

using namespace std;

constexpr int GAME_THICKNESS = 5;
constexpr int SCORE_THICKNESS = 100;

int main() {

    srand(time(0));
    sf::RenderWindow window(
           sf::VideoMode(
               DEFAULT_SIZE*X_GAME+2*GAME_THICKNESS
                   -DEFAULT_THICKNESS,
               DEFAULT_SIZE*Y_GAME+2*GAME_THICKNESS+SCORE_THICKNESS
                   -DEFAULT_THICKNESS),
            "TETRIS");
    TetrisGame game;

    sf::Clock clock;
    sf::RenderTexture gameTexture;
    gameTexture.create(DEFAULT_SIZE*X_GAME-DEFAULT_THICKNESS,
                       DEFAULT_SIZE*Y_GAME-DEFAULT_THICKNESS);
    sf::RectangleShape gameOutline(
            sf::Vector2f(DEFAULT_SIZE*X_GAME-DEFAULT_THICKNESS,
                         DEFAULT_SIZE*Y_GAME-DEFAULT_THICKNESS));
    gameOutline.setPosition(GAME_THICKNESS, GAME_THICKNESS+SCORE_THICKNESS);
    gameOutline.setOutlineColor(sf::Color::White);
    gameOutline.setFillColor(sf::Color::Transparent);
    gameOutline.setOutlineThickness(GAME_THICKNESS);

    sf::Font font;
    font.loadFromFile(
"/home/uhlrich/.local/share/fonts/Ubuntu Mono derivative Powerline Bold.ttf");

    sf::Text textScore(sf::String("Score: "), font, 40);
    textScore.setFillColor(sf::Color::White);
    textScore.setPosition(50, 25);

    sf::Text textTime(sf::String("Time: "), font, 40);
    textTime.setFillColor(sf::Color::White);
    textTime.setPosition(370, 25);

    sf::SoundBuffer buffer;
    sf::Sound sound;
    buffer.loadFromFile("Tetris.ogg");
    sound.setBuffer(buffer);
    sound.setLoop(true);
    sound.play();

    while(true) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }      
            else
                game.handleEvent(event);
        }
        if (not window.isOpen())
            break;
        window.clear(sf::Color::Black);
        double elapsedTime = clock.restart().asSeconds();
        gameTexture.clear(sf::Color::Black);
        game.draw(&gameTexture, elapsedTime);
        gameTexture.display();
        sf::Sprite gameSprite;
        gameSprite.setTexture(gameTexture.getTexture());
        gameSprite.setPosition(GAME_THICKNESS, GAME_THICKNESS+SCORE_THICKNESS);

        ostringstream sout;
        sout << "Score : " << game.getScore();
        textScore.setString(sout.str());

        ostringstream sout2;
        sout2 << "Time : " << floor(game.getTotalTime());
        textTime.setString(sout2.str());

        window.draw(gameOutline);
        window.draw(gameSprite);
        window.draw(textScore);
        window.draw(textTime);
        window.display();
    }
    
    return 0;
}

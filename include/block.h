#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-const-variable"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <array>
#include <cmath>
#include <cstdlib>
#include <string>
#include <sstream>

enum TypeShape {
    Square,
    Line,
    Cross,
    AngleL,
    AngleR,
    StepL,
    StepR
};

static int getRotationCenter(TypeShape type)
{
#pragma GCC diagnostic ignored "-Wreturn-type"
    switch(type) {
        case Square: 
            return -1; 
        case Line  :
        case Cross : 
        case StepL : 
        case StepR : 
            return 1;
        case AngleL:
        case AngleR:
            return 2;
    }
}

static TypeShape randomTypeShape()
{
    int random = rand()%7;
    return (TypeShape)random;
}

static sf::Color randomColor()
{
    int random = rand()%6;
    switch(random) {
        case 0: return sf::Color::Blue;    break;
        case 1: return sf::Color::Green;   break;
        case 2: return sf::Color::Red;     break;
        case 3: return sf::Color::Yellow;  break;
        case 4: return sf::Color::Cyan;    break;
        case 5: return sf::Color::Magenta; break;
        default: 
                return sf::Color::Blue;
    }
}

constexpr static const int DEFAULT_SIZE      = 40;
constexpr static const int DEFAULT_THICKNESS = 3;
constexpr static const int X_GAME            = 16;
constexpr static const int Y_GAME            = 20;

class TetrisShape {

    public:

    TetrisShape(){};

    TetrisShape(TypeShape type, int t_size = DEFAULT_SIZE);

    ~TetrisShape(){};

    std::vector<sf::RectangleShape> getShapes() const;

    int getXMin() const;

    int getXMax() const;

    int getYMin() const;

    void drawMiniature(sf::RenderTarget* target) const;

    void setPosition(int x, int y);

    void setFillColor(sf::Color color);

    void moveLeft();

    void moveRight();

    void moveDown();

    void rotate(bool rightRotation);

    void reset(TypeShape type);

    void draw(sf::RenderTarget* target, float elapsedTime);

    private:

    int size;
    TypeShape type;
    std::array<sf::RectangleShape, 4> blocks;
};

int convertPos(float pos);

float invConvertPos(int pos);


class TetrisGame {

    public:

    TetrisGame();

    ~TetrisGame();

    float getTotalTime() const;

    int getScore() const;

    void setSound(sf::Sound* sound);

    void handleEvent(sf::Event event);

    void draw(sf::RenderTarget* target, float elapsedTime);

    protected:

    float getFactor() const;

    void placeMovingShape();

    bool shapesOverlap(const sf::RectangleShape& A,
                       const sf::RectangleShape& B) const;

    bool leftMoveLegit() const;

    bool rightMoveLegit() const;

    bool downMoveLegit() const;

    bool freeMovingShape() const;

    void removeFullLines();

    void setBackup();

    void endGame();

    void update(float elapsedTime);

    private:

    bool game = true;
    bool running = true;

    int shapeSize;
    sf::Vector2i gameSize;
    bool backup = false;
    bool backupUsed = false;
    sf::RectangleShape outlineBackup;
    sf::RenderTexture* backupTexture;
    sf::Sprite backupSprite;
    TetrisShape backupShape;
    TetrisShape movingShape;
    std::vector<sf::RectangleShape> shapes;
    sf::Sound* sound;

    float time = 0;
    float thresholdMove = 0.3;
    float effThreshold = thresholdMove;

    float totalTime = 0;
    int   score = 0;
};


#endif

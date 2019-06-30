#include "block.h"

using namespace std;

int ColorGenerator::index = -1;

TetrisShape::TetrisShape(TypeShape t_type, int t_size)
    :size(t_size), type(t_type)
{
    for (auto& b : blocks) {
        b = sf::RectangleShape(sf::Vector2f(t_size-DEFAULT_THICKNESS,
                                            t_size-DEFAULT_THICKNESS));
        b.setOutlineThickness(DEFAULT_THICKNESS);
        b.setOutlineColor(sf::Color::White);
    }

    for (auto& b : blocks)
        b.setPosition(0,0);
    switch(type) {
        case Square:
        blocks[1].move(0,t_size);
        blocks[2].move(t_size,0);
        blocks[3].move(t_size,t_size);
        break;

        case Line:
        blocks[1].move(0,t_size);
        blocks[2].move(0,2*t_size);
        blocks[3].move(0,3*t_size);
        break;

        case Cross:
        blocks[1].move(t_size,0);
        blocks[2].move(2*t_size,0);
        blocks[3].move(t_size,t_size);
        break;

        case AngleR:
        blocks[1].move(t_size,0);
        blocks[2].move(t_size,t_size);
        blocks[3].move(t_size,2*t_size);
        break;

        case AngleL:
        blocks[0].move(t_size,0);
        blocks[1].move(0,0);
        blocks[2].move(0,t_size);
        blocks[3].move(0,2*t_size);
        break;

        case StepL:
        blocks[0].move(t_size,0);
        blocks[1].move(t_size,t_size);
        blocks[2].move(0,t_size);
        blocks[3].move(0,2*t_size);
        break;

        case StepR:
        blocks[1].move(0,t_size);
        blocks[2].move(t_size,t_size);
        blocks[3].move(t_size,2*t_size);
        break;
    }

    setFillColor(ColorGenerator::randomColor());
}

void TetrisShape::reset()
{
    *this = TetrisShape(randomTypeShape(), size);
}

vector<sf::RectangleShape> TetrisShape::getShapes() const
{
    vector<sf::RectangleShape> res;
    for (const auto& b : blocks)
        res.push_back(b);
    return res;
}

int TetrisShape::getXMin() const
{
    int xmin = blocks[0].getPosition().x;
    for (const auto& b : blocks)
        if (b.getPosition().x < xmin)
            xmin = b.getPosition().x;

    return round(xmin/blocks[0].getSize().x);
}

int TetrisShape::getXMax() const
{
    int xmax = blocks[0].getPosition().x;
    for (const auto& b : blocks)
        if (b.getPosition().x > xmax)
            xmax = b.getPosition().x;

    return round(xmax/blocks[0].getSize().x);
}

int TetrisShape::getYMin() const
{
    int ymin = blocks[0].getPosition().y;
    for (const auto& b : blocks)
        if (b.getPosition().y < ymin)
            ymin = b.getPosition().y;

    return round(ymin/blocks[0].getSize().y);
}

void TetrisShape::drawMiniature(sf::RenderTarget* target) const
{
    int x;
    int y = 0;
    switch(type) {
        case Square:
        case Line:
        case Cross:
        case AngleR:
            x = 0;
            break;
        case AngleL:
        case StepR:
        case StepL:
            x = 1;
            break;
    }
    TetrisShape other(*this);
    other.setPosition(x, y+1);
    other.draw(target, 0);
}

void TetrisShape::setPosition(int x, int y)
{
    int deltaX = x*size - blocks[0].getPosition().x;
    int deltaY = y*size - blocks[0].getPosition().y;
    for (auto& b : blocks)
        b.move(deltaX, deltaY);
}

void TetrisShape::setFillColor(sf::Color color)
{
    for (auto& block : blocks)
        block.setFillColor(color);
}

void TetrisShape::setGhostMode(bool mode)
{
    if (mode) {
        for (auto& b : blocks) {
            b.setOutlineColor(b.getFillColor());
            b.setFillColor(sf::Color::Transparent);
        }
    }
    else {
        for (auto& b : blocks) {
            b.setFillColor(b.getOutlineColor());
            b.setOutlineColor(sf::Color::White);
        }
    }
}

void TetrisShape::moveLeft()
{
    for (auto& b : blocks)
        b.move(-size, 0);
}

void TetrisShape::moveRight()
{
    for (auto& b : blocks)
        b.move(size, 0);
}

void TetrisShape::moveDown() 
{
    for (auto& b : blocks)
        b.move(0,size);
}

void TetrisShape::rotate(bool rightRotation)
{
    int center = getRotationCenter(type);
    if (center == -1)
        return;
    int x = convertPos(blocks[center].getPosition().x);
    int y = convertPos(blocks[center].getPosition().y);
    for (int i = 0; i != 4; ++i) {
        if (i == center)
            continue;
        int xB = convertPos(blocks[i].getPosition().x);
        int yB = convertPos(blocks[i].getPosition().y);
        int xRel = xB - x;
        int yRel = yB - y;
        swap(xRel, yRel);
        if (rightRotation) 
            yRel = -yRel;
        else
            xRel = -xRel;
        blocks[i].setPosition(invConvertPos(x + xRel),
                              invConvertPos(y + yRel));
    }
}

void TetrisShape::draw(sf::RenderTarget* target, float elapsedTime)
{
    for (auto& b : blocks) 
        target->draw(b);
}

int convertPos(float pos) 
{
    return pos / DEFAULT_SIZE;
}

float invConvertPos(int pos)
{
    return pos * DEFAULT_SIZE;
}

///////////////////////////////////////////////////
/*************************************************/
// TetrisGame class                              //
/*************************************************/
///////////////////////////////////////////////////

TetrisGame::TetrisGame(): shapeSize(DEFAULT_SIZE), gameSize(X_GAME, Y_GAME),
    nextShape(randomTypeShape()), movingShape(randomTypeShape())
{
    font.loadFromFile(
"/home/uhlrich/.local/share/fonts/Ubuntu Mono derivative Powerline Bold.ttf");
    backupText = sf::Text("Backup", font, 25);
    backupText.setFillColor(sf::Color::White);
    nextText = sf::Text("Next", font, 25);
    nextText.setFillColor(sf::Color::White);
    backupText.setPosition(3,3);
    nextText.setPosition(3,3);
    backupTexture = new sf::RenderTexture;
    backupTexture->create(3*shapeSize, 4*shapeSize+shapeSize);
    nextTexture = new sf::RenderTexture;
    nextTexture->create(3*shapeSize, 4*shapeSize+shapeSize);
    outlineBackup = sf::RectangleShape(sf::Vector2f(3*shapeSize,
                4*shapeSize+shapeSize));
    outlineBackup.setFillColor(sf::Color::Transparent);
    outlineBackup.setOutlineColor(sf::Color(128,128,128,128));
    outlineBackup.setOutlineThickness(3);
    outlineBackup.setPosition(gameSize.x*shapeSize-3*shapeSize-10,10);
    outlineNext = sf::RectangleShape(sf::Vector2f(3*shapeSize,
                4*shapeSize+shapeSize));
    outlineNext.setFillColor(sf::Color::Transparent);
    outlineNext.setOutlineColor(sf::Color(128,128,128,128));
    outlineNext.setOutlineThickness(3);
    outlineNext.setPosition(10,10);
    placeMovingShape();
    ghostShape.setGhostMode(true);
    updateBackup();
    updateGhost();
    updateNext();
}

TetrisGame::~TetrisGame()
{
    delete backupTexture;
    delete nextTexture;
}

float TetrisGame::getTotalTime() const
{
    return totalTime;
}

int TetrisGame::getScore() const
{
    return score + floor(totalTime/60)*250;
}

int TetrisGame::getLine() const
{
    return line;
}

void TetrisGame::setSound(sf::Sound* t_sound)
{
    sound = t_sound;
}

float TetrisGame::getFactor() const
{
    return pow(1.10, -floor(getScore()/1000));
}

void TetrisGame::placeMovingShape()
{
    int x = round(gameSize.x / 2);
    int y = 0;
    movingShape.setPosition(x, y);
}

bool TetrisGame::shapesOverlap(const sf::RectangleShape& A,
                               const sf::RectangleShape& B) const
{
    return (round(A.getPosition().x) == round(B.getPosition().x)
            and round(A.getPosition().y) == round(B.getPosition().y));
}

bool TetrisGame::leftMoveLegit() const
{
    vector<sf::RectangleShape> moving(movingShape.getShapes());
    for (auto& rectangle : moving) {
        rectangle.move(-shapeSize, 0);
        if (rectangle.getPosition().x < 0)
            return false;
        for (const auto& staticRectangle : shapes)
            if (shapesOverlap(rectangle, staticRectangle))
                return false;
    }
    return true;
}

bool TetrisGame::rightMoveLegit() const
{
    vector<sf::RectangleShape> moving(movingShape.getShapes());
    for (auto& rectangle : moving) {
        rectangle.move(shapeSize, 0);
        if (rectangle.getPosition().x >= gameSize.x*shapeSize)
            return false;
        for (const auto& staticRectangle : shapes)
            if (shapesOverlap(rectangle, staticRectangle))
                return false;
    }
    return true;
}

bool TetrisGame::downMoveLegit() const
{
    vector<sf::RectangleShape> moving(movingShape.getShapes());
    for (auto& rectangle : moving) {
        rectangle.move(0, shapeSize);
        if (rectangle.getPosition().y >= gameSize.y*shapeSize)
            return false;
        for (const auto& staticRectangle : shapes)
            if (shapesOverlap(rectangle, staticRectangle))
                return false;
    }
    return true;
}

bool TetrisGame::freeMovingShape() const
{
    for (const auto& rectA : movingShape.getShapes()) {
        int x = convertPos(rectA.getPosition().x);
        int y = convertPos(rectA.getPosition().y);
        if (x < 0 or y < 0 or x >= gameSize.x or y >= gameSize.y)
            return false;
        for (const auto& rectB : shapes)
            if (shapesOverlap(rectA, rectB))
                return false;
    }
    return true;
}

void TetrisGame::removeFullLines()
{
    vector<int> number(gameSize.y, 0);
    vector<sf::Color> colors(gameSize.y, sf::Color::Black);
    for (const auto& s : shapes) {
        int index = convertPos(s.getPosition().y);
        ++number[index];
        if (colors[index] == sf::Color::Black)
            colors[index] = s.getFillColor();
        else if (colors[index] != s.getFillColor())
            colors[index] = sf::Color::White;
    }

    int scoreBonus = 0;
    for (size_t full = 0; full != number.size(); ++full) {
        if (not (number[full] == gameSize.x))
            continue;
        // Here the line "full" is full
        if (colors[full] != sf::Color::White) {
            score += 5000;
            int ymin = gameSize.y;
            for (const auto& s : shapes)
                if (convertPos(s.getPosition().y) < ymin)
                    ymin = convertPos(s.getPosition().y);
            line += gameSize.y-ymin;
            shapes.clear();
            return;
        }
        scoreBonus += 250;
        ++line;
        for (size_t i = 0; i != shapes.size(); ++i) {
            size_t pos = convertPos(shapes[i].getPosition().y);
            if (pos == full) {
                shapes.erase(shapes.begin() + i);
                --i;
            }
            else if (pos < full) 
                shapes[i].move(0, shapeSize);
        }
    }
    score += scoreBonus;
}

void TetrisGame::sendNextShape()
{
    movingShape = nextShape;
    nextShape.reset();
    placeMovingShape();
    updateNext();
}

void TetrisGame::parkShape()
{
    const vector<sf::RectangleShape>& newShapes 
        = movingShape.getShapes();
    shapes.insert(shapes.end(), newShapes.begin(), newShapes.end());
    sendNextShape();
    backupUsed = false;
    removeFullLines();
    updateGhost();
    if (not freeMovingShape())
        endGame();
}

void TetrisGame::endGame() 
{
    game = false;
    running = false;
}

void TetrisGame::handleEvent(sf::Event event)
{
    if (not running) {
        if (event.type == sf::Event::KeyPressed
                and event.key.code == sf::Keyboard::Escape)
            running = true;
        else if (not game and event.type == sf::Event::KeyPressed
                and event.key.code == sf::Keyboard::N) {
            *this = TetrisGame();
        }
        return;
    }
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Left
                and leftMoveLegit()) {
            movingShape.moveLeft();
            updateGhost();
        }
        else if (event.key.code == sf::Keyboard::Right
                and rightMoveLegit()) {
            movingShape.moveRight();
            updateGhost();
        }
        else if (event.key.code == sf::Keyboard::A) {
            movingShape.rotate(false);
            if (not freeMovingShape()) 
                movingShape.rotate(true);
            else
                updateGhost();
        }
        else if (event.key.code == sf::Keyboard::Z) {
            movingShape.rotate(true);
            if (not freeMovingShape()) 
                movingShape.rotate(false);
            else 
                updateGhost();
        }
        else if (event.key.code == sf::Keyboard::Up 
                and not backupUsed) {
            backupUsed = true;
            if (not backup) {
                backup = true;
                backupShape = movingShape;
                sendNextShape();
            }
            else {
                swap(backupShape, movingShape);
                placeMovingShape();
            }
            updateBackup();
            updateGhost();
        }
        else if (event.key.code == sf::Keyboard::Escape) {
            running = false;
        }
        else if (event.key.code == sf::Keyboard::Space) {
            while (downMoveLegit()) {
                movingShape.moveDown();
                ++score;
            }
            parkShape();
        }
    }
}

void TetrisGame::update(float elapsedTime)
{
    bool move = false;
    bool downLegit = downMoveLegit();
    if (downLegit)
        time += elapsedTime;
    else
        time += elapsedTime / 3;

    totalTime += elapsedTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) 
        effThreshold = thresholdMove / 5;
    else
        effThreshold = thresholdMove;
    if (time > getFactor()*effThreshold) {
        time = 0;
        move = true;
    }
    if (move) {
        if (downLegit) {
            movingShape.moveDown();
            ++score;
        }
        else {
            parkShape();
        }
    }
}

void TetrisGame::draw(sf::RenderTarget* target, float elapsedTime)
{
    if (running)
        update(elapsedTime);
    for (const auto& s : shapes)
        target->draw(s);
    ghostShape.draw(target, elapsedTime);
    movingShape.draw(target, elapsedTime);
    target->draw(backupSprite);
    target->draw(outlineBackup);
    target->draw(nextSprite);
    target->draw(outlineNext);
    if (not running and game) {
        sf::Font font;
        font.loadFromFile(
    "/home/uhlrich/.local/share/fonts/Ubuntu Mono derivative Powerline Bold.ttf");
        sf::Text pause("Pause", font, 40);
        pause.setFillColor(sf::Color::White);
        pause.setOutlineColor(sf::Color::Black);
        pause.setOutlineThickness(3);
        sf::FloatRect rect = pause.getGlobalBounds();
        pause.setPosition(gameSize.x*shapeSize/2-rect.width/2,
                          gameSize.y*shapeSize/2-rect.height/2);
        target->draw(pause);
    }
    else if (not game) {
        sf::Font font;
        font.loadFromFile(
    "/home/uhlrich/.local/share/fonts/Ubuntu Mono derivative Powerline Bold.ttf");

        ostringstream sout;
        sout << "GAME OVER: " << getScore();
        sf::Text gameOver(sout.str(), font, 40);
        gameOver.setFillColor(sf::Color::White);
        gameOver.setOutlineColor(sf::Color::Black);
        gameOver.setOutlineThickness(3);
        sf::FloatRect rect = gameOver.getGlobalBounds();
        gameOver.setPosition(gameSize.x*shapeSize/2-rect.width/2,
                             gameSize.y*shapeSize/2-rect.height/2);
        target->draw(gameOver);
    }
}

void TetrisGame::updateBackup()
{
    backupTexture->clear(sf::Color::Transparent);
    backupShape.drawMiniature(backupTexture);
    backupTexture->draw(backupText);
    backupTexture->display();
    backupSprite.setTexture(backupTexture->getTexture());
    backupSprite.setPosition(10,10);
    backupSprite.setColor(sf::Color(255,255,255,128));
}

void TetrisGame::updateNext()
{
    nextTexture->clear(sf::Color::Transparent);
    nextShape.drawMiniature(nextTexture);
    nextTexture->draw(nextText);
    nextTexture->display();
    nextSprite.setTexture(nextTexture->getTexture());
    nextSprite.setPosition(gameSize.x*shapeSize-3*shapeSize-10,10);
    nextSprite.setColor(sf::Color(255,255,255,128));
}

void TetrisGame::updateGhost()
{
    ghostShape.setGhostMode(false);
    ghostShape = movingShape;
    while(downMoveLegit())
        movingShape.moveDown();
    swap(ghostShape, movingShape);
    ghostShape.setGhostMode(true);
}

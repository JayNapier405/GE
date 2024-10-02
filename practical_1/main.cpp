#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

//Controlls for game
const Keyboard::Key controls[4] = {
    Keyboard::A,   // Player1 UP
    Keyboard::Z,   // Player1 Down
    Keyboard::Up,  // Player2 UP
    Keyboard::Down // Player2 Down
};

//setting values for game objects
//.f indecates that a number is a float
const Vector2f paddleSize(25.f, 100.f);
const float ballRadius = 10.f;
const int gameWidth = 800;
const int gameHeight = 600;
const float paddleSpeed = 400.f;
const float paddleOffsetWall = 10.f;

//Shapes for objects
CircleShape ball;
RectangleShape paddles[2];

//Pt 3 starting ball values
Vector2f ballVelocity;
bool isPlayer1Serving = false;
const float initialVelocityX = 100.f;
const float initialVelocityY = 60.f;

const float velocityMultiplier = 1.1f;

//declare font and text objects
Font font;
Text text;

//player scores
int player1Score = 0;
int player2Score = 0;

void Load() {
    // For each paddle set size and origin
    for (auto& p : paddles) {
        p.setSize(paddleSize);
        p.setOrigin(paddleSize / 2.f); //This sets the paddles position to the center of it
    }

    // Set size and origin of ball
    ball.setRadius(ballRadius);
    ball.setOrigin(ballRadius,ballRadius); //Should be half the ball width and height

    // reset paddle position
    //paddleSize.x .x is used to accsess x positon
    paddles[0].setPosition(Vector2f(paddleOffsetWall + paddleSize.x / 2.f, gameHeight / 2.f));
    paddles[1].setPosition(Vector2f(gameWidth - paddleOffsetWall - paddleSize.x / 2.f, gameHeight / 2.f));
    // reset Ball Position
    ball.setPosition(Vector2f(gameWidth/2.f, gameHeight /2.f));

    //pt3 Inline statement where isPlayer1Serving is evaluated (before the ?)
    //if isPlayer1Serving is true then velocity withh be + of false it will be - (y velocity never changes)
    ballVelocity = { (isPlayer1Serving ? initialVelocityX : -initialVelocityX), initialVelocityY };

    // Load font-face from res dir
    // absolute path DO NOT USE
    //font.loadFromFile("C:/Users/larry/Desktop/GamesEngineering/GE/res/fonts/RobotoMono-Regular.ttf");
    //Issue loading fonts with relitive path cause by sln being in builds and not in GE/Practical 1 
    //This was literally the next step oops
    font.loadFromFile("../GE/res/fonts/RobotoMono-Regular.ttf");
    // Set text element to use font
    text.setFont(font);
    // set the character size to 24 pixels
    text.setCharacterSize(24);
}

//reset function
void Reset() {
    paddles[0].setPosition(Vector2f(paddleOffsetWall + paddleSize.x / 2.f, gameHeight / 2.f));
    paddles[1].setPosition(Vector2f(gameWidth - paddleOffsetWall - paddleSize.x / 2.f, gameHeight / 2.f));

    ball.setPosition(Vector2f(gameWidth / 2.f, gameHeight / 2.f));

    ballVelocity.x = 100.f;

    // Update Score Text
    text.setString(to_string(player1Score) + " : " + to_string(player2Score));
    // Keep Score Text Centered
    text.setPosition((gameWidth * .5f) - (text.getLocalBounds().width * .5f), 0);
}

void Update(RenderWindow& window) {
    // Reset clock, recalculate deltatime
    static Clock clock;
    float dt = clock.restart().asSeconds();
    // check and consume events
    Event event;
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed) {
            window.close();
            return;
        }
    }

    // Quit Via ESC Key
    if (Keyboard::isKeyPressed(Keyboard::Escape)) {
        window.close();
    }

    // paddle movement left
    float directionL = 0.0f;
    if (Keyboard::isKeyPressed(controls[0]) && paddles[0].getPosition().y > 0 + paddleSize.y * 0.5) {
        directionL--;
    }
    if (Keyboard::isKeyPressed(controls[1]) && paddles[0].getPosition().y < 600 - paddleSize.y * 0.5) {
        directionL++;
    }
    paddles[0].move(Vector2f(0.f, directionL * paddleSpeed * dt));

    // paddle movement right
    float directionR = 0.0f;
    if (Keyboard::isKeyPressed(controls[2]) && paddles[1].getPosition().y > 0 + paddleSize.y * 0.5) {
        directionR--;
    }
    if (Keyboard::isKeyPressed(controls[3]) && paddles[1].getPosition().y < 600 - paddleSize.y * 0.5) {
        directionR++;
    }
    paddles[1].move(Vector2f(0.f, directionR * paddleSpeed * dt));

    //pt3
    ball.move(ballVelocity * dt);

    // check ball collision
    const float bx = ball.getPosition().x;
    const float by = ball.getPosition().y;
    if (by > gameHeight) { //bottom wall
        // bottom wall
        ballVelocity.x *= velocityMultiplier;
        ballVelocity.y *= -velocityMultiplier;
        ball.move(Vector2f(0.f, -10.f));
    }
    else if (by < 0) { //top wall
        // top wall
        ballVelocity.x *= velocityMultiplier;
        ballVelocity.y *= -velocityMultiplier;
        ball.move(Vector2f(0.f, 10.f));
    }

    //collition check for left paddle
    else if (
        //ball is inline or behind paddle AND
        bx < paddleSize.x + paddleOffsetWall &&
        //ball is below top edge of paddle AND
        by > paddles[0].getPosition().y - (paddleSize.y * 0.5) &&
        //ball is above bottom edge of paddle
        by < paddles[0].getPosition().y + (paddleSize.y * 0.5))
    {
        //invert ball velocity and multiply by velocity multiplyer
        ballVelocity.x = -ballVelocity.x * velocityMultiplier;
    }
    else if (
        //checks where the ball is relivive to the x position of the paddle
        bx > gameWidth - paddleSize.x - paddleOffsetWall &&
        //checks that the ball is within the recangle of the paddle on the y axis
        //it is multiplied by 0.5 because the origin is at middle point of paddle i think?
        by > paddles[1].getPosition().y - (paddleSize.y * 0.5)&&
        by < paddles[1].getPosition().y + (paddleSize.y * 0.5)
        ) {
        // bounce off right paddle
        ballVelocity.x = -ballVelocity.x * velocityMultiplier;
    }



    //call reset if ball goes outside gamespace
    //code to increment score counter probably goes here
   else if (bx > gameWidth) {
        // right wall
        player1Score = player1Score + 1;
        Reset();
    }
    else if (bx < 0) {
        // left wall
        player2Score = player2Score + 1;
        Reset();
    }


}

void Render(RenderWindow& window) {
    // Draw Everything
    window.draw(paddles[0]);
    window.draw(paddles[1]);
    window.draw(ball);
    //render text
    window.draw(text);
}

int main() {
    RenderWindow window(VideoMode(gameWidth, gameHeight), "PONG");
    Load();
    while (window.isOpen()) {
        window.clear();
        Update(window);
        Render(window);
        window.display();
    }
    return 0;
}

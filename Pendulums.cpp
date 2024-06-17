#define _USE_MATH_DEFINES
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <limits>
#include <chrono>
#include <thread>

// Global variables
const double WINDOW_WIDTH = 1700;
const double WINDOW_HEIGHT = 800;
const double DT = 0.01;
const int NUM_DT = 3000;
double INIT_DEGREES = 45;
const double RADIUS = 15;
const double LENGTH = 300;
const double G = 5000; // Gravity
const double TRANSPARANCY = 100;

// Function to convert degrees to radians
double toRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

// Function to draw a line between two points
void drawLine(sf::RenderWindow& window, double x1, double y1, double x2, double y2, sf::Color color) {
    sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(x1, y1)),
        sf::Vertex(sf::Vector2f(x2, y2))
    };
    line[0].color = color;
    line[1].color = color;
    window.draw(line, 2, sf::Lines);
}

// Pendulum struct
struct Pendulum {
    double phi;
    double omega;
    double alpha;
    double init_phi;
    double init_omega;
    double init_alpha;
    sf::Color color;
    double xOffset;
    double yOffset;

    Pendulum(double initialAngle, sf::Color pendulumColor, double xPosition, double yPosition)
        : phi(toRadians(initialAngle)), omega(0), alpha(0), init_phi(toRadians(initialAngle)), init_omega(0), init_alpha(0),
        color(pendulumColor), xOffset(xPosition), yOffset(yPosition) {}

    void integrateRegular() {
        phi += omega * DT;
        omega += alpha * DT;
        alpha = -G / LENGTH * std::sin(phi);
    }

    void integrateSymplectic() {
        alpha = -G / LENGTH * std::sin(phi);
        omega += alpha * DT;
        phi += omega * DT;
    }

    void draw(sf::RenderWindow& window) {
        double xPos = LENGTH * std::sin(phi);
        double yPos = LENGTH * std::cos(phi);
        drawLine(window, xOffset, yOffset, xPos + xOffset, yPos + yOffset, color);
        sf::CircleShape ball(RADIUS);
        ball.setFillColor(color);
        ball.setPosition(xPos + xOffset - RADIUS, yPos + yOffset - RADIUS);
        window.draw(ball);
    }

    void drawInitial(sf::RenderWindow& window) {
        double xPos = LENGTH * std::sin(init_phi);
        double yPos = LENGTH * std::cos(init_phi);
        sf::Color transparentColor(color.r, color.g, color.b, TRANSPARANCY);
        drawLine(window, xOffset, yOffset, xPos + xOffset, yPos + yOffset, transparentColor);
        sf::CircleShape ball(RADIUS);
        ball.setFillColor(sf::Color(color.r, color.g, color.b, TRANSPARANCY));
        ball.setPosition(xPos + xOffset - RADIUS, yPos + yOffset - RADIUS);
        window.draw(ball);
    }
};


int main() {
    // Create the window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML Pendulum Animation");
    window.setFramerateLimit(60); // Limit the framerate to 60 frames per second

    sf::Font font;
    font.loadFromFile("arial.ttf");
    sf::Text leftText("Regular Euler", font, 30);
    leftText.setFillColor(sf::Color::White);
    leftText.setPosition(window.getSize().x * 0.3333f - leftText.getGlobalBounds().width / 2, window.getSize().y * 0.92f);
    sf::Text rightText("Symplectic Euler", font, 30);
    rightText.setFillColor(sf::Color::Blue);
    rightText.setPosition(window.getSize().x * 0.6666f - rightText.getGlobalBounds().width / 2, window.getSize().y * 0.92f);

    double xOffset1 = WINDOW_WIDTH / 3;
    double xOffset2 = WINDOW_WIDTH * 2 / 3;
    double yOffset = WINDOW_HEIGHT / 2;
    Pendulum pendulum1(INIT_DEGREES, sf::Color::White, xOffset1, yOffset);
    Pendulum pendulum2(INIT_DEGREES, sf::Color::Blue, xOffset2, yOffset);

    for (int i = 0; i < NUM_DT; ++i) {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        pendulum1.drawInitial(window);
        pendulum2.drawInitial(window);
        pendulum1.draw(window);
        pendulum2.draw(window);
        window.draw(leftText);
        window.draw(rightText);
        window.display();
        if (i == 0) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        pendulum1.integrateRegular();
        pendulum2.integrateSymplectic();
    }

    return 0;
}

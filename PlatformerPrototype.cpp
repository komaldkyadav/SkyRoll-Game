#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
float LEVEL_HEIGHT = 1400.f;

struct SpecialPlatform {
    sf::RectangleShape shape;
    float speed = 0.f;
    float leftBound = 0.f, rightBound = 0.f;
    int direction = 1;
    bool isVanishing = false;
    float vanishTimer = 0.f;
    bool isHidden = false;
    bool isHazard = false;
};

// Global State
int currentLevel = 1;
bool levelComplete = false;
int coinCount = 0;
std::vector<SpecialPlatform> levelPlatforms;
std::vector<sf::CircleShape> levelCoins;
float lavaY = 3000.f;

void setupLevel(int level, sf::RectangleShape& door, sf::CircleShape& player, sf::View& view) {
    levelPlatforms.clear();
    levelCoins.clear();
    levelComplete = false;
    coinCount = 0;

    // Ground Platform
    SpecialPlatform ground;
    ground.shape.setSize(sf::Vector2f(800.f, 40.f));
    ground.shape.setFillColor(sf::Color::White);

    if (level == 1) { // L1: Static platforms
        LEVEL_HEIGHT = 1400.f;
        ground.shape.setPosition(0.f, LEVEL_HEIGHT - 40.f);
        levelPlatforms.push_back(ground);
        for (int i = 0; i < 12; i++) {
            SpecialPlatform p;
            p.shape.setSize(sf::Vector2f(160.f, 20.f));
            p.shape.setFillColor(sf::Color::Magenta);
            p.shape.setPosition(120.f + (i % 2) * 250.f, (LEVEL_HEIGHT - 150.f) - i * 95.f);
            levelPlatforms.push_back(p);
        }
        door.setPosition(460.f, 126.f);
    }
    else if (level == 2) { // L2: Moving Platforms
        LEVEL_HEIGHT = 1600.f;
        ground.shape.setPosition(0.f, LEVEL_HEIGHT - 40.f);
        levelPlatforms.push_back(ground);
        for (int i = 0; i < 10; i++) {
            SpecialPlatform p;
            p.shape.setSize(sf::Vector2f(130.f, 20.f));
            p.shape.setFillColor(sf::Color::Cyan);
            p.speed = 180.f; p.direction = (i % 2 == 0) ? 1 : -1;
            p.leftBound = 50.f; p.rightBound = 620.f;
            p.shape.setPosition(150.f, (LEVEL_HEIGHT - 200.f) - i * 130.f);
            levelPlatforms.push_back(p);
        }
        door.setPosition(400.f, 50.f);
    }
    else if (level == 3) { // L3: Vanishing Platforms
        LEVEL_HEIGHT = 1800.f;
        ground.shape.setPosition(0.f, LEVEL_HEIGHT - 40.f);
        levelPlatforms.push_back(ground);

        int totalPlatforms = 8; // You can change this to 8, 10, or 12 anytime!

        for (int i = 0; i < totalPlatforms; i++) {
            SpecialPlatform p;
            p.shape.setSize(sf::Vector2f(140.f, 20.f));
            p.shape.setFillColor(sf::Color(255, 140, 0)); // Orange
            p.isVanishing = true;
            p.shape.setPosition(100.f + (i % 3) * 200.f, (LEVEL_HEIGHT - 200.f) - i * 130.f);
            levelPlatforms.push_back(p);
        }

        // Dynamically place the door on the very last platform
        int lastIndex = totalPlatforms - 1;
        float lastPlatformX = 100.f + (lastIndex % 3) * 200.f;
        float lastPlatformY = (LEVEL_HEIGHT - 200.f) - lastIndex * 130.f;

        door.setPosition(lastPlatformX + 40.f, lastPlatformY - 80.f);
    
    }
    else if (level >= 4) { // L4 & L5: Hazards + Lava
        LEVEL_HEIGHT = 2200.f;
        lavaY = LEVEL_HEIGHT + 200.f;
        ground.shape.setPosition(0.f, LEVEL_HEIGHT - 40.f);
        levelPlatforms.push_back(ground);

        int totalPlatforms = 16;

        for (int i = 0; i < totalPlatforms; i++) {
            // 1. Safe Green Platforms
            SpecialPlatform p;
            p.shape.setSize(sf::Vector2f(110.f, 20.f));
            p.shape.setFillColor(sf::Color::Green);
            if (level == 5) { p.speed = 220.f; p.leftBound = 50; p.rightBound = 650; }

            p.shape.setPosition(150.f + (i % 2) * 300.f, (LEVEL_HEIGHT - 200.f) - i * 115.f);
            levelPlatforms.push_back(p);

            // 2. Patrolling Red Hazards
            if (i % 3 == 0 && i != 0) {
                SpecialPlatform hazard;
                hazard.shape.setSize(sf::Vector2f(40.f, 40.f)); // Made it a smaller square block
                hazard.shape.setFillColor(sf::Color::Red);
                hazard.isHazard = true;

                // Make the hazard move back and forth to test timing!
                hazard.speed = 180.f;
                hazard.leftBound = 200.f;
                hazard.rightBound = 500.f;
                hazard.direction = (i % 2 == 0) ? 1 : -1;

                // Positioned halfway between the green platforms vertically
                hazard.shape.setPosition(350.f, (LEVEL_HEIGHT - 200.f) - i * 115.f - 60.f);
                levelPlatforms.push_back(hazard);
            }
        }

        // Dynamically place the door safely on the final platform
        int lastIndex = totalPlatforms - 1;
        float lastPlatformX = 150.f + (lastIndex % 2) * 300.f;
        float lastPlatformY = (LEVEL_HEIGHT - 200.f) - lastIndex * 115.f;
        door.setPosition(lastPlatformX + 25.f, lastPlatformY - 80.f);
    }

    // Add Coins
    for (auto& p : levelPlatforms) {
        if (!p.isHazard && p.shape.getSize().x < 700) {
            sf::CircleShape c(8.f);
            c.setFillColor(sf::Color::Yellow);
            c.setPosition(p.shape.getPosition().x + 50, p.shape.getPosition().y - 25);
            levelCoins.push_back(c);
        }
    }

    // Reset Player & Camera
    player.setPosition(400.f, LEVEL_HEIGHT - 80.f);
    view.setCenter(400.f, LEVEL_HEIGHT - 300.f);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Vertical Climb - Complete Game");
    window.setFramerateLimit(60);

    sf::CircleShape player(20.f); player.setFillColor(sf::Color::Blue);
    sf::RectangleShape door(sf::Vector2f(60.f, 80.f)); door.setFillColor(sf::Color::Black);
    sf::RectangleShape lava(sf::Vector2f(800.f, 1000.f)); lava.setFillColor(sf::Color(255, 69, 0, 180));

    sf::View view(sf::FloatRect(0, 0, 800, 600));
    float gravity = 1500.f, velocityY = 0.f, moveSpeed = 350.f, jumpStr = -650.f;
    bool onGround = false;

    setupLevel(currentLevel, door, player, view);
    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) window.close();
            // Next Level trigger
            if (levelComplete && ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Enter) {
                if (currentLevel < 5) {
                    currentLevel++;
                    setupLevel(currentLevel, door, player, view);
                    velocityY = 0;
                }
            }
        }

        if (!levelComplete) {
            // Player Input
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) player.move(-moveSpeed * dt, 0);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) player.move(moveSpeed * dt, 0);

            // Lava rise in Level 5
            if (currentLevel == 5) lavaY -= 65.f * dt;

            onGround = false;

            // Platform Logic
            for (auto& p : levelPlatforms) {
                if (p.isHidden) continue;

                // Move moving platforms
                if (p.speed != 0) {
                    p.shape.move(p.speed * p.direction * dt, 0);
                    if (p.shape.getPosition().x < p.leftBound || p.shape.getPosition().x > p.rightBound) p.direction *= -1;
                }

                // Collision Detection
                if (player.getGlobalBounds().intersects(p.shape.getGlobalBounds()) && velocityY > 0) {
                    if (p.isHazard) {
                        setupLevel(currentLevel, door, player, view); // Reset if red
                        break;
                    }
                    player.setPosition(player.getPosition().x, p.shape.getPosition().y - 40.f);

                    // Ride the platform if it moves
                    if (p.speed != 0) player.move(p.speed * p.direction * dt, 0);

                    velocityY = 0;
                    onGround = true;
                    if (p.isVanishing) p.vanishTimer += dt;
                }

                // Vanishing timer logic
                if (p.isVanishing && p.vanishTimer > 0) p.vanishTimer += dt;
                if (p.vanishTimer > 2.0f) p.isHidden = true; // Increased from 1.2s to 2.0s
            }

            // Gravity & Jumping
            velocityY += gravity * dt;
            player.move(0, velocityY * dt);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && onGround) velocityY = jumpStr;

            // Coin Logic
            for (size_t i = 0; i < levelCoins.size(); i++) {
                if (i < levelPlatforms.size() && levelPlatforms[i].speed != 0) {
                    levelCoins[i].setPosition(levelPlatforms[i].shape.getPosition().x + 50.f, levelPlatforms[i].shape.getPosition().y - 25.f);
                }
                if (levelCoins[i].getRadius() > 0 && player.getGlobalBounds().intersects(levelCoins[i].getGlobalBounds())) {
                    levelCoins[i].setRadius(0); coinCount++;
                }
            }

            // Door Interaction
            if (player.getGlobalBounds().intersects(door.getGlobalBounds())) levelComplete = true;

            // Death Mechanics
            if (player.getPosition().y > lavaY || player.getPosition().y > LEVEL_HEIGHT) {
                setupLevel(currentLevel, door, player, view);
            }

            // Camera bounds (don't scroll below the ground)
            if (player.getPosition().y < view.getCenter().y) {
                view.setCenter(400.f, player.getPosition().y);
            }
        }

        // ---------- DRAWING ----------
        window.clear(sf::Color(10, 10, 40));
        window.setView(view);

        for (auto& p : levelPlatforms) if (!p.isHidden) window.draw(p.shape);
        for (auto& c : levelCoins) if (c.getRadius() > 0) window.draw(c);

        window.draw(door);
        window.draw(player);
        if (currentLevel == 5) { lava.setPosition(0, lavaY); window.draw(lava); }

        // ---------- UI (Fontless) ----------
        window.setView(window.getDefaultView());

        // Draw small coins to represent score top-left
        for (int i = 0; i < coinCount; i++) {
            sf::CircleShape miniCoin(6.f);
            miniCoin.setFillColor(sf::Color::Yellow);
            miniCoin.setPosition(20 + (i * 15), 20);
            window.draw(miniCoin);
        }

        if (levelComplete) {
            // Semi-transparent overlay box
            sf::RectangleShape winBox(sf::Vector2f(400.f, 200.f));
            winBox.setFillColor(sf::Color(0, 0, 0, 200));
            winBox.setPosition(200.f, 200.f);
            window.draw(winBox);

            // Calculate stars
            int stars = (coinCount >= 10) ? 3 : (coinCount >= 5) ? 2 : 1;

            // Draw Stars
            for (int i = 0; i < 3; i++) {
                sf::CircleShape s(30, 5);
                s.setPosition(270 + i * 90, 270);
                s.setFillColor(i < stars ? sf::Color::Yellow : sf::Color(80, 80, 80));
                window.draw(s);
            }
        }
        window.display();
    }
    return 0;
}
#include <math.h>
#include <stdlib.h>
#include "main.hpp"


// Sätter planetens position och hastighet för att den ska hamna i perfekt omloppsbana.
void placeInOrbit(Planet* satellite, Planet centre, double distance) {
  double speed = std::sqrt(GRAVITY_COEFFICIENT * centre.mass() / distance) + centre.dx();

  double angle = ((double)(rand() % 101) / 100.0) * 2.0 * 3.14; // Vi vill få en vinkel i radianer.
  double xSpeed = std::sin(angle) * speed;
  double ySpeed = std::cos(angle) * speed;

  double deltaX = std::cos(angle) * distance;
  double deltaY = -std::sin(angle) * distance;

  
  satellite->setPosition(centre.x() + deltaX, centre.y() + deltaY);
  satellite->setVelocity(centre.dx() + xSpeed, centre.dy() + ySpeed);
}

// Skapar en ny planet, och placerar den i omloppsbana.
Planet* placeSatellite(Planet centre, double distance, double radius, sf::Color* color) {
  Planet* planet = new Planet(0, 0, 0, 0, radius, PLANET_MASS, color);



  placeInOrbit(planet, centre, distance);

  return planet;
}


// Definition av färgerna.
sf::Color* SUN     = new sf::Color(255, 255, 100);
sf::Color* MERCURY = new sf::Color(100, 100, 100);
sf::Color* VENUS   = new sf::Color(255, 200, 200);
sf::Color* EARTH   = new sf::Color(100, 200, 255);
sf::Color* MARS    = new sf::Color(255, 50, 50);
sf::Color* JUPITER = new sf::Color(255, 100, 100);
sf::Color* SATURN  = new sf::Color(255, 100, 100);
sf::Color* URANUS  = new sf::Color(100, 255, 100);
sf::Color* NEPTUNE = new sf::Color(100, 100, 255);
sf::Color* PLUTO   = new sf::Color(255, 100, 100);

// Här initialiseras alla planeter och objekt i solsystemet.
void Game::initializeUniverse() {
  Planet* sun = new Planet(550, 550, 0, 0, 200, SUN_MASS, SUN);
  
  Planet* mercury = placeSatellite(*sun, 800, 50, MERCURY);
  Planet* venus = placeSatellite(*sun, 1640, 50, VENUS);
  Planet* earth = placeSatellite(*sun, 2400, 60, EARTH);
  Planet* mars = placeSatellite(*sun, 3230, 40, MARS);
  Planet* jupiter = placeSatellite(*sun, 4090, 100, JUPITER);
  Planet* saturn = placeSatellite(*sun, 4840, 70, SATURN);
  Planet* uranus = placeSatellite(*sun, 5690, 50, URANUS);
  Planet* neptune = placeSatellite(*sun, 6430, 50, NEPTUNE);
  Planet* pluto = placeSatellite(*sun, 7200, 15, PLUTO);

  placeInOrbit(player, *sun, 3000);
  
  add(sun);
  add(mercury);
  add(venus);
  add(earth);
  add(mars);
  add(jupiter);
  add(saturn);
  add(uranus);
  add(neptune);
  add(pluto);
  add(player);
}


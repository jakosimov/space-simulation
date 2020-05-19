#include <iostream>
#include <SFML/Graphics.hpp>
#include <unistd.h>
#include <vector>
#include <math.h>
#include <map>
#include <tuple>
#include "main.hpp"

#define WIDTH 1980
#define HEIGHT 1080

#define WAIT_TIME 100 // Microseconds

#define TRAIL_WIDTH 2
#define TRAIL_DURATION -2

#define GRAVITY_COEFFICIENT 0.0001
#define PLANET_POINT_COUNT 100

#define SHIP_MASS 1
#define SHIP_RADIUS 10

#define SCALE_FACTOR 1.001;
#define CAMERA_MOVE_DISTANCE_FACTOR 0.4

using std::get;
using std::map;
using std::pair;
using std::vector;

// Definition av alla färger
namespace Colors {
  sf::Color* MERCURY = new sf::Color(255, 100, 100);
  sf::Color* VENUS   = new sf::Color(255, 100, 100);
  sf::Color* EARTH   = new sf::Color(255, 100, 100);
  sf::Color* MARS    = new sf::Color(255, 100, 100);
  sf::Color* JUPITER = new sf::Color(255, 100, 100);
  sf::Color* SATURN  = new sf::Color(255, 100, 100);
  sf::Color* URANUS  = new sf::Color(255, 100, 100);
  sf::Color* NEPTUNE = new sf::Color(255, 100, 100);
  sf::Color* PLUTO   = new sf::Color(255, 100, 100);
  sf::Color* SPACE   = new sf::Color(0, 0, 0);
  sf::Color* PLAYER  = new sf::Color(255, 255, 255);
  sf::Color* EXHAUST = new sf::Color(0, 255, 0);
}

sf::Color* TRAIL_COLOR = new sf::Color(0xFF, 0xE6, 0x6D);
sf::Color* PLANET_COLOR = new sf::Color(255, 100, 100);


// Vad kan denna funktionen göra, tro?
double square(double a) {
  return a * a;
}

// -- Metoder för Object ------

// Objektets position uppdateras baserat på den nuvarande hastigheten.
void Object::move() {
  _x += _dx;
  _y += _dy;
}

// Constructor för Object.
Object::Object(double x, double y, double dx, double dy, double mass, ObjectType type) {
  _x = x;
  _y = y;
  _dx = dx;
  _dy = dy;
  _fx = _fy = 0.0;
  _mass = mass;
  _type = type;
}


// Getters för alla fält som tillhör Object.
double Object::x() const {
  return _x;
}

double Object::y() const {
  return _y;
}

double Object::dx() const {
  return _dx;
}

double Object::dy() const {
  return _dy;
}

double Object::mass() const {
  return _mass;
}

ObjectType Object::type() const {
  return _type;
}

vector<Object*> Universe::getObjects() {
  return objects;
}

// Applicerar en kraft på ett objekt, med storleken 'fx' i x-led och 'fy' i y-led.
void Object::applyForce(double fx, double fy) {
  _fx += fx;
  _fy += fy;
}

// Beräknar avståndet mellan två objekt.
double Object::distance(Object* o) const {
  return std::sqrt(square(x() - o->x()) + square(y() - o->y()));
}

// Uppdaterar ett objekt för en frame.
void Object::update(Universe universe) {
  move();
  _dx += _fx / _mass;
  _dy += _fy / _mass;
  _fx = 0.0;
  _fy = 0.0;
}

// -- Metoder för Planet ------

// Getter för Planet.radius.
double Planet::radius() const {
  return _radius;
}


// Constructor för Planet.
Planet::Planet(double x, double y, double dx, double dy, double radius, double mass) : Object(x, y, dx, dy, mass, PlanetType) {
  _radius = radius;
  color = PLANET_COLOR;
}


// Uppdaterar en planet för en frame.
void Planet::update(Universe universe) {
  Object::update(universe);

  // Kollar igenom varenda annat objekt, och kollar om det är en kollision.
  for (Object* object : universe.getObjects()) {
    if (object == this) {
      continue;
    }
    
    if (object->type() == PlanetType) {
      Planet other = *((Planet*)object);
      if (isOverlapping(other)) {
        handleCollision((Planet*)object);
      }
    }
  }
}

// Här hanteras kollsioner mellan planeter, så att de studsar, istället för att bara passera igenom varandra.
void Planet::handleCollision(Planet* other) {
  // Försök inte förstå dig på detta, bara en massa konstig matte och fysik.
  
  Planet o = *other;
  Point pos = Point(x(), y());
  Point pos2 = Point(o.x(), o.y());

  Point delta = pos - pos2;
  double distance = delta.magnitude();
  Point mtd = delta * (((radius() + o.radius()) - distance) / distance);

  double massInverse = 1.0 / mass();
  double massInverse2 = 1.0 / o.mass();

  Point velo = Point(dx(), dy());
  Point velo2 = Point(o.dx(), o.dy());
  
  Point newPos = pos + (mtd * (massInverse / (massInverse + massInverse2)));
  Point newPos2 = pos2 - (mtd * (massInverse2 / (massInverse + massInverse2)));

  Point v = velo - velo2;
  double vn = v * mtd.normalize();

  if (vn > 0.0) {
    return;
  }

  double restitution = 1.0;
  double i = (-(1.0 + restitution) * vn) / (massInverse + massInverse2);
  Point impulse = mtd.normalize() * i;
  
  // Här ändras den faktiska hastigheten och positionen
  _x = newPos.x;
  _y = newPos.y;
  other->_x = newPos2.x;
  other->_y = newPos2.y;
  Point newVelo = velo + (impulse * massInverse);
  Point newVelo2 = velo2 - (impulse * massInverse2);
  _dx = newVelo.x;
  _dy = newVelo.y;
  other->_dx = newVelo2.x;
  other->_dy = newVelo2.y;
}

// Ritar en planet på skärmen.
void Planet::draw(sf::RenderTarget& target, const Universe* universe) const {
  double scaledRadius = radius() * universe->scale();
  sf::CircleShape shape(scaledRadius);
  shape.setPointCount(PLANET_POINT_COUNT);
  shape.setFillColor(*color);
  Point scaledPoint = universe->scalePoint(Point(x(), y()));
  int x0 = scaledPoint.x;
  int y0 = scaledPoint.y;
  shape.setPosition(x0 - scaledRadius, y0 - scaledRadius);
  target.draw(shape);
}

// Ser om två planeter överlappar.
bool Planet::isOverlapping(Planet o) const {
  double minDistance = radius() + o.radius();
  return distance(&o) <= minDistance;
}

// -- Metoder för Point ------

// Contructor för Point.
Point::Point(double _x, double _y) {
  x = _x;
  y = _y;
}

// Implementerar bara några vanliga operatörer för punkten.
bool Point::operator>(const Point o) const {
  if (x == o.x) {
    return y > o.y;
  }
  return x > o.x;
}

bool Point::operator<(const Point o) const {
  if (x == o.x) {
    return y < o.y;
  }
  return x < o.x;
}

bool Point::operator==(const Point o) const {
  return x == o.x && y == o.y;
}

Point Point::operator+(const Point o) const {
  return Point(x + o.x, y + o.y);
}

Point Point::operator-(const Point o) const {
  return Point(x - o.x, y - o.y);
}

Point Point::operator*(double s) const {
  return Point(x * s, y * s);
}

// Normaliserar en vektor, d.v.s. behåller samma riktning, men sätter magnituden till 1.
Point Point::normalize() const {
  return (*this) * (1 / magnitude());
}

// Beräknar magnituden, eller storleken, på en vektor.
double Point::magnitude() const {
  return std::sqrt(x * x + y * y);
}

// Beräknar 'dot product' för två vektorer.
double Point::operator*(const Point o) const {
  return x * o.x + y * o.y;
}


Planet* placeSatellite(Planet p, double distance, double radius) {
  double speed = std::sqrt(GRAVITY_COEFFICIENT * p.mass() / distance);
  
  return new Planet(p.x(), p.y()-distance, speed, 0, radius, 0.00001);
}

// -- Metoder för Universe ------

void Universe::increaseScale(double s) {
  _scale *= s;
}

Point Universe::scalePoint(Point p) const {
  int x = (p.x - xOrigin()) * scale();
  int y = (p.y - yOrigin()) * scale();

  return Point(x, y);
}

// Sätter ut alla planeter i solsystemet.
vector<Object*> initialCondition1() {
  vector<Object*> objects;
  Planet* mainPlanet = new Planet(550, 550, 0, 0, 80, 10000);
  
  objects.push_back(mainPlanet);
  objects.push_back(placeSatellite(*mainPlanet, 1000, 40));
  objects.push_back(placeSatellite(*mainPlanet, 1400, 60));
  objects.push_back(placeSatellite(*mainPlanet, 2000, 40));
  objects.push_back(new Planet(200, 200, 0.01, 0, 10, 10));
  return objects;
}

vector<Object*> initialCondition2() {
  vector<Object*> objects;
  objects.push_back(new Planet(200, 210, 0.1, 0, 10, 1));
  objects.push_back(new Planet(500, 200, 0, 0, 50, 10000));
  return objects;
}

vector<Object*> Universe::initialCondition() {
  return initialCondition1();
}

// Applicerar gravitation mellan alla objekt i universumet. Följer Newtons gravitationslag.
void Universe::applyGravity() {
  for (Object* source : objects) {
    for (Object* target : objects) {
      if (source == target) {
        continue;
      }
      double distance = source->distance(target);
      double forceStrength = GRAVITY_COEFFICIENT * source->mass() * target->mass() / square(distance);
      double forceX = forceStrength * (source->x() - target->x()) / distance;
      double forceY = forceStrength * (source->y() - target->y()) / distance;
      target->applyForce(forceX, forceY);
    }
  }
}

void Universe::resetVisits() {
  visited = map<Point, int>();
}

void Universe::freeObjects() {
  for (Object* object : objects) {
    free(object);
  }
}

void Universe::clearVisited() {
  if (TRAIL_DURATION >= 0) {
    map<Point, int>::iterator iter = visited.begin();
    while (iter != visited.end()) {
      int lastVisit = iter->second;
      if (currentTime - lastVisit >= TRAIL_DURATION) {
        iter = visited.erase(iter);
      } else {
        ++iter;
      }
    }
  }
}
  
Universe::Universe() {
  objects = initialCondition();
  resetVisits();
  _x = 0;
  _y = 0;
  _scale = 1.0;
}

double Universe::scale() const {
  return _scale;
}

double Universe::xOrigin() const {
  return _x;
}

double Universe::yOrigin() const {
  return _y;
}

void Universe::reset() {
  freeObjects();
  resetVisits();
  objects = initialCondition();
}

void Universe::handleKeyPresses() {
  double scaling = SCALE_FACTOR;
  double moveDistance = CAMERA_MOVE_DISTANCE_FACTOR / scale();
  
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
    translate(0, -moveDistance);
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
    translate(0, moveDistance);
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
    translate(-moveDistance, 0);
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
    translate(moveDistance, 0);
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
    increaseScale(scaling);
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
    increaseScale(1 / scaling);
  }
}

void Universe::update() {
  currentTime++;
  applyGravity();

  handleKeyPresses();
  
  for (Object* object : objects) {
    object->update(*this);
    double x = object->x();
    double y = object->y();
    visited[Point(std::round(x), std::round(y))] = currentTime;
  }
  clearVisited();
}

void Universe::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  for (pair<Point, int> element : visited) {
    int x = get<0>(element).x;
    int y = get<0>(element).y;
    int lastVisit = get<1>(element);

    if (TRAIL_DURATION != -2 && (TRAIL_DURATION == -1 || (currentTime - lastVisit < TRAIL_DURATION))) {
      sf::RectangleShape shape(sf::Vector2f(TRAIL_WIDTH,TRAIL_WIDTH));
      shape.setPosition(x - (int)xOrigin(), y - yOrigin());
      shape.setFillColor(*TRAIL_COLOR);
      target.draw(shape);        
    }
  }
    
  for (Object* object : objects) {
    object->draw(target, this);
  }
}

void Universe::translate(double dx, double dy) {
  _x += dx;
  _y += dy;
}

void Universe::addObject(Object* o) {
  objects.push_back(o);
}

Player::Player(double x, double y) : Planet(x, y, 0, 0, SHIP_RADIUS, SHIP_MASS) {}

void Player::push(double dx, double dy) {
  applyForce(dx, dy);
}


void Player::draw(sf::RenderTarget& target, const Universe* universe) const {
  drawFire(target, universe);
  Planet::draw(target, universe);
}

// Ritar den gröna 'elden' bakom spelarens UFO.
void Player::drawFire(sf::RenderTarget& target, const Universe* universe) const {
  double dx = 0, dy = 0;
  double dist = 2;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
    dx -= 1;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
    dx += 1;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
    dy -= 1;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
    dy += 1;
  }
  
  double scaledRadius = radius() * universe->scale();
  sf::CircleShape shape(scaledRadius);
  shape.setPointCount(PLANET_POINT_COUNT);
  shape.setFillColor(sf::Color::Green);
  Point scaledPoint = universe->scalePoint(Point(x(), y()));
  int x0 = scaledPoint.x;
  int y0 = scaledPoint.y;
  shape.setPosition(x0 - scaledRadius - dist * dx, y0 - scaledRadius - dist * dy);
  target.draw(shape);  
}

void drawMainScreen(sf::RenderTarget& target) {
  
}

int main() {
  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Hello world");
  Universe universe;
  Player player(100, 100);
  universe.addObject(&player);

  while (window.isOpen()) {
    sf::Event event;

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
          universe.reset();
        } else if (event.key.code == sf::Keyboard::Q) {
          window.close();
        }
      }
    }

    double pushStrength = 0.001;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
      player.push(-pushStrength, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
      player.push(pushStrength, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
      player.push(0, -pushStrength);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
      player.push(0, pushStrength);
    }
    
    window.clear(*Colors::SPACE);
    universe.update();
    window.draw(universe);

    window.display();
    usleep(WAIT_TIME);
  }

  return 0;
}

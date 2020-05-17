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

#define GRAVITY_COEFFICIENT 0.00001
#define PLANET_POINT_COUNT 100

using std::vector;
using std::map;
using std::pair;
using std::get;

sf::Color BACKGROUND_COLOR = sf::Color(0x39, 0x39, 0x3A);
sf::Color PLANET_COLOR = sf::Color(0xFF, 0x6B, 0x6B);
sf::Color TRAIL_COLOR = sf::Color(0xFF, 0xE6, 0x6D);

double square(double a) {
  return a * a;
}

void Object::move() {
  _x += _dx;
  _y += _dy;
}

Object::Object(double x, double y, double dx, double dy, double mass, ObjectType type) {
  _x = x;
  _y = y;
  _dx = dx;
  _dy = dy;
  _fx = _fy = 0.0;
  _mass = mass;
  _type = type;
}

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

void Object::applyForce(double fx, double fy) {
  _fx += fx;
  _fy += fy;
}

double Object::distance(Object* o) const {
  return std::sqrt(square(x() - o->x()) + square(y() - o->y()));
}

vector<Object*> Universe::getObjects() {
  return objects;
}

void Object::update(Universe universe) {
  move();
  _dx += _fx / _mass;
  _dy += _fy / _mass;
  _fx = 0.0;
  _fy = 0.0;
}

double Planet::radius() const {
  return _radius;
}
  
Planet::Planet(double x, double y, double dx, double dy, double radius, double mass) : Object(x, y, dx, dy, mass, PlanetType) {
  _radius = radius;
}

void Planet::update(Universe universe) {
  Object::update(universe);
  for (Object* object : universe.getObjects()) {
    if (object == this) {
      continue;
    }
    
    if (object->type() == PlanetType) {
      if (isOverlapping( *((Planet*)object) )) {
        std::cout << "Collision!" << std::endl;
      }
    }
  }
}

void Planet::draw(sf::RenderTarget& target, const Universe* universe) const {
  sf::CircleShape shape(radius() * universe->scale());
  shape.setPointCount(PLANET_POINT_COUNT);
  shape.setFillColor(PLANET_COLOR);
  Point scaledPoint = scalePoints(Point(x(), y()), universe);
  int x0 = scaledPoint.x;
  int y0 = scaledPoint.y;
  shape.setPosition(x0 - radius(), y0 - radius());
  target.draw(shape);
}

bool Planet::isOverlapping(Planet o) const {
  double minDistance = radius() + o.radius();
  return distance(&o) <= minDistance;
}

Point::Point(int _x, int _y) {
  x = _x;
  y = _y;
}

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

Planet* placeSatellite(Planet p, double distance, double radius) {
  double speed = std::sqrt(GRAVITY_COEFFICIENT * p.mass() / distance);
  return new Planet(p.x(), p.y()-distance, speed, 0, radius, 0.00001);
}

void Universe::increaseScale(double s) {
  _scale *= s;
}

Point scalePoints(Point p, const Universe* universe) {
  int x = (p.x - universe->xOrigin()) * universe->scale();
  int y = (p.y - universe->yOrigin()) * universe->scale();

  return Point(x, y);
}

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
  objects.push_back(new Planet(200, 200, 0, 0, 10, 1));
  objects.push_back(new Planet(500, 200, 0, 0, 50, 10000));
  return objects;
}

vector<Object*> Universe::initialCondition() {
  return initialCondition2();
}

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

void Universe::update() {
  currentTime++;
  applyGravity();
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
      shape.setFillColor(TRAIL_COLOR);
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

int main() {
  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Hello world");
  Universe universe;

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

    double scaling = 1.001;
    double moveDistance = 0.4 / universe.scale();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
      universe.translate(0, -moveDistance);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
      universe.translate(0, moveDistance);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
      universe.translate(-moveDistance, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
      universe.translate(moveDistance, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
      universe.increaseScale(scaling);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
      universe.increaseScale(1 / scaling);
    }
    
    window.clear(BACKGROUND_COLOR);
    universe.update();
    window.draw(universe);

    window.display();
    usleep(WAIT_TIME);
  }

  return 0;
}

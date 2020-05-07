#include <iostream>
#include <SFML/Graphics.hpp>
#include <unistd.h>
#include <vector>
#include <math.h>

#define WIDTH 1980
#define HEIGHT 1080

#define WAIT_TIME 10000 // Microseconds

using std::vector;

sf::Color BACKGROUND_COLOR = sf::Color(0x39, 0x39, 0x3A);
sf::Color PLANET_COLOR = sf::Color(0xFF, 0x6B, 0x6B);
sf::Color TRAIL_COLOR = sf::Color(0xFF, 0xE6, 0x6D);

double square(double a) {
  return a * a;
}

class Object : public sf::Drawable {
  double _x, _y,
    _dx, _dy, _fx, _fy, _mass; 

  void move() {
    _x += _dx;
    _y += _dy;
  }

protected:
  Object() {
    
  }
  
  Object(double x, double y, double dx, double dy, double mass) {
    _x = x;
    _y = y;
    _dx = dx;
    _dy = dy;
    _fx = _fy = 0.0;
    _mass = mass;
  }

public:

  double x() const {
    return _x;
  }

  double y() const {
    return _y;
  }

  double dx() const {
    return _dx;
  }

  double dy() const {
    return _dy;
  }

  double mass() const {
    return _mass;
  }

  void applyForce(double fx, double fy) {
    _fx += fx;
    _fy += fy;
  }

  double distance(Object* o) {
    return std::sqrt(square(x() - o->x()) + square(y() - o->y()));
  }
  
  void update() {
    move();
    _dx += _fx / _mass;
    _dy += _fy / _mass;
    _fx = 0.0;
    _fy = 0.0;
  }
};

class Planet : public Object {
  double _radius;

public:
  double radius() const {
    return _radius;
  }
  
  Planet(double x, double y, double dx, double dy, double radius, double mass) : Object(x, y, dx, dy, mass) {
    _radius = radius;
  }

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    sf::CircleShape shape(radius());
    shape.setFillColor(PLANET_COLOR);
    shape.setPosition(x() - radius(), y() - radius());
    target.draw(shape);
  }
};

void applyGravity(vector<Object*> objects) {
  for (Object* source : objects) {
    for (Object* target : objects) {
      if (source == target) {
        continue;
      }
      double forceCoeff = 0.1;
      double distance = source->distance(target);
      double forceStrength = forceCoeff * source->mass() * target->mass() / square(distance);
      double forceX = forceStrength * (source->x() - target->x()) / distance;
      double forceY = forceStrength * (source->y() - target->y()) / distance;
      target->applyForce(forceX, forceY);
    }
  }
}

#define TRAIL_WIDTH 7
#define TRAIL_DURATION -1

vector<Object*> initialCondition() {
  vector<Object*> objects;
  objects.push_back(new Planet(550, 550, 0, 0, 150, 10000));
  objects.push_back(new Planet(550, 700, 2.7, 0, 10, 10000 / 80));
  return objects;
}

void freeObjects(vector<Object*> objects) {
  for (Object* object : objects) {
    free(object);
  }
}

struct VisitInfo {
  int lastVisited;
};


int main() {
  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Hello world");
  vector<Object*> objects = initialCondition();
  vector<VisitInfo> visits;


  
  visits.resize(WIDTH * HEIGHT);

  for (int i = 0 ; i < WIDTH*HEIGHT ; i++) {
    visits[i].lastVisited = -1;
  }

  int time = 0;

  while (window.isOpen()) {
    sf::Event event;

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
          objects = initialCondition();
          for (int i = 0 ; i < WIDTH*HEIGHT ; i++) {
            visits[i].lastVisited = -1;
          }
        }
      }
    }

    window.clear();
    for (int x = 0 ; x < WIDTH ; x++) {
      for (int y = 0 ; y < HEIGHT ; y++) {
        VisitInfo visit = visits[WIDTH * y + x];
        sf::RectangleShape shape(sf::Vector2f(TRAIL_WIDTH,TRAIL_WIDTH));
        shape.setPosition(x, y);
        if (visit.lastVisited != -1 && (std::abs(time - visit.lastVisited) < TRAIL_DURATION || TRAIL_DURATION == -1)) {
          shape.setFillColor(TRAIL_COLOR);
        } else {
          shape.setFillColor(BACKGROUND_COLOR);
        }
        window.draw(shape);        
      }
    }
    for (Object* object : objects) {
      object->update();
      window.draw(*object);
      double x = object->x();
      double y = object->y();
      if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        visits[WIDTH * (int)y + (int)x].lastVisited = time;        
      }
    }


    applyGravity(objects);
    window.display();
    usleep(WAIT_TIME);
    time++;
  }
  

  return 0;
}

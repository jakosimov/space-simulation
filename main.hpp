#include <SFML/Graphics.hpp>
#include <vector>
#include <map>

using std::vector;
using std::map;

class Universe;

enum ObjectType { PlanetType, PlaceholderType };

class Object {
  ObjectType _type;

  void move();

protected:
  double _x, _y, _dx, _dy, _fx, _fy, _mass;
  Object(double,double,double,double,double,ObjectType);

public:
  double x() const;
  double y() const;
  double dx() const;
  double dy() const;
  double mass() const;

  ObjectType type() const;

  void applyForce(double,double);
  double distance(Object*) const;

  virtual void update(Universe);
  virtual void draw(sf::RenderTarget&, const Universe*) const = 0;
};

struct Point {
  // Punkt fungerar i detta fall precis på samma sätt som en vektor.
  
  double x, y;
  Point(double, double);
  bool operator>(const Point) const;
  bool operator<(const Point) const;
  bool operator==(const Point) const;
  Point operator+(const Point) const;
  Point operator-(const Point) const;
  Point operator*(double) const;
  Point normalize() const;
  double operator*(const Point) const;
  
  double magnitude() const;
};

class Universe : public sf::Drawable {
  vector<Object*> objects;
  map<Point, int> visited;
  int currentTime;
  double _x, _y;
  double _scale;

  vector<Object*> initialCondition();
  void applyGravity();
  void resetVisits();
  void freeObjects();
  void clearVisited();
  void handleKeyPresses();

public:
  Universe();
  double xOrigin() const;
  double yOrigin() const;
  double scale() const;
  vector<Object*> getObjects();
  Point scalePoint(Point) const;
  void increaseScale(double);
  void reset();
  void update();
  void translate(double, double);
  void addObject(Object*);
  virtual void draw(sf::RenderTarget&, sf::RenderStates) const;
};

class Planet : public Object {
  double _radius;
  
protected:
  sf::Color* color;

public:
  double radius() const;
  Planet(double, double, double, double, double, double);
  virtual void draw(sf::RenderTarget&, const Universe*) const;
  bool isOverlapping(Planet) const;
  void update(Universe);
  void handleCollision(Planet*);
};

class Player : public Planet {
  void drawFire(sf::RenderTarget&, const Universe*) const;
  
public:
  Player(double, double);
  
  void push(double, double);
  void draw(sf::RenderTarget&, const Universe*) const;
};


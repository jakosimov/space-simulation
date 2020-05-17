#include <SFML/Graphics.hpp>
#include <vector>
#include <map>

using std::vector;
using std::map;

class Universe;

enum ObjectType { PlanetType, PlaceholderType };

class Object {
  double _x, _y, _dx, _dy, _fx, _fy, _mass;
  ObjectType _type;

  void move();

protected:
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
  int x, y;
  Point(int, int);
  bool operator>(const Point) const;
  bool operator<(const Point) const;
  bool operator==(const Point) const;
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

public:
  Universe();
  double xOrigin() const;
  double yOrigin() const;
  double scale() const;
  vector<Object*> getObjects();
  void increaseScale(double);
  void reset();
  void update();
  void translate(double, double);
  virtual void draw(sf::RenderTarget&, sf::RenderStates) const;
};

class Planet : public Object {
  double _radius;

public:
  double radius() const;
  Planet(double, double, double, double, double, double);
  void draw(sf::RenderTarget&, const Universe*) const;
  bool isOverlapping(Planet planet) const;
  void update(Universe);
};

Planet* placeSatellite(Planet, double);
Point scalePoints(Point p, const Universe*);


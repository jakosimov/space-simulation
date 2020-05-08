#include <SFML/Graphics.hpp>
#include <vector>
#include <map>

using std::vector;
using std::map;

class Universe;

class Object {
  double _x, _y, _dx, _dy, _fx, _fy, _mass;

  void move();

protected:
  Object();

  Object(double,double,double,double,double);

public:
  double x() const;
  double y() const;
  double dx() const;
  double dy() const;
  double mass() const;

  void applyForce(double,double);
  double distance(Object*) const;

  void update();
  virtual void draw(sf::RenderTarget&, const Universe*) const;
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
};

Planet* placeSatellite(Planet, double);
Point scalePoints(Point p, const Universe*);


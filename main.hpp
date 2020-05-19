#include <SFML/Graphics.hpp>
#include <vector>
#include <map>

using std::vector;
using std::map;

// Definition av konstanter.

#define GRAVITY_COEFFICIENT 0.0001

#define WIDTH 1980
#define HEIGHT 1080

#define WAIT_TIME 10 // Microseconds

#define PLANET_POINT_COUNT 1000
#define SCALE_FACTOR 1.001
#define CAMERA_MOVE_DISTANCE_FACTOR 0.4

#define PLAYER_PUSH_STRENGTH 0.00001
#define SHIP_MASS 0.1
#define SHIP_RADIUS 50
#define PLANET_MASS 0.1
#define SUN_MASS 100000.0

#define START_ZOOM 0.1

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

  void setPosition(double,double);
  void setVelocity(double,double);

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
  Object* player;
  double _x, _y;
  double _scale;

  void applyGravity();
  void freeObjects();
  void handleKeyPresses();
  void drawBackground(sf::RenderTarget&) const;

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
  void setOffset(double, double);
  void addObject(Object*);
  void draw(sf::RenderTarget&, sf::RenderStates) const;
  void drawCircle(double, double,double,sf::Color*,sf::RenderTarget&) const;
};

class Planet : public Object {
  double _radius;
  
protected:
  sf::Color* color;

public:
  double radius() const;
  Planet(double, double, double, double, double, double, sf::Color*);
  virtual void draw(sf::RenderTarget&, const Universe*) const;
  bool isOverlapping(Planet) const;
  void update(Universe);
  void handleCollision(Planet*);
};

class Player : public Planet {
  void drawExhaust(sf::RenderTarget&, const Universe*) const;
  
public:
  Player(double, double);
  Player();
  
  void push(double, double);
  void draw(sf::RenderTarget&, const Universe*) const;
};

enum Screen { MENU_SCREEN, INGAME };

class Game : public sf::Drawable {
  Universe universe;
  Player* player;
  bool cameraIsLocked;
  Screen currentScreen;
  

  void handleKeyPresses();
  void initializeUniverse();
  void add(Planet*);
  void centerCamera();

public:
  Game();
  void update();
  void reset();
  void draw(sf::RenderTarget&, sf::RenderStates) const;
  void toggleCameraLock();
  void drawMainMenu(sf::RenderTarget&) const;
};


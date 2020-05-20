#include <iostream>
#include <SFML/Graphics.hpp>
#include <unistd.h>
#include <vector>
#include <math.h>
#include <map>
#include <tuple>
#include "main.hpp"

using std::get;
using std::map;
using std::pair;
using std::vector;

sf::Font font;

sf::Color* SPACE_COLOR   = new sf::Color(0, 0, 20);
sf::Color* PLAYER_COLOR  = new sf::Color(50, 255, 100);
sf::Color* EXHAUST_COLOR = new sf::Color(255, 0, 0);
sf::Color* STAR_COLOR = new sf::Color(100, 100, 100);

// Vad kan denna funktionen göra, tro?
double square(double a) {
  return a * a;
}

// -- Metoder för Object ------

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

// Objektets position uppdateras baserat på den nuvarande hastigheten.
void Object::move() {
  _x += _dx;
  _y += _dy;
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

void Object::setPosition(double x, double y) {
  _x = x;
  _y = y;
}

void Object::setVelocity(double dx, double dy) {
  _dx = dx;
  _dy = dy;
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

// Constructor för Planet.
Planet::Planet(double x, double y, double dx, double dy, double radius, double mass, sf::Color* _color) : Object(x, y, dx, dy, mass, PlanetType) {
  _radius = radius;
  color = _color;
}

// Getter för Planet.radius.
double Planet::radius() const {
  return _radius;
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
  universe->drawCircle(x(), y(), radius(), color, target);
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

// -- Metoder för Universe ------

Universe::Universe() {
  _x = 0;
  _y = 0;
  _scale = START_ZOOM;
}

// 'Zoomar in' universumet.
void Universe::increaseScale(double s) {
  _scale *= s;
}

Point Universe::scalePoint(Point p) const {
  int x = (p.x - xOrigin()) * scale();
  int y = (p.y - yOrigin()) * scale();

  return Point(x, y);
}

// Applicerar gravitation mellan alla objekt i universumet. Följer Newtons gravitationslag.
void Universe::applyGravity() {
  for (Object* source : objects) {
    for (Object* target : objects) {
      if (source == target) {
        continue;
      }
      double distance = source->distance(target);

      // Kraftens storlek enligt Newtons gravitationslag.
      double forceStrength = GRAVITY_COEFFICIENT * source->mass() * target->mass() / square(distance);
      
      double forceX = forceStrength * (source->x() - target->x()) / distance;
      double forceY = forceStrength * (source->y() - target->y()) / distance;
      target->applyForce(forceX, forceY);
    }
  }
}

// Raderar alla gamla objekt från minnet.
void Universe::freeObjects() {
  for (Object* object : objects) {
    free(object); // Frigör utrymmet som pointern pekar på.
  }
}

// Getters för alla fält för Universe.
double Universe::scale() const {
  return _scale;
}

double Universe::xOrigin() const {
  return _x;
}

double Universe::yOrigin() const {
  return _y;
}

// Återstället universumet till utgångsläget.
void Universe::reset() {
  freeObjects();
  objects = vector<Object*>();
}

// Reagerar på vilka knappar som är nedtryckta.
void Universe::handleKeyPresses() {
  double scaling = SCALE_FACTOR;
  double moveDistance = CAMERA_MOVE_DISTANCE_FACTOR / scale();

  // Om spelaren trycker på någon av piltangenterna ska kameran flyttas.
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

  // Om spelaren trycker på Shift eller Control ska skärmen zoomas in och ut.
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
    increaseScale(scaling);
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
    increaseScale(1 / scaling);
  }
}

// Updaterar universumet för en frame.
void Universe::update() {
  applyGravity();

  handleKeyPresses();
  
  for (Object* object : objects) {
    object->update(*this);
  }
}

// Ritar ut universumet på skärmen.
void Universe::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  drawBackground(target);
  for (Object* object : objects) {
    object->draw(target, this);
  }
}

// Ritar en stjärna på skärmen.
void drawStar(int x, int y, sf::RenderTarget& target) {
  sf::RectangleShape rect(sf::Vector2f(1,1));
  rect.setPosition(x * WIDTH / 1980 ,y * WIDTH / 1980);
  rect.setFillColor(*STAR_COLOR);
  target.draw(rect);
}

// Ritar några fina stjärnbilder i bakgrunden.
void Universe::drawBackground(sf::RenderTarget& target) const {
  // Ritar typ ut halva karlavagnen.
  drawStar(200, 200, target);
  drawStar(400, 240, target);
  drawStar(180, 380, target);
  drawStar(380, 440, target);
  drawStar(500, 50, target);

  // Ritary typ ut orion.
  drawStar(1000, 700, target);
  drawStar(1100, 730, target);
  drawStar(1200, 710, target);
  drawStar(950, 900, target);
  drawStar(1350, 880, target);
  drawStar(950, 400, target);
  drawStar(1250, 440, target);
  drawStar(1150, 350, target);
}

// Flyttar hela 'ytan' i någon riktning.
void Universe::translate(double dx, double dy) {
  _x += dx;
  _y += dy;
}

// Flyttar hela 'ytan' till en absolut offset.
void Universe::setOffset(double x, double y) {
  _x = x;
  _y = y;
}

void Universe::addObject(Object* o) {
  objects.push_back(o);
}

void Universe::drawCircle(double x, double y, double radius, sf::Color* color, sf::RenderTarget& target) const {
  double scaledRadius = radius * scale();
  sf::CircleShape shape(scaledRadius);
  shape.setPointCount(PLANET_POINT_COUNT);
  shape.setFillColor(*color);
  Point scaledPoint = scalePoint(Point(x, y));
  int x0 = scaledPoint.x;
  int y0 = scaledPoint.y;
  shape.setPosition(x0 - scaledRadius, y0 - scaledRadius);
  target.draw(shape);
}

// -- Metoder för Player ------

// Constructor för Player.
Player::Player(double x, double y) : Planet(x, y, 0, 0, SHIP_RADIUS, SHIP_MASS, PLAYER_COLOR) {}

Player::Player() : Player(0, 0) {}

// Applicerar en kraft på spelaren i någon riktning.
void Player::push(double dx, double dy) {
  applyForce(dx, dy);
}

// Ritar spelaren på skärmen.
void Player::draw(sf::RenderTarget& target, const Universe* universe) const {
  drawExhaust(target, universe);
  Planet::draw(target, universe);
}

// Ritar den gröna 'elden' bakom spelarens UFO.
void Player::drawExhaust(sf::RenderTarget& target, const Universe* universe) const {
  double dx = 0, dy = 0;
  double dist = 10;

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

  universe->drawCircle(x() - dist * dx, y() - dist * dy, radius(), EXHAUST_COLOR, target);
}

// -- Metoder för Game ------

// Constructor för Game.
Game::Game() {
  reset();
  cameraIsLocked = false;
  currentScreen = MENU_SCREEN;
}

// Lägger till ett nytt objekt till universumet.
void Game::add(Object* p) {
  universe.addObject(p);
}

// Centrerar kameran på spelaren.
void Game::centerCamera() {
  double cameraX = player->x() - WIDTH / (universe.scale() * 2);
  double cameraY = player->y() - HEIGHT / (universe.scale() * 2);
  universe.setOffset(cameraX, cameraY);
}

// Updaterar en frame i spelet.
void Game::update() {
  handleKeyPresses();
  if (currentScreen == INGAME) {
    if (cameraIsLocked) {
      centerCamera();
    }
    universe.update();
  }
}

// Startar om spelet.
void Game::reset() {
  universe.reset();
  player = new Player(100, 100);
  initializeUniverse();
  centerCamera();
}

// Centrerar texten på skärmen.
void centerText(int y, int fontSize, sf::Text& text) {
  text.setCharacterSize(fontSize);
  sf::FloatRect textRect = text.getLocalBounds();
  text.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);
  text.setPosition(sf::Vector2f(WIDTH / 2.0f, y));
}

// Ritar startskärmen till skärmen.
void Game::drawMainMenu(sf::RenderTarget& target) const {
  sf::Text text;
  text.setFont(font);
  text.setFillColor(sf::Color::White);

  text.setString(L"Jakobs (inte så vackra) rymdsimulation!");
  centerText(100, 40 * 1980 / WIDTH, text);
  target.draw(text);

  text.setString(L"Använd WASD för att styra skeppet, och piltangenterna för att styra kameran.");
  centerText(200, 20 * 1980 / WIDTH, text);
  target.draw(text);
  
  text.setString(L"Du kan även låsa kameran till skeppet genom att trycka 'C'.");
  centerText(230, 20 * 1980 / WIDTH, text);
  target.draw(text);

  text.setString(L"Tryck 'Enter' för att fortsätta!");
  centerText(260, 20 * 1980 / WIDTH, text);
  target.draw(text);

  text.setString(L"FAQ:");
  centerText(320, 20 * 1980 / WIDTH, text);
  target.draw(text);

  text.setString(L"Q: Vad kan man egentligen göra i simulationen?");
  centerText(350, 20 * 1980 / WIDTH, text);
  target.draw(text);

  text.setString(L"A: Inte mycket, man kan mest åka runt i ditt gröna UFO och putta på olika");
  centerText(380, 20 * 1980 / WIDTH, text);
  target.draw(text);

  text.setString(L"planeter, och se vad som händer med deras omloppsbanor.");
  centerText(410, 20 * 1980 / WIDTH, text);
  target.draw(text);
}

// Ritar spelet till skärmen.
void Game::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  if (currentScreen == INGAME) {
    target.draw(universe);
  } else if (currentScreen == MENU_SCREEN) {
    drawMainMenu(target);
  }
}

// Här hanteras spelet användarens input som inte är direkt relaterat till simulationen.
void Game::handleKeyPresses() {
  if (currentScreen == INGAME) {
    double pushStrength = PLAYER_PUSH_STRENGTH;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
      player->push(-pushStrength, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
      player->push(pushStrength, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
      player->push(0, -pushStrength);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
      player->push(0, pushStrength);
    }    
  } else if (currentScreen == MENU_SCREEN) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
      currentScreen = INGAME;
    }
  }
}

// Togglar huruvida kameran ska vara låst på spelaren.
void Game::toggleCameraLock() {
  cameraIsLocked = !cameraIsLocked;
}

int main() {
  srand((unsigned)time(0)); 
  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Rymdsimulator");
  Game game;

  
  // Försöker ladda in fonten som används i startskärmen;
  if (!font.loadFromFile("PressStart2P-Regular.ttf")) {
    std::cout << "Kunde inte ladda fonten!" << std::endl;
    return -1;
  }

  // Huvudloopen.
  while (window.isOpen()) {
    sf::Event event;

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Q) {
          window.close();
        } else if (event.key.code == sf::Keyboard::C) {
          game.toggleCameraLock();
        }
      }
    }
    
    window.clear(*SPACE_COLOR);
    
    game.update();
    window.draw(game);

    window.display();
    usleep(WAIT_TIME); // Detta är egentligen inte det bästa sättet att hantera det på, men det fungerar helt okej.
  }

  return 0;
}

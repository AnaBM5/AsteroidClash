#pragma once
#include<Blit3D.h>

class Shot
{
public:
	glm::vec2 velocity, position;
	Sprite *sprite = NULL;
	float timeToLive = 1.6f; //shots live for 2 seconds
	void Draw();
	bool Update(float seconds); //return false if shot dead (timeToLive <= 0)
};

class Ship
{
public:
	Sprite* shieldSprite = NULL; //blit this semi-transparent sprite ove rthe ship, to show te shield is active
	Sprite *shotSprite = NULL; //all shots use this sprite
	std::vector<Sprite *> spriteList; //1 frame no thrust, 3 frame animation for thrusting
	glm::vec2 velocity, position;
	float angle = 0; //rotation angle for the ship sprite
	float shotTimer = 0; //keeps track of whether it's been long enough since the last shot to fire a new one
	float radius = 27.f; //collision circle size for this object
	float radius2 = radius * radius; //radius squared
	int lives = 3;
	int frameNumber = 0; //what frame of the ship spritesheet are we displaying?
	float thrustTimer = 0; //controls advancing the frameNumber
	bool thrusting = false; //are we thrusting right now?
	bool turningLeft = false; //controls turning
	bool turningRight = false;
	float shieldTimer = 0; //if >0, the shield up
	float blinkTimer = 0.f; //to animate the shield graphic, unused as of yet
	bool blink = true; //cntrol blinking the shield graphic, toggle this to make blink effect
	bool boostedShot = false; // wether to use the power up shoot or not

	void Draw();
	void Update(float seconds);
	bool Shoot(std::vector<Shot> &shotList); //creates a shot
};

enum class AsteroidSize {SMALL, MEDIUM, LARGE};

class Asteroid
{
public:
	glm::vec2 velocity, position;
	Sprite* sprite = NULL;
	float radius2; //radius squared for collison checking purposes
	float spin; //some random amount of tumble, in degrees per second
	float angle = 0; //to rotate the sprite
	AsteroidSize size;

	void Draw();
	void Update(float seconds); 
};

Asteroid AsteroidFactory(AsteroidSize type);
void InitializeRNG();

AsteroidSize MakeRandomAsteroidSize();

//calculate distance squared between two points
float DistanceSquared(glm::vec2 pos1, glm::vec2 pos2);

//returns true if there was a collision
bool Collide(Shot& s, Asteroid& a);
bool Collide(Ship* s, Asteroid& a);

extern std::vector<Sprite*> explosionSpriteList;

class Explosion
{
public:
	int frameNum = 0; //which frame to display
	float frameSpeed = 1.f / 20.f; //how long a frame is displayed for
	float frameTimer = 0.f;
	glm::vec2 position;
	float scale = 3.f;

	void Draw();
	bool Update(float seconds);//return false if explosion should be removed

	Explosion(glm::vec2 location, float size);
};

class PowerUp
{
public:
	glm::vec2 position;
	Sprite* sprite = NULL;
	Sprite* shotPowerUpSprite = NULL;
	Sprite* shieldPowerUpSprite = NULL;
	float radius2 = 55.f * 55.f; //intentionally a bit smaller than the sprite to make it more difficult to get the power ups
	float timer = 0; // time that the power up appears on screen
	float timeTilAppearance; // how much it takes to show up
	int powerUp = 0; // which powerUp will be used
	bool active = false;

	void Draw();
	void Update(float seconds);

	void DefinePowerUp();
	void ApplyPowerUp(Ship* s);
};

bool Collide(Ship* s, PowerUp* p);
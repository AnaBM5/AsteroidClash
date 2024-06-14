#include "Ship.h"
#include <random>

std::mt19937 rng;

void Shot::Draw()
{
	sprite->Blit(position.x, position.y);
}

bool Shot::Update(float seconds)
{
	//TODO:
	// determine if we are past the end of our life time,
	//if so return false
	timeToLive -= seconds;
	if (timeToLive < 0) return false;
	//move the shot
	position += velocity * seconds;

	//TODO:
	//wrap around
	//bounds check position
	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;
	
	return true;
}

void Ship::Draw()
{
	//change ship angle because my graphics face "up", not "right"
	spriteList[frameNumber]->angle = angle - 90;

	//draw ship sprite at 9 different locations for wrap-around effect
	for (float xoffset = -1920; xoffset <= 1920; xoffset += 1920)
	{
		for (float yoffset = -1080; yoffset <= 1080; yoffset += 1080)
		{
			spriteList[frameNumber]->Blit(position.x + xoffset, position.y + yoffset);
			//draw shield if it's activated
			if (shieldTimer > 0)
			{
				if (blink) shieldSprite->Blit(position.x + xoffset, position.y + yoffset, 1.5f, 1.5f, 0.75f);
				//else shieldSprite->Blit(position.x + xoffset, position.y + yoffset, 1.5f, 1.5f, shieldTimer / 3.f);
			}
		}
	}
}

void Ship::Update(float seconds)
{
	//handle turning
	if (turningLeft)
	{
		angle += 180.f * seconds;
	}

	if (turningRight)
	{
		angle -= 180.f * seconds;
	}

	if (thrusting)
	{
		//calculate facing vector
		float radians = angle * (M_PI / 180);
		glm::vec2 facing;
		facing.x = std::cos(radians);
		facing.y = std::sin(radians);

		facing *= seconds * 400.f;

		velocity += facing;

		//check if over max speed
		if (velocity.length() > 600.f)
		{
			velocity = glm::normalize(velocity) * 600.f;
		}

		thrustTimer += seconds;
		
		//animation timing
		if (thrustTimer >= 1.f / 20.f)
		{
			//change frames
			frameNumber++;
			if (frameNumber > 3)
				frameNumber = 1;

			thrustTimer -= 1.f / 20.f;
		}
	}
	else frameNumber = 0;

	//update position
	position += velocity * seconds;

	//bounds check position
	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;

	//reduce velocity due to "space friction"
	float scale = 1.f - seconds * 0.5f;
	velocity *= scale;

	//velocity could potentialy get very small:we should
	//cap it to zero when it gets really close,
	//but that is a more advanced topic that
	//requires techniques like std::isnan()
	float length = glm::length(velocity);
	if(std::isnan(length) || length < 0.00001f)
		velocity = glm::vec2(0, 0);

	//handle shot timer
	if (shotTimer > 0) shotTimer -= seconds;

	//handle shield timer
	if (shieldTimer > 0) shieldTimer -= seconds;
	
}

bool Ship::Shoot(std::vector<Shot> &shotList)
{
	if(shotTimer > 0) return false;
	//if (shieldTimer > 0) return false;

	//TODO:
	//time to shoot!
	//reset shot timer
	shotTimer = 0.15f;
	if (!boostedShot) 
	{
		shotTimer = 0.15f;
		//make a new shot
		Shot S;
		//set the shot's sprite and position using the ship's variables
		S.position = position;
		S.sprite = shotSprite;
		//build a vector from the ship angle
		S.velocity.x = cos(glm::radians(angle));
		S.velocity.y = sin(glm::radians(angle));
		//scale up the shot velocity
		S.velocity *= 600.f;

		//add the ship velocity 
		S.velocity += velocity;

		//add the shot to the shotList
		shotList.push_back(S);
	}
	else
	{
		for (int i = 0; i < 3; ++i)
		{
			shotTimer = 0.2f;
			Shot S2;
			//S2.velocity = S.velocity;
			S2.velocity.x = cos(glm::radians(angle));
			S2.velocity.y = sin(glm::radians(angle));
			//scale up the shot velocity
			S2.velocity *= 600.f;

			S2.sprite = shotSprite;

			float gunAngle = angle;
			if (i == 0)
				gunAngle += 117.f;
			if (i == 1)
				gunAngle -= 117.f;

			glm::vec2 gunVector;
			gunAngle = glm::radians(gunAngle);
			gunVector.x = cos(gunAngle);
			gunVector.y = sin(gunAngle);

			gunVector *= 37.f;
			S2.position = position + gunVector;

			//add the shot to the shotList
			shotList.push_back(S2);
		}
	}

	return true;
}

void Asteroid::Draw()
{
	sprite->angle = angle;

	for (float xoffset = -1920; xoffset <= 1920; xoffset += 1920)
	{
		for (float yoffset = -1080; yoffset <= 1080; yoffset += 1080)
		{
			sprite->Blit(position.x + xoffset, position.y + yoffset);
		}
	}
}

void Asteroid::Update(float seconds)
{
	position += velocity * seconds;

	//wrap around
	//bounds check position
	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;

	angle += spin * seconds;
}

extern Sprite* largeAsteroid;
extern Sprite* mediumAsteroid;
extern Sprite* smallAsteroid;

std::uniform_real_distribution<float> posDistX(0.f, 1920.f);
std::uniform_real_distribution<float> posDistY(0.f, 1080.f);
std::uniform_real_distribution<float> spinDist(-45.f, 45.f);
std::uniform_real_distribution<float> directionDist(0.f, 360.f);
std::uniform_real_distribution<float> largeSpeedDist(20.f, 150.f);
std::uniform_real_distribution<float> mediumSpeedDist(200.f, 300.f);
std::uniform_real_distribution<float> smallSpeedDist(350.f, 450.f);

Asteroid AsteroidFactory(AsteroidSize type)
{
	Asteroid A;

	A.angle = directionDist(rng);

	float dirAngle = directionDist(rng);//0-360 degrees direction
	A.velocity.x = cos(glm::radians(dirAngle));
	A.velocity.y = sin(glm::radians(dirAngle));


	switch (type)
	{
	case AsteroidSize::LARGE:
		A.sprite = largeAsteroid;
		A.velocity *= largeSpeedDist(rng);
		A.radius2 = (242.f/2) * (242.f / 2);
		break;
	case AsteroidSize::MEDIUM:
		A.sprite = mediumAsteroid;
		A.velocity *= mediumSpeedDist(rng);
		A.radius2 = (90.f/2) * (90.f / 2);
		break;
	case AsteroidSize::SMALL:
		A.sprite = smallAsteroid;
		A.velocity *= smallSpeedDist(rng);
		A.radius2 = (50.f/2) * (50.f/2);
		break;
	}

	A.size = type;

	A.position.x = posDistX(rng);
	A.position.y = posDistY(rng);

	A.spin = spinDist(rng);

	return A;
}

void InitializeRNG()
{
	std::random_device rd;
	rng.seed(rd());
}

std::uniform_int_distribution<int> assteroidSizeDist(0, 2);

AsteroidSize MakeRandomAsteroidSize()
{
	int size = assteroidSizeDist(rng);

	switch (size)
	{
	case 0:
		return AsteroidSize::SMALL;
		break;
	case 1:
		return AsteroidSize::MEDIUM;
		break;
	default:
		return AsteroidSize::LARGE;
		break;
	}
}

float DistanceSquared(glm::vec2 pos1, glm::vec2 pos2)
{
	return (pos1.x - pos2.x) * (pos1.x - pos2.x) +
		(pos1.y - pos2.y) * (pos1.y - pos2.y);
}

bool Collide(Shot& s, Asteroid& a)
{
	//point to circle collision check
	if (DistanceSquared(s.position, a.position) > a.radius2) return false;
	return true;
}

bool Collide(Ship* s, Asteroid& a)
{
	if (s->shieldTimer > 0) return false;
	if (DistanceSquared(s->position, a.position) > a.radius2 + s->radius2) return false;
	return true;
}


void Explosion::Draw()
{
	explosionSpriteList[frameNum]->Blit(position.x, position.y, scale, scale);
}

bool Explosion::Update(float seconds)
{
	//update frame timer
	frameTimer += seconds;
	//see if we should advance the frame counter
	if (frameTimer >= frameSpeed)
	{
		if (frameNum > explosionSpriteList.size() - 2) return false;
		frameNum++; //advance to the next frame

		frameTimer -= frameSpeed;
	}
	
	return true;
}

Explosion::Explosion(glm::vec2 location, float size)
{
	position = location;
	scale = size;
}

void PowerUp::Draw()
{
	if (active)
	{
		sprite->Blit(position.x, position.y);
	}
	
	
}

void PowerUp::Update(float seconds)
{
	/*if (timer <= 0.f)
	{
		active = false;
	}
		
	else
		timer-= seconds;*/

	if (timeTilAppearance > 0.f)
		timeTilAppearance -= seconds;
	else if (timeTilAppearance <= 0.f && timer > 0.f)
	{
		active = true;
		timer -= seconds;
	}
	else if (timer <= 0.f)
	{
		active = false;
	}
		
}

void PowerUp::DefinePowerUp()
{
	std::uniform_int_distribution<int> powerUpType(1, 2);
	std::uniform_real_distribution<float> powerPositionX(200.f, 1800.f);
	std::uniform_real_distribution<float> powerPositionY(200.f, 820.f);
	std::uniform_real_distribution<float> timeUntilAppears(2.f, 8.f);
	powerUp = powerUpType(rng);

	if (powerUp == 1)
	{
		sprite = shotPowerUpSprite;
	}
	else {
		sprite = shieldPowerUpSprite;
	}

	position.x = powerPositionX(rng);
	position.y = powerPositionY(rng);

	timer = 6.f;
	timeTilAppearance = timeUntilAppears(rng);
}

void PowerUp::ApplyPowerUp(Ship* s)
{
	if (powerUp == 1)
	{
		s->boostedShot = true;
	}
	else {
		s->shieldTimer = 8.f;
	}


	timer = 0.f;

	active = false;
}

bool Collide(Ship* s, PowerUp* p)
{
	if (!p->active) return false;
	if (DistanceSquared(s->position, p->position) > p->radius2 + s->radius2) return false;
	return true;
}
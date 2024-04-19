#pragma once
#include <vector>
#include <memory>
#include <numbers>
#include "Tools.h"
#include <random>

class PARTICLE;
class RANDOMORIGIN;
class CENTEREDEDORIGIN;
class FORCEDORIGIN;
class CIRCULARPOS;
class CIRCULARDIR;
class TRAIL;
class RADIAL;
class CIRCLE;
class WATERFALL;

class PARTICLES
{
private:
	uint8_t NUMBER_OF_PARTICLES;
	int32_t SCREEN_WIDTH;
	int32_t SCREEN_HEIGHT;
	uint8_t Remaining;
	std::vector<std::unique_ptr<PARTICLE>> particles;
	std::default_random_engine e1;

public:
	PARTICLES(const int _SCREEN_WIDTH, const int _SCREEN_HEIGHT);

	enum PARTICULES_TYPES {
		TYPE_RANDOMORIGIN,
		TYPE_CENTEREDEDORIGIN,
		TYPE_FORCEDORIGIN,
		TYPE_CIRCULARPOS,
		TYPE_CIRCULARDIR,
		TYPE_TRAIL,
		TYPE_RADIAL,
		TYPE_CIRCLE,
		TYPE_WATERFALL,
		TYPE_THISISMADNESS
	};
	bool draw(uint8_t* fire);
	void init(uint8_t _NUMBER_OF_PARTICLES, uint8_t _PARTICULES_TYPES, const int xOrg = 0, const int yOrg = 0, const double radius = 0);
	const uint8_t getRemaining();
};

/* EXPLOSIONS */
/*particle structure*/
class PARTICLE
{
protected:
	int32_t xpos, ypos;
	int32_t xdir, ydir;
	uint8_t colorindex;
	bool dead;
	int32_t SCREEN_WIDTH;
	int32_t SCREEN_HEIGHT;
	PARTICLE(const int _SCREEN_WIDTH, const int _SCREEN_HEIGHT);
	virtual const bool setDeath();
public:
	bool draw(uint8_t* fire, bool& bAtLeastOneAlive);
};

class RANDOMORIGIN : public PARTICLE
{
public:
	// Starts from random pos.
	RANDOMORIGIN(std::default_random_engine& e1, const int SCREEN_WIDTH, const int SCREEN_HEIGHT);
};

// Original settings from "Retro Particle Explosion Effect - W.P. van Paassen - 2002"
class CENTEREDEDORIGIN : public PARTICLE
{
public:
	// Starts from the middle of the screen
	CENTEREDEDORIGIN(std::default_random_engine& e1, const int SCREEN_WIDTH, const int SCREEN_HEIGHT);
};

class FORCEDORIGIN : public PARTICLE
{
public:
	// Starts from (xOrg; yOrg)
	FORCEDORIGIN(std::default_random_engine& e1, const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg);
};

class CIRCULARPOS : public PARTICLE
{
private:
	const double radius;
public:
	// Starts from (xOrg; yOrg)
	// Every particle with the same (xOrg; yOrg) will be at the same distance from it (On a circle with the radius specified centered from there)
	CIRCULARPOS(std::default_random_engine& e1, const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg, const double _radius);
};

class CIRCULARDIR : public PARTICLE
{
public:
	// Starts from (xOrg; yOrg)
	// Every particle with the same (xOrg; yOrg) will run away from there at same speed
	CIRCULARDIR(std::default_random_engine& e1, const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg);
};

class TRAIL : public PARTICLE
{
public:
	// Starts from the bottom of the screen.
	// Will die when starting to fall down.
	TRAIL(std::default_random_engine& e1, const int SCREEN_WIDTH, const int SCREEN_HEIGHT);

	void init(std::default_random_engine& e1);

	const int32_t getXPos() { return xpos; }
	const int32_t getYPos() { return ypos; }

protected:
	const bool setDeath();
};

class RADIAL : public CIRCULARDIR
{
public:
	// Starts from (xOrg; yOrg)
	// Every particle with the same (xOrg; yOrg) will run away from there at same speed
	// Will never fall.
	RADIAL(std::default_random_engine& e1, const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg);
protected:
	const bool setDeath();
};

class CIRCLE : public RADIAL
{
public:
	// Starts from (xOrg; yOrg)
	// Every particle with the same (xOrg; yOrg) will run away from there at same speed
	// Will never fall.
	// Will die after 30 frames.
	CIRCLE(std::default_random_engine& e1, const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg);
private:
	uint8_t radius;
protected:
	const bool setDeath();
};

class WATERFALL : public RADIAL
{
public:
	// Starts from (xOrg; yOrg)
	// Every particle with the same (xOrg; yOrg) will run away from there at same speed
	// Will never fall.
	// Will fall after 30 frames.
	WATERFALL(std::default_random_engine& e1, const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg);
private:
	uint8_t radius;
protected:
	const bool setDeath();
};

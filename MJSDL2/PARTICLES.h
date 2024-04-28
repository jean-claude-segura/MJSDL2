#pragma once
#include <vector>
#include <memory>
#include <numbers>
#include "Tools.h"

class PARTICLE;
class RANDOMORIGIN;
class CENTEREDEDORIGIN;
class FORCEDORIGIN;
class CIRCULARPOS;
class CIRCULARDIR;
class TRAIL;
class RADIAL;
class CIRCLE;
class SPHERE;
class BUBBLE;
class RADIALSPHERE;
class WATERFALL;

class PARTICLES
{
private:
	int32_t NUMBER_OF_PARTICLES;
	int32_t SCREEN_WIDTH;
	int32_t SCREEN_HEIGHT;
	uint8_t Remaining;
	std::vector<std::unique_ptr<PARTICLE>> particles;

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
		TYPE_SPHERE,
		TYPE_BUBBLE,
		TYPE_RADIALSPHERE,
		TYPE_WATERFALL,
		TYPE_THISISMADNESS
	};
	bool draw(uint8_t* fire);
	void init(uint32_t _NUMBER_OF_PARTICLES, uint8_t _PARTICULES_TYPES, const int xOrg = 0, const int yOrg = 0, const double radius = 0);
	const uint8_t getRemaining();
};

/* EXPLOSIONS */
/*particle structure*/
class PARTICLE
{
protected:
	int32_t XPos, YPos;
	int32_t XDir, YDir;
	uint8_t ColorIndex;
	bool Dead;
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
	RANDOMORIGIN(const int SCREEN_WIDTH, const int SCREEN_HEIGHT);
};

// Original settings from "Retro Particle Explosion Effect - W.P. van Paassen - 2002"
class CENTEREDEDORIGIN : public PARTICLE
{
public:
	// Starts from the middle of the screen
	CENTEREDEDORIGIN(const int SCREEN_WIDTH, const int SCREEN_HEIGHT);
};

class FORCEDORIGIN : public PARTICLE
{
public:
	// Starts from (xOrg; yOrg)
	FORCEDORIGIN(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg);
};

class CIRCULARPOS : public PARTICLE
{
private:
	const double Radius;
public:
	// Starts from (xOrg; yOrg)
	// Every particle with the same (xOrg; yOrg) will be at the same distance from it (On a circle with the Radius specified centered from there)
	CIRCULARPOS(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg, const double radius);
};

class CIRCULARDIR : public PARTICLE
{
public:
	// Starts from (xOrg; yOrg)
	// Every particle with the same (xOrg; yOrg) will run away from there at same speed
	CIRCULARDIR(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg);
};

class TRAIL : public PARTICLE
{
public:
	// Starts from the bottom of the screen.
	// Will die when starting to fall down.
	TRAIL(const int SCREEN_WIDTH, const int SCREEN_HEIGHT);

	void init();

	const int32_t getXPos() { return XPos; }
	const int32_t getYPos() { return YPos; }

protected:
	const bool setDeath();
};

class RADIAL : public CIRCULARDIR
{
public:
	// Starts from (xOrg; yOrg)
	// Every particle with the same (xOrg; yOrg) will run away from there at same speed
	// Will never fall.
	RADIAL(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg);
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
	CIRCLE(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg);
private:
	uint8_t Radius;
protected:
	const bool setDeath();
};

class SPHERE : public PARTICLE
{
public:
	// Starts from (xOrg; yOrg)
	// Every particle with the same (xOrg; yOrg) will run away from there at same speed
	// Will never fall.
	// Will die after 30 frames.
	SPHERE(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg);
private:
	uint8_t Radius;
protected:
	const bool setDeath();
};

class BUBBLE : public PARTICLE
{
private:
	const double Radius;
public:
	// Starts from (xOrg; yOrg)
	// Every particle with the same (xOrg; yOrg) will be at the same distance from it (On a circle with the Radius specified centered from there)
	BUBBLE(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg, const double radius);
};

class RADIALSPHERE : public PARTICLE
{
private:
	const double Radius;
	uint8_t DeathRadius;
public:
	// Starts from (xOrg; yOrg)
	// Every particle with the same (xOrg; yOrg) will be at the same distance from it (On a circle with the Radius specified centered from there)
	RADIALSPHERE(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg, const double radius);
protected:
	const bool setDeath();
};

class WATERFALL : public RADIAL
{
public:
	// Starts from (xOrg; yOrg)
	// Every particle with the same (xOrg; yOrg) will run away from there at same speed
	// Will fall after 30 frames.
	WATERFALL(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg);
private:
	uint8_t Radius;
protected:
	const bool setDeath();
};

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
class CIRCLE;

class PARTICLES
{
private:
	uint8_t NUMBER_OF_PARTICLES;
	int32_t SCREEN_WIDTH;
	int32_t SCREEN_HEIGHT;
	uint8_t Remaining;
	std::vector<std::unique_ptr<PARTICLE>> particles;
public:
	PARTICLES(const int32_t, const int32_t);

	enum PARTICULES_TYPES {
		TYPE_RANDOMORIGIN,
		TYPE_CENTEREDEDORIGIN,
		TYPE_FORCEDORIGIN,
		TYPE_CIRCULARPOS,
		TYPE_CIRCULARDIR,
		TYPE_TRAIL,
		TYPE_CIRCLE
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
	RANDOMORIGIN(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg);
};

class CENTEREDEDORIGIN : public PARTICLE
{
public:
	CENTEREDEDORIGIN(const int SCREEN_WIDTH, const int SCREEN_HEIGHT);
};

class FORCEDORIGIN : public PARTICLE
{
public:
	FORCEDORIGIN(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg);
};

class CIRCULARPOS : public PARTICLE
{
private:
	const double radius;
public:
	CIRCULARPOS(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg, const double _radius);
};

class CIRCULARDIR : public PARTICLE
{
public:
	CIRCULARDIR(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg);
};

class TRAIL : public PARTICLE
{
public:
	TRAIL(const int SCREEN_WIDTH, const int SCREEN_HEIGHT);

	void init();

	const int32_t getXPos() { return xpos; }
	const int32_t getYPos() { return ypos; }

protected:
	const bool setDeath();
};

class CIRCLE : public CIRCULARDIR
{
public:
	CIRCLE(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg);
private:
	uint8_t radius;
protected:
	const bool setDeath();
};

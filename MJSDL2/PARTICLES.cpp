#include "PARTICLES.h"

PARTICLES::PARTICLES(const int _SCREEN_WIDTH, const int _SCREEN_HEIGHT) : SCREEN_WIDTH(_SCREEN_WIDTH), SCREEN_HEIGHT(_SCREEN_HEIGHT)
{
	NUMBER_OF_PARTICLES = 0;
	Remaining = 0;
}

bool PARTICLES::draw(uint8_t* fire)
{
	bool bAtLeastOneAlive = false;
	for (auto& particle : particles)
	{
		if (particle->draw(fire, bAtLeastOneAlive))
			bAtLeastOneAlive = true;
		else
			--Remaining;
	}
	return bAtLeastOneAlive;
}

void PARTICLES::init(uint8_t _NUMBER_OF_PARTICLES, uint8_t _PARTICULES_TYPES, const int xOrg, const int yOrg, const double radius)
{
	NUMBER_OF_PARTICLES = _NUMBER_OF_PARTICLES;
	particles.clear();
	switch (_PARTICULES_TYPES)
	{
	default:
	case TYPE_RANDOMORIGIN:
		for (int i = 0; i < NUMBER_OF_PARTICLES; ++i)
			particles.emplace_back(std::make_unique<RANDOMORIGIN>(RANDOMORIGIN{ SCREEN_WIDTH, SCREEN_HEIGHT }));
		break;
	case TYPE_CENTEREDEDORIGIN:
		for (int i = 0; i < NUMBER_OF_PARTICLES; ++i)
			particles.emplace_back(std::make_unique<CENTEREDEDORIGIN>(CENTEREDEDORIGIN{ SCREEN_WIDTH, SCREEN_HEIGHT }));
		break;
	case TYPE_FORCEDORIGIN:
		for (int i = 0; i < NUMBER_OF_PARTICLES; ++i)
			particles.emplace_back(std::make_unique<FORCEDORIGIN>(FORCEDORIGIN{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg }));
		break;
	case TYPE_CIRCULARPOS:
	{
		const double radius = (int)(50.0 * (rand() / (RAND_MAX + 1.0))) + 30;
		for (int i = 0; i < NUMBER_OF_PARTICLES; ++i)
			particles.emplace_back(std::make_unique<CIRCULARPOS>(CIRCULARPOS{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg, radius }));
		break;
	}
	case TYPE_CIRCULARDIR:
		for (int i = 0; i < NUMBER_OF_PARTICLES; ++i)
			particles.emplace_back(std::make_unique<CIRCULARDIR>(CIRCULARDIR{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg }));
		break;
	case TYPE_TRAIL:
		for (int i = 0; i < NUMBER_OF_PARTICLES; ++i)
			particles.emplace_back(std::make_unique<TRAIL>(TRAIL{ SCREEN_WIDTH, SCREEN_HEIGHT }));
		break;
	case TYPE_RADIAL:
		for (int i = 0; i < NUMBER_OF_PARTICLES; ++i)
			particles.emplace_back(std::make_unique<RADIAL>(RADIAL{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg }));
		break;
	case TYPE_CIRCLE:
		for (int i = 0; i < NUMBER_OF_PARTICLES; ++i)
			particles.emplace_back(std::make_unique<CIRCLE>(CIRCLE{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg }));
		break;
	case TYPE_WATERFALL:
		for (int i = 0; i < NUMBER_OF_PARTICLES; ++i)
			particles.emplace_back(std::make_unique<WATERFALL>(WATERFALL{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg }));
		break;
	case TYPE_THISISMADNESS:
		for (int i = 0; i < NUMBER_OF_PARTICLES; ++i)
		{
			auto next = (int)(TYPE_THISISMADNESS * (rand() / (RAND_MAX + 1.0)));
			switch (next)
			{
			default:
			case TYPE_RANDOMORIGIN:
					particles.emplace_back(std::make_unique<RANDOMORIGIN>(RANDOMORIGIN{ SCREEN_WIDTH, SCREEN_HEIGHT }));
				break;
			case TYPE_CENTEREDEDORIGIN:
					particles.emplace_back(std::make_unique<CENTEREDEDORIGIN>(CENTEREDEDORIGIN{ SCREEN_WIDTH, SCREEN_HEIGHT }));
				break;
			case TYPE_FORCEDORIGIN:
					particles.emplace_back(std::make_unique<FORCEDORIGIN>(FORCEDORIGIN{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg }));
				break;
			case TYPE_CIRCULARPOS:
			{
				const double radius = (int)(50.0 * (rand() / (RAND_MAX + 1.0))) + 30;
					particles.emplace_back(std::make_unique<CIRCULARPOS>(CIRCULARPOS{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg, radius }));
				break;
			}
			case TYPE_CIRCULARDIR:
					particles.emplace_back(std::make_unique<CIRCULARDIR>(CIRCULARDIR{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg }));
				break;
			case TYPE_TRAIL:
					particles.emplace_back(std::make_unique<TRAIL>(TRAIL{ SCREEN_WIDTH, SCREEN_HEIGHT }));
				break;
			case TYPE_RADIAL:
					particles.emplace_back(std::make_unique<RADIAL>(RADIAL{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg }));
				break;
			case TYPE_CIRCLE:
				particles.emplace_back(std::make_unique<CIRCLE>(CIRCLE{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg }));
				break;
			case TYPE_WATERFALL:
				particles.emplace_back(std::make_unique<WATERFALL>(WATERFALL{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg }));
				break;
			}
		}
		break;
	}
}

const uint8_t PARTICLES::getRemaining()
{
	return Remaining;
}

PARTICLE::PARTICLE(const int _SCREEN_WIDTH, const int _SCREEN_HEIGHT) : SCREEN_WIDTH(_SCREEN_WIDTH), SCREEN_HEIGHT(_SCREEN_HEIGHT)
{
	xpos = ypos = 0;
	xdir = ydir = 0;
	colorindex = 255;
	dead = false;
}

bool PARTICLE::draw(uint8_t* fire, bool& bAtLeastOneAlive)
{
	if (!dead && !setDeath())
	{
		int32_t temp;
		/* draw particle */
		if (ypos > 1 && xpos > 1 &&
			(xpos < SCREEN_WIDTH - 3))
		{
			temp = ypos * SCREEN_WIDTH + xpos;

			fire[temp] = colorindex;
			fire[temp - 1] = colorindex;
			fire[temp + SCREEN_WIDTH] = colorindex;
			fire[temp - SCREEN_WIDTH] = colorindex;
			fire[temp + 1] = colorindex;
		}
		bAtLeastOneAlive = true;
		return true;
	}
	return false;
}

const bool PARTICLE::setDeath()
{
	xpos += xdir;
	ypos += ydir;

	/* is particle dead? */
	if (colorindex == 0 ||
		(ypos >= SCREEN_HEIGHT - 3))
	{
		dead = true;
		return true;
	}

	// Is particle on the left side of visible screen coming back ?
	// If not -> dead.
	if (xpos <= 1 && xdir <= 0) // Minimal check : those going left are not coming back for sure.
	{
		dead = true;
		return true;
	}

	// Is particle on the right side of visible screen coming back ?
	// If not -> dead.
	if ((xpos >= SCREEN_WIDTH - 3) && xdir >= 0) // Minimal check : those going right are not coming back for sure.
	{
		dead = true;
		return true;
	}

	// Is particle above (Actually *under*) visible screen coming back ?
	// If not -> dead.
	if (ypos <= 1)
	{
		int32_t deltaX = xdir;
		if ((deltaX > 0 && (xpos + deltaX >= SCREEN_WIDTH - 3)) ||
			(deltaX < 0 && (xpos + deltaX <= 1)))
		{
			dead = true;
			return true;
		}
	}

	/* gravity takes over */
	ydir++;

	/* particle cools off */
	colorindex--;

	return false;
}

RANDOMORIGIN::RANDOMORIGIN(const int SCREEN_WIDTH, const int SCREEN_HEIGHT) : PARTICLE(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	const int xOrg = (int)(SCREEN_WIDTH * (rand() / (RAND_MAX + 1.0)));
	const int yOrg = (int)(SCREEN_HEIGHT * (rand() / (RAND_MAX + 1.0)));

	xpos = xOrg - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
	ypos = yOrg - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
	xdir = -10 + (int)(20.0 * (rand() / (RAND_MAX + 1.0)));
	ydir = -17 + (int)(19.0 * (rand() / (RAND_MAX + 1.0)));
}

CENTEREDEDORIGIN::CENTEREDEDORIGIN(const int SCREEN_WIDTH, const int SCREEN_HEIGHT) : PARTICLE(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	/* randomly init particle, generate it in the center of the screen */
	xpos = (SCREEN_WIDTH >> 1) - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
	ypos = (SCREEN_HEIGHT >> 1) - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
	xdir = -10 + (int)(20.0 * (rand() / (RAND_MAX + 1.0)));
	ydir = -17 + (int)(19.0 * (rand() / (RAND_MAX + 1.0)));
}

FORCEDORIGIN::FORCEDORIGIN(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg) : PARTICLE(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	xpos = xOrg - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
	ypos = yOrg - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
	xdir = -10 + (int)(20.0 * (rand() / (RAND_MAX + 1.0)));
	ydir = -17 + (int)(19.0 * (rand() / (RAND_MAX + 1.0)));
}

CIRCULARPOS::CIRCULARPOS(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg, const double _radius) : radius(_radius), PARTICLE(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	double angle = std::numbers::pi * 2 * (rand() / (RAND_MAX + 1.0));
	xpos = xOrg + std::cos(angle) * radius; // X est le cosinus de l'angle.
	ypos = yOrg + std::sin(angle) * radius; // Y est le sinus de l'angle.
	xdir = -10 + (int)(20.0 * (rand() / (RAND_MAX + 1.0)));
	ydir = -17 + (int)(19.0 * (rand() / (RAND_MAX + 1.0)));
}

CIRCULARDIR::CIRCULARDIR(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg) : PARTICLE(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	double angle = std::numbers::pi * 2 * (rand() / (RAND_MAX + 1.0));
	xpos = xOrg - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
	ypos = yOrg - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
	xdir = std::cos(angle) * 10;
	ydir = std::sin(angle) * 10;
}

TRAIL::TRAIL(const int SCREEN_WIDTH, const int SCREEN_HEIGHT) : PARTICLE(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	xpos = (int)(SCREEN_WIDTH * (rand() / (RAND_MAX + 1.0))) - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
	ypos = SCREEN_HEIGHT - 4;
	xdir = -10 + (int)(20.0 * (rand() / (RAND_MAX + 1.0)));
	ydir = -32 + (int)(12.0 * (rand() / (RAND_MAX + 1.0))); // -33 <  < -20
}

void TRAIL::init()
{
	xpos = (int)(SCREEN_WIDTH * (rand() / (RAND_MAX + 1.0))) - 20 + (int)(40.0 * (rand() / (RAND_MAX + 1.0)));
	ypos = SCREEN_HEIGHT - 4;
	xdir = -10 + (int)(20.0 * (rand() / (RAND_MAX + 1.0)));
	ydir = -32 + (int)(12.0 * (rand() / (RAND_MAX + 1.0))); // -32 -20
	colorindex = 255;
	dead = false;
}

const bool TRAIL::setDeath()
{
	xpos += xdir;
	ypos += ydir;

	/* is particle dead? */
	if (colorindex == 0 ||
		(ypos >= SCREEN_HEIGHT - 3))
	{
		dead = true;
		return true;
	}

	// Is particle above (Actually *under*) visible screen coming back ?
	// If not -> dead.
	if (ypos <= 1)
	{
		int32_t deltaX = xdir;
		if ((deltaX > 0 && (xpos + deltaX >= SCREEN_WIDTH - 3)) ||
			(deltaX < 0 && (xpos + deltaX <= 1)))
		{
			dead = true;
			return true;
		}
	}

	if (ydir > 0)
	{
		dead = true;
		return true;
	}

	/* gravity takes over */
	ydir++;

	/* particle cools off */
	colorindex--;
	return false;
}

RADIAL::RADIAL(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg) : CIRCULARDIR(SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg)
{
}

const bool RADIAL::setDeath()
{
	xpos += xdir;
	ypos += ydir;

	/* is particle dead? */
	if (colorindex == 0 ||
		(ypos >= SCREEN_HEIGHT - 3))
	{
		dead = true;
		return true;
	}

	// Is particle on the left side of visible screen coming back ?
	// If not -> dead.
	if (xpos <= 1 && xdir <= 0) // Minimal check : those going left are not coming back for sure.
	{
		dead = true;
		return true;
	}

	// Is particle on the right side of visible screen coming back ?
	// If not -> dead.
	if ((xpos >= SCREEN_WIDTH - 3) && xdir >= 0) // Minimal check : those going right are not coming back for sure.
	{
		dead = true;
		return true;
	}

	// Is particle above (Actually *under*) visible screen coming back ?
	// If not -> dead.
	if (ypos <= 1 && ydir <= 0) // RADIAL never fall.
	{
		dead = true;
		return true;
	}

	/* particle cools off */
	colorindex--;

	return false;
}

CIRCLE::CIRCLE(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg) : RADIAL(SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg)
{
	radius = 30;
}

const bool CIRCLE::setDeath()
{
	xpos += xdir;
	ypos += ydir;
	if (radius > 0)
		--radius;

	/* is particle dead? */
	if (colorindex == 0 ||
		(ypos >= SCREEN_HEIGHT - 3) ||
		radius == 0)
	{
		dead = true;
		return true;
	}

	// Is particle on the left side of visible screen coming back ?
	// If not -> dead.
	if (xpos <= 1 && xdir <= 0) // Minimal check : those going left are not coming back for sure.
	{
		dead = true;
		return true;
	}

	// Is particle on the right side of visible screen coming back ?
	// If not -> dead.
	if ((xpos >= SCREEN_WIDTH - 3) && xdir >= 0) // Minimal check : those going right are not coming back for sure.
	{
		dead = true;
		return true;
	}

	// Is particle above (Actually *under*) visible screen coming back ?
	// If not -> dead.
	if (ypos <= 1 && ydir <= 0) // Circulars never fall.
	{
		dead = true;
		return true;
	}

	/* particle cools off */
	colorindex--;

	return false;
}

WATERFALL::WATERFALL(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg) : RADIAL(SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg)
{
	radius = 30;
}

const bool WATERFALL::setDeath()
{
	xpos += xdir;
	ypos += ydir;
	if (radius > 0)
		--radius;

	/* is particle dead? */
	if (colorindex == 0 ||
		(ypos >= SCREEN_HEIGHT - 3))
	{
		dead = true;
		return true;
	}

	// Is particle on the left side of visible screen coming back ?
	// If not -> dead.
	if (xpos <= 1 && xdir <= 0) // Minimal check : those going left are not coming back for sure.
	{
		dead = true;
		return true;
	}

	// Is particle on the right side of visible screen coming back ?
	// If not -> dead.
	if ((xpos >= SCREEN_WIDTH - 3) && xdir >= 0) // Minimal check : those going right are not coming back for sure.
	{
		dead = true;
		return true;
	}

	if (radius == 0)
	{
		// Is particle above (Actually *under*) visible screen coming back ?
		// If not -> dead.
		if (ypos <= 1)
		{
			int32_t deltaX = xdir;
			if ((deltaX > 0 && (xpos + deltaX >= SCREEN_WIDTH - 3)) ||
				(deltaX < 0 && (xpos + deltaX <= 1)))
			{
				dead = true;
				return true;
			}
		}
	}

	/* gravity takes over */
	if (radius == 0)
		ydir++;

	/* particle cools off */
	colorindex--;

	return false;
}

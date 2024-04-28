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

void PARTICLES::init(uint32_t _NUMBER_OF_PARTICLES, uint8_t _PARTICULES_TYPES, const int xOrg, const int yOrg, const double radius)
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
		const double radius = user_uniform_real_distribution<double>( 0., 50. )+30.;
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
	case TYPE_SPHERE:
		for (int i = 0; i < NUMBER_OF_PARTICLES; ++i)
			particles.emplace_back(std::make_unique<SPHERE>(SPHERE{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg }));
		break;
	case TYPE_BUBBLE:
	{
		const double radius = user_uniform_real_distribution<double>(0., 50.) + 30.;
		for (int i = 0; i < NUMBER_OF_PARTICLES; ++i)
			particles.emplace_back(std::make_unique<BUBBLE>(BUBBLE{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg, radius }));
		break;
	}
	case TYPE_RADIALSPHERE:
	{
		const double radius = user_uniform_real_distribution<double>(0., 50.) + 30.;
		for (int i = 0; i < NUMBER_OF_PARTICLES; ++i)
			particles.emplace_back(std::make_unique<RADIALSPHERE>(RADIALSPHERE{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg, radius }));
		break;
	}
	case TYPE_WATERFALL:
		for (int i = 0; i < NUMBER_OF_PARTICLES; ++i)
			particles.emplace_back(std::make_unique<WATERFALL>(WATERFALL{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg }));
		break;
	case TYPE_THISISMADNESS:
		for (int i = 0; i < NUMBER_OF_PARTICLES; ++i)
		{
			auto next = user_uniform_int_distribution<int>( 0, int(TYPE_WATERFALL) );
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
				const double radius = user_uniform_real_distribution<double>( 0., 50. )+30.;
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
			case TYPE_SPHERE:
				particles.emplace_back(std::make_unique<SPHERE>(SPHERE{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg }));
				break;
			case TYPE_BUBBLE:
			{
				const double radius = user_uniform_real_distribution<double>(0., 50.) + 30.;
				particles.emplace_back(std::make_unique<BUBBLE>(BUBBLE{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg, radius }));
				break;
			}
			case TYPE_RADIALSPHERE:
			{
				const double radius = user_uniform_real_distribution<double>(0., 50.) + 30.;
				particles.emplace_back(std::make_unique<RADIALSPHERE>(RADIALSPHERE{ SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg, radius }));
				break;
			}
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
	XPos = YPos = 0;
	XDir = YDir = 0;
	ColorIndex = 255;
	Dead = false;
}

bool PARTICLE::draw(uint8_t* fire, bool& bAtLeastOneAlive)
{
	if (!Dead && !setDeath())
	{
		int32_t temp;
		/* draw particle */
		if (YPos > 1 && XPos > 1 &&
			(XPos < SCREEN_WIDTH - 3))
		{
			temp = YPos * SCREEN_WIDTH + XPos;

			fire[temp] = ColorIndex;
			fire[temp - 1] = ColorIndex;
			fire[temp + SCREEN_WIDTH] = ColorIndex;
			fire[temp - SCREEN_WIDTH] = ColorIndex;
			fire[temp + 1] = ColorIndex;
		}
		bAtLeastOneAlive = true;
		return true;
	}
	return false;
}

const bool PARTICLE::setDeath()
{
	XPos += XDir;
	YPos += YDir;

	/* is particle Dead? */
	if (ColorIndex == 0 ||
		(YPos >= SCREEN_HEIGHT - 3))
	{
		Dead = true;
		return true;
	}

	// Is particle on the left side of visible screen coming back ?
	// If not -> Dead.
	if (XPos <= 1 && XDir <= 0) // Minimal check : those going left are not coming back for sure.
	{
		Dead = true;
		return true;
	}

	// Is particle on the right side of visible screen coming back ?
	// If not -> Dead.
	if ((XPos >= SCREEN_WIDTH - 3) && XDir >= 0) // Minimal check : those going right are not coming back for sure.
	{
		Dead = true;
		return true;
	}

	// Is particle above (Actually *under*) visible screen coming back ?
	// If not -> Dead.
	if (YPos <= 1)
	{
		int32_t deltaX = XDir;
		if ((deltaX > 0 && (XPos + deltaX >= SCREEN_WIDTH - 3)) ||
			(deltaX < 0 && (XPos + deltaX <= 1)))
		{
			Dead = true;
			return true;
		}
	}

	/* gravity takes over */
	YDir++;

	/* particle cools off */
	ColorIndex--;

	return false;
}

RANDOMORIGIN::RANDOMORIGIN(const int SCREEN_WIDTH, const int SCREEN_HEIGHT) : PARTICLE(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	const int xOrg = user_uniform_int_distribution<int32_t>( 0, SCREEN_WIDTH );
	const int yOrg = user_uniform_int_distribution<int32_t>( 0, SCREEN_HEIGHT );

	XPos = xOrg - 20 + user_uniform_int_distribution<int32_t>( 0, 40 );
	YPos = yOrg - 20 + user_uniform_int_distribution<int32_t>( 0, 40 );
	XDir = -10 + user_uniform_int_distribution<int32_t>( 0, 20 );
	YDir = -17 + user_uniform_int_distribution<int32_t>( 0, 19 );
}

// Original settings from "Retro Particle Explosion Effect - W.P. van Paassen - 2002"
CENTEREDEDORIGIN::CENTEREDEDORIGIN(const int SCREEN_WIDTH, const int SCREEN_HEIGHT) : PARTICLE(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	/* randomly init particle, generate it in the center of the screen */
	XPos = (SCREEN_WIDTH >> 1) - 20 + user_uniform_int_distribution<int32_t>( 0, 40 );
	YPos = (SCREEN_HEIGHT >> 1) - 20 + user_uniform_int_distribution<int32_t>( 0, 40 );
	XDir = -10 + user_uniform_int_distribution<int32_t>( 0, 20 );
	YDir = -17 + user_uniform_int_distribution<int32_t>( 0, 19 );
}

FORCEDORIGIN::FORCEDORIGIN(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg) : PARTICLE(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	XPos = xOrg - 20 + user_uniform_int_distribution<int32_t>( 0, 40 );
	YPos = yOrg - 20 + user_uniform_int_distribution<int32_t>( 0, 40 );
	XDir = -10 + user_uniform_int_distribution<int32_t>( 0, 20 );
	YDir = -17 + user_uniform_int_distribution<int32_t>( 0, 19 );
}

CIRCULARPOS::CIRCULARPOS(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg, const double radius) : Radius(radius), PARTICLE(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	const double angle = user_uniform_real_distribution<double>( 0, std::numbers::pi * 2. );
	XPos = xOrg + std::cos(angle) * Radius; // X est le cosinus de l'angle.
	YPos = yOrg + std::sin(angle) * Radius; // Y est le sinus de l'angle.
	XDir = -10 + user_uniform_int_distribution<int32_t>( 0, 20 );
	YDir = -17 + user_uniform_int_distribution<int32_t>( 0, 19 );
}

CIRCULARDIR::CIRCULARDIR(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg) : PARTICLE(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	const double angle = user_uniform_real_distribution<double>( 0, std::numbers::pi * 2. );
	XPos = xOrg - 20 + user_uniform_int_distribution<int32_t>( 0, 40 );
	YPos = yOrg - 20 + user_uniform_int_distribution<int32_t>( 0, 40 );
	XDir = std::cos(angle) * 10;
	YDir = std::sin(angle) * 10;
}

TRAIL::TRAIL(const int SCREEN_WIDTH, const int SCREEN_HEIGHT) : PARTICLE(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	int32_t ydirbase = -32;
	int32_t ydirdecbase = 12;
	if (SCREEN_HEIGHT >= 1080)
	{
		ydirbase = -46;
		ydirdecbase = 18;
	}
	else if (SCREEN_HEIGHT >= 2160)
	{
		ydirbase = -65;
		ydirdecbase = 25;
	}

	XPos = user_uniform_int_distribution<int32_t>( 0, SCREEN_WIDTH ) - 20 + user_uniform_int_distribution<int32_t>( 0, 40 );
	YPos = SCREEN_HEIGHT - 4;
	XDir = -10 + user_uniform_int_distribution<int32_t>( 0, 20 );
	YDir = ydirbase + user_uniform_int_distribution<int32_t>( 0, ydirdecbase ); // -32 -20
}

void TRAIL::init()
{
	// 540 : ( 32 / 20 / 12) +14 +8
	//  *2    +14   +8   +6
	// 1080 : (46 / 28 / 18)
	//  *2    +19 / +12  +7
	// 2160 :  65 / 40 / 25
	int32_t ydirbase = -32;
	int32_t ydirdecbase = 12;
	if (SCREEN_HEIGHT >= 1080)
	{
		ydirbase = -46;
		ydirdecbase = 18;
	}
	else if (SCREEN_HEIGHT >= 2160)
	{
		ydirbase = -65;
		ydirdecbase = 25;
	}
	XPos = user_uniform_int_distribution<int32_t>( 0, SCREEN_WIDTH ) - 20 + user_uniform_int_distribution<int32_t>( 0, 40 );
	YPos = SCREEN_HEIGHT - 4;
	XDir = -10 + user_uniform_int_distribution<int32_t>( 0, 20 );
	YDir = ydirbase + user_uniform_int_distribution<int32_t>( 0, ydirdecbase ); // -32 -20
	ColorIndex = 255;
	Dead = false;
}

const bool TRAIL::setDeath()
{
	XPos += XDir;
	YPos += YDir;

	/* is particle Dead? */
	if (ColorIndex == 0 ||
		(YPos >= SCREEN_HEIGHT - 3))
	{
		Dead = true;
		return true;
	}

	// Is particle above (Actually *under*) visible screen coming back ?
	// If not -> Dead.
	if (YPos <= 1)
	{
		int32_t deltaX = XDir;
		if ((deltaX > 0 && (XPos + deltaX >= SCREEN_WIDTH - 3)) ||
			(deltaX < 0 && (XPos + deltaX <= 1)))
		{
			Dead = true;
			return true;
		}
	}

	if (YDir > 0)
	{
		Dead = true;
		return true;
	}

	/* gravity takes over */
	YDir++;

	/* particle cools off */
	ColorIndex--;
	return false;
}

RADIAL::RADIAL(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg) : CIRCULARDIR(SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg)
{
}

const bool RADIAL::setDeath()
{
	XPos += XDir;
	YPos += YDir;

	/* is particle Dead? */
	if (ColorIndex == 0 ||
		(YPos >= SCREEN_HEIGHT - 3))
	{
		Dead = true;
		return true;
	}

	// Is particle on the left side of visible screen coming back ?
	// If not -> Dead.
	if (XPos <= 1 && XDir <= 0) // Minimal check : those going left are not coming back for sure.
	{
		Dead = true;
		return true;
	}

	// Is particle on the right side of visible screen coming back ?
	// If not -> Dead.
	if ((XPos >= SCREEN_WIDTH - 3) && XDir >= 0) // Minimal check : those going right are not coming back for sure.
	{
		Dead = true;
		return true;
	}

	// Is particle above (Actually *under*) visible screen coming back ?
	// If not -> Dead.
	if (YPos <= 1 && YDir <= 0) // RADIAL never fall.
	{
		Dead = true;
		return true;
	}

	/* particle cools off */
	ColorIndex--;

	return false;
}

CIRCLE::CIRCLE(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg) : RADIAL(SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg)
{
	Radius = 30;
}

const bool CIRCLE::setDeath()
{
	XPos += XDir;
	YPos += YDir;
	if (Radius > 0)
		--Radius;

	/* is particle Dead? */
	if (ColorIndex == 0 ||
		(YPos >= SCREEN_HEIGHT - 3) ||
		Radius == 0)
	{
		Dead = true;
		return true;
	}

	// Is particle on the left side of visible screen coming back ?
	// If not -> Dead.
	if (XPos <= 1 && XDir <= 0) // Minimal check : those going left are not coming back for sure.
	{
		Dead = true;
		return true;
	}

	// Is particle on the right side of visible screen coming back ?
	// If not -> Dead.
	if ((XPos >= SCREEN_WIDTH - 3) && XDir >= 0) // Minimal check : those going right are not coming back for sure.
	{
		Dead = true;
		return true;
	}

	// Is particle above (Actually *under*) visible screen coming back ?
	// If not -> Dead.
	if (YPos <= 1 && YDir <= 0) // Circulars never fall.
	{
		Dead = true;
		return true;
	}

	/* particle cools off */
	ColorIndex--;

	return false;
}

SPHERE::SPHERE(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg) : PARTICLE(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	const double alpha = user_uniform_real_distribution<double>(0, std::numbers::pi * 2.);
	const double beta = user_uniform_real_distribution<double>(0, std::numbers::pi * 2.);
	XPos = xOrg - 20 + user_uniform_int_distribution<int32_t>(0, 40);
	YPos = yOrg - 20 + user_uniform_int_distribution<int32_t>(0, 40);
	XDir = std::cos(alpha) * std::cos(beta) * 10.;
	YDir = std::sin(alpha) * 10;

	Radius = 30;
}

const bool SPHERE::setDeath()
{
	XPos += XDir;
	YPos += YDir;
	if (Radius > 0)
		--Radius;

	/* is particle Dead? */
	if (ColorIndex == 0 ||
		(YPos >= SCREEN_HEIGHT - 3) ||
		Radius == 0)
	{
		Dead = true;
		return true;
	}

	// Is particle on the left side of visible screen coming back ?
	// If not -> Dead.
	if (XPos <= 1 && XDir <= 0) // Minimal check : those going left are not coming back for sure.
	{
		Dead = true;
		return true;
	}

	// Is particle on the right side of visible screen coming back ?
	// If not -> Dead.
	if ((XPos >= SCREEN_WIDTH - 3) && XDir >= 0) // Minimal check : those going right are not coming back for sure.
	{
		Dead = true;
		return true;
	}

	// Is particle above (Actually *under*) visible screen coming back ?
	// If not -> Dead.
	if (YPos <= 1 && YDir <= 0) // Circulars never fall.
	{
		Dead = true;
		return true;
	}

	/* particle cools off */
	ColorIndex--;

	return false;
}

BUBBLE::BUBBLE(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg, const double radius) : Radius(radius), PARTICLE(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	const double alpha = user_uniform_real_distribution<double>(0, std::numbers::pi * 2.);
	const double beta = user_uniform_real_distribution<double>(0, std::numbers::pi * 2.);

	XPos = xOrg + std::cos(alpha) * std::cos(beta) * Radius; // X est le cosinus de l'angle.
	YPos = yOrg + std::sin(alpha) * Radius; // Y est le sinus de l'angle.
	XDir = 0;// -10 + user_uniform_int_distribution<int32_t>(0, 20);
	YDir = -17;// +user_uniform_int_distribution<int32_t>(0, 19);
}

RADIALSPHERE::RADIALSPHERE(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg, const double radius) : Radius(radius), PARTICLE(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	const double alpha = user_uniform_real_distribution<double>(0, std::numbers::pi * 2.);
	const double beta = user_uniform_real_distribution<double>(0, std::numbers::pi * 2.);

	XPos = xOrg + std::cos(alpha) * std::cos(beta) * Radius; // X est le cosinus de l'angle.
	YPos = yOrg + std::sin(alpha) * Radius; // Y est le sinus de l'angle.
	XDir = std::cos(alpha) * std::cos(beta) * 10.;
	YDir = std::sin(alpha) * 10;

	DeathRadius = 30;
}

const bool RADIALSPHERE::setDeath()
{
	XPos += XDir;
	YPos += YDir;
	if (DeathRadius > 0)
		--DeathRadius;

	/* is particle Dead? */
	if (ColorIndex == 0 ||
		(YPos >= SCREEN_HEIGHT - 3) ||
		DeathRadius == 0)
	{
		Dead = true;
		return true;
	}

	// Is particle on the left side of visible screen coming back ?
	// If not -> Dead.
	if (XPos <= 1 && XDir <= 0) // Minimal check : those going left are not coming back for sure.
	{
		Dead = true;
		return true;
	}

	// Is particle on the right side of visible screen coming back ?
	// If not -> Dead.
	if ((XPos >= SCREEN_WIDTH - 3) && XDir >= 0) // Minimal check : those going right are not coming back for sure.
	{
		Dead = true;
		return true;
	}

	// Is particle above (Actually *under*) visible screen coming back ?
	// If not -> Dead.
	if (YPos <= 1 && YDir <= 0) // Circulars never fall.
	{
		Dead = true;
		return true;
	}

	/* particle cools off */
	ColorIndex--;

	return false;
}

WATERFALL::WATERFALL(const int SCREEN_WIDTH, const int SCREEN_HEIGHT, const int xOrg, const int yOrg) : RADIAL(SCREEN_WIDTH, SCREEN_HEIGHT, xOrg, yOrg)
{
	Radius = 30;
}

const bool WATERFALL::setDeath()
{
	XPos += XDir;
	YPos += YDir;
	if (Radius > 0)
		--Radius;

	/* is particle Dead? */
	if (ColorIndex == 0 ||
		(YPos >= SCREEN_HEIGHT - 3))
	{
		Dead = true;
		return true;
	}

	// Is particle on the left side of visible screen coming back ?
	// If not -> Dead.
	if (XPos <= 1 && XDir <= 0) // Minimal check : those going left are not coming back for sure.
	{
		Dead = true;
		return true;
	}

	// Is particle on the right side of visible screen coming back ?
	// If not -> Dead.
	if ((XPos >= SCREEN_WIDTH - 3) && XDir >= 0) // Minimal check : those going right are not coming back for sure.
	{
		Dead = true;
		return true;
	}

	if (Radius == 0)
	{
		// Is particle above (Actually *under*) visible screen coming back ?
		// If not -> Dead.
		if (YPos <= 1)
		{
			int32_t deltaX = XDir;
			if ((deltaX > 0 && (XPos + deltaX >= SCREEN_WIDTH - 3)) ||
				(deltaX < 0 && (XPos + deltaX <= 1)))
			{
				Dead = true;
				return true;
			}
		}
	}

	/* gravity takes over */
	if (Radius == 0)
		YDir++;

	/* particle cools off */
	ColorIndex--;

	return false;
}

#include "Tools.h"

/* ************************************************** /
*  https://www.baeldung.com/cs/convert-color-hsl-rgb  /
* ************************************************** */
uint32_t HSLtoRGB(const double H, const double S, const double L)
{
    auto C = (1. - abs(2. * L - 1.)) * S;
    auto Hp = H / 60.;
    auto X = C * (1 - abs(std::fmod(Hp, 2) - 1));
    double Rp = 0., Gp = 0., Bp = 0.;
    if (Hp <= 1.)
    {
        Rp = C;
        Gp = X;
        Bp = 0.;
    }
    else if (Hp <= 2.)
    {
        Rp = X;
        Gp = C;
        Bp = 0.;
    }
    else if (Hp <= 3.)
    {
        Rp = 0.;
        Gp = C;
        Bp = X;
    }
    else if (Hp <= 4.)
    {
        Rp = 0.;
        Gp = X;
        Bp = C;
    }
    else if (Hp <= 5.)
    {
        Rp = X;
        Gp = 0.;
        Bp = C;
    }
    else if (Hp <= 6.)
    {
        Rp = C;
        Gp = 0.;
        Bp = X;
    }
    auto m = L - C / 2.;

    return (uint32_t((Rp+m) * 255.) << 16) | (uint32_t((Gp + m) * 255.) << 8) | uint32_t((Bp + m) * 255.);
}

uint32_t HSLtoARGB8888(const double H, const double S, const double L, const uint8_t Alpha)
{
    return (uint32_t(Alpha) << 24) | HSLtoRGB(H, S, L);
}

// Adapted from :
// https://lodev.org/cgtutor/fire.html
void GenerateFirePalette(uint32_t* palette, const uint32_t size, const uint32_t Alpha)
{
    for (int x = 0; x < size; ++x)
    {
        //HSLtoRGB is used to generate colors:
        //Hue goes from 0 to 85: red to yellow
        //Saturation is always the maximum: 1
        //Lightness is 0..1 for 0 < x < (size / 2), and 1 for (size/2) < x < size
        auto H = (double)x * 85. / (size - 1.); // Ok.
        auto S = 1.; // [0..1] -> [0..255]
        auto t = ((double)x * 2.) / size;
        auto L = std::min(1., t); // [0..1] -> [0..255]
        if (Alpha > 0)
        {
            auto color = HSLtoARGB8888(H, S, L, Alpha);
            //set the palette to the calculated RGB value + Alpha
            palette[x] = color;
        }
        else
        {
            auto color = HSLtoRGB(H, S, L);
            //set the palette to the calculated RGB value
            // Alpha is set at 0xFF except for black made transparent.
            palette[x] = color != 0 ? (color | 0xFF000000) : color;
        }
    }
}

// Adapted from :
/* https://demo-effects.sourceforge.net/ */
void GenerateFireWithBluePalette(uint32_t* palette, const uint32_t size, const uint32_t Alpha)
{
    memset(palette, 0, size * sizeof(uint32_t));

    for (int i = 0; i < 32; ++i)
    {
        /* black to blue, 32 values*/
        palette[i] |= i << 1;

        /* blue to red, 32 values*/
        palette[i + 32] |= (i << 3) << 16;
        palette[i + 32] |= (64 - (i << 1));

        /*red to yellow, 32 values*/
        palette[i + 64] |= 255 << 16;
        palette[i + 64] |= (i << 3) << 8;

        /* yellow to white, 162 */
        palette[i + 96] |= 255 << 16;
        palette[i + 96] |= 255 << 8;
        palette[i + 96] |= (i << 2);
        palette[i + 128] |= 255 << 16;
        palette[i + 128] |= 255 << 8;
        palette[i + 128] |= 64 + (i << 2);
        palette[i + 160] |= 255 << 16;
        palette[i + 160] |= 255 << 8;
        palette[i + 160] |= 128 + (i << 2);
        palette[i + 192] |= 255 << 16;
        palette[i + 192] |= 255 << 8;
        palette[i + 192] |= 192 + i;
        palette[i + 224] |= 255 << 16;
        palette[i + 224] |= 255 << 8;
        palette[i + 224] |= 224 + i;
    }

    if (Alpha > 0)
    {
        for (int i = 0; i < size; ++i) palette[i] |= Alpha << 24;
    }
    else
    {
        for (int i = 0; i < size; ++i) if (palette[i] != 0) palette[i] |= 0xFF000000;
    }
}

// https://colortutorial.design/grey.html
void GenerateGreyPalette(uint32_t* palette, const uint32_t size, const uint32_t Alpha)
{
    for (int x = 0; x < size; ++x)
    {
        //HSLtoRGB is used to generate colors:
        //Hue therefore meaningless with...
        //...Saturation held to zero
        //Lightness is 0..1 for 0 < x < (size / 2), and 1 for (size/2) < x < size
        auto H = 0.; // Ok.
        auto S = 0.; // [0..1] -> [0..255]
        auto L = (double)x / (double)size;
        if (Alpha > 0)
        {
            auto color = HSLtoARGB8888(H, S, L, Alpha);
            //set the palette to the calculated RGB value + Alpha
            palette[x] = color;
        }
        else
        {
            auto color = HSLtoRGB(H, S, L);
            //set the palette to the calculated RGB value
            // Alpha is set at 0xFF except for black made transparent.
            palette[x] = color != 0 ? (color | 0xFF000000) : color;
        }
    }
}

void GenerateBlueFirePalette(uint32_t* palette, const uint32_t size, const uint32_t Alpha)
{
    for (int x = 0; x < size; ++x)
    {
        //HSLtoRGB is used to generate colors:
        //Hue goes from 240 to 300: B to M
        //Saturation is always the maximum: 1
        //Lightness is 0..1 for 0 < x < (size / 2), and 1 for (size/2) < x < size
        auto H = (double)x * (300 - 240.) / (size - 1.) + 240; // Ok.
        auto S = 1.; // [0..1] -> [0..255]
        auto t = ((double)x * 2.) / size;
        auto L = std::min(1., t); // [0..1] -> [0..255]
        if (Alpha > 0)
        {
            auto color = HSLtoARGB8888(H, S, L, Alpha);
            //set the palette to the calculated RGB value + Alpha
            palette[x] = color;
        }
        else
        {
            auto color = HSLtoRGB(H, S, L);
            //set the palette to the calculated RGB value
            // Alpha is set at 0xFF except for black made transparent.
            palette[x] = color != 0 ? (color | 0xFF000000) : color;
        }
    }
}

void GenerateAnyHSLColourFirePalette(uint32_t* palette, const uint32_t size, const int start, const int end, const uint32_t Alpha)
{
    for (int x = 0; x < size; ++x)
    {
        //HSLtoRGB is used to generate colors:
        //Hue goes from start to end
        //Saturation is always the maximum: 1
        //Lightness is 0..1 for 0 < x < (size / 2), and 1 for (size/2) < x < size
        auto H = (double)x * (double(std::max(start, end)) - double(std::min(start, end))) / (size - 1.) + double(std::min(start, end)); // Ok.
        auto S = 1.; // [0..1] -> [0..255]
        auto t = ((double)x * 2.) / size;
        auto L = std::min(1., t); // [0..1] -> [0..255]
        if (Alpha > 0)
        {
            auto color = HSLtoARGB8888(H, S, L, Alpha);
            //set the palette to the calculated RGB value + Alpha
            palette[x] = color;
        }
        else
        {
            auto color = HSLtoRGB(H, S, L);
            //set the palette to the calculated RGB value
            // Alpha is set at 0xFF except for black made transparent.
            palette[x] = color != 0 ? (color | 0xFF000000) : color;
        }
    }
}

// https://www.hanshq.net/fire.html
void GenerateJaresFirePalette(uint32_t* palette, const uint32_t size, const uint32_t Alpha)
{
	const uint32_t paletteFromJare[256] = {
		/* Jare's original FirePal. */
		#define C(r,g,b) ((((r) * 4) << 16) | ((g) * 4 << 8) | ((b) * 4))
		C(0,   0,   0), C(0,   1,   1), C(0,   4,   5), C(0,   7,   9),
		C(0,   8,  11), C(0,   9,  12), C(15,   6,   8), C(25,   4,   4),
		C(33,   3,   3), C(40,   2,   2), C(48,   2,   2), C(55,   1,   1),
		C(63,   0,   0), C(63,   0,   0), C(63,   3,   0), C(63,   7,   0),
		C(63,  10,   0), C(63,  13,   0), C(63,  16,   0), C(63,  20,   0),
		C(63,  23,   0), C(63,  26,   0), C(63,  29,   0), C(63,  33,   0),
		C(63,  36,   0), C(63,  39,   0), C(63,  39,   0), C(63,  40,   0),
		C(63,  40,   0), C(63,  41,   0), C(63,  42,   0), C(63,  42,   0),
		C(63,  43,   0), C(63,  44,   0), C(63,  44,   0), C(63,  45,   0),
		C(63,  45,   0), C(63,  46,   0), C(63,  47,   0), C(63,  47,   0),
		C(63,  48,   0), C(63,  49,   0), C(63,  49,   0), C(63,  50,   0),
		C(63,  51,   0), C(63,  51,   0), C(63,  52,   0), C(63,  53,   0),
		C(63,  53,   0), C(63,  54,   0), C(63,  55,   0), C(63,  55,   0),
		C(63,  56,   0), C(63,  57,   0), C(63,  57,   0), C(63,  58,   0),
		C(63,  58,   0), C(63,  59,   0), C(63,  60,   0), C(63,  60,   0),
		C(63,  61,   0), C(63,  62,   0), C(63,  62,   0), C(63,  63,   0),
		/* Followed by "white heat". */
		#define W C(63,63,63)
		W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
		W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
		W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
		W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
		W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
		W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
		W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
		W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W
		#undef W
		#undef C
	};

    for (int x = 0; x < size; ++x)
    {
        if (Alpha > 0)
        {
            //set the palette to the calculated RGB value + Alpha
            palette[x] = paletteFromJare[x] | (Alpha << 24);
        }
        else
        {
            //set the palette to the calculated RGB value
            // Alpha is set at 0xFF except for black made transparent.
            palette[x] = paletteFromJare[x] != 0 ? (paletteFromJare[x] | 0xFF000000) : paletteFromJare[x];
        }
    }
}

#include "Tools.h"

/* ************************************************** /
*  https://www.baeldung.com/cs/convert-color-hsl-rgb  /
* ************************************************** */
uint32_t HSLtoRGB(double H, double S, double L)
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

uint32_t HSLtoARGB(double H, double S, double L, uint8_t Alpha)
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

    return (uint32_t(Alpha) << 24) | (uint32_t((Rp + m) * 255.) << 16) | (uint32_t((Gp + m) * 255.) << 8) | uint32_t((Bp + m) * 255.);
}

void GenerateFirePalette(uint32_t* palette, uint32_t Alpha)
{
    //generate the palette
    for (int x = 0; x < 256; x++)
    {
        //HSLtoRGB is used to generate colors:
        //Hue goes from 0 to 85: red to yellow
        //Saturation is always the maximum: 1
        //Lightness is 0..1 for x=0..128, and 1 for x=128..255
        auto H = (double)x / 3.; // Ok.
        auto S = 1.; // [0..1] -> [0..255]
        auto t = ((double)x * 2.) / 255.;
        auto L = std::min(1., t); // [0..1] -> [0..255]
        if (Alpha)
        {
            auto color = HSLtoARGB(H, S, L, Alpha);
            //set the palette to the calculated RGB value
            palette[x] = color;
        }
        else
        {
            auto color = HSLtoRGB(H, S, L);
            //set the palette to the calculated RGB value
            palette[x] = color != 0 ? (color | 0xFF000000) : color;
        }
    }
}
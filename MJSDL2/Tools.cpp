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

    return (uint32_t(0xFF) << 24) | (uint32_t((Rp+m) * 255.) << 16) | (uint32_t((Gp + m) * 255.) << 8) | uint32_t((Bp + m) * 255.);
}

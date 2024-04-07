#include "Tools.h"

/**
 * Converts an HSL color value to RGB. Conversion formula
 * adapted from https://en.wikipedia.org/wiki/HSL_color_space.
 * Assumes h, s, and l are contained in the set [0, 1] and
 * returns r, g, and b in the set [0, 255].
 *
 * @param   {number}  h       The hue
 * @param   {number}  s       The saturation
 * @param   {number}  l       The lightness
 * @return  {Array}           The RGB representation
 */
std::tuple<int, int, int> hslToRgb(int h, int s, int l)
{
    int r, g, b;

    if (s == 0) {
        r = g = b = l; // achromatic
    }
    else {
        const int q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        const int p = 2 * l - q;
        r = hueToRgb(p, q, h + 1 / 3);
        g = hueToRgb(p, q, h);
        b = hueToRgb(p, q, h - 1 / 3);
    }

    return std::make_tuple<int, int, int>((r * 255), round(g * 255), round(b * 255));
}


int hueToRgb(int p, int q, int t)
{
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1 / 6) return p + (q - p) * 6 * t;
    if (t < 1 / 2) return q;
    if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
    return p;
}
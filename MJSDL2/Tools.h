#pragma once
#include <cmath>
#include <tuple>

/*
https://stackoverflow.com/questions/2353211/hsl-to-rgb-color-conversion
*/
std::tuple<int, int, int> hslToRgb(double h, double s, double l);
double hueToRgb(double p, double q, double t);

// https://www.baeldung.com/cs/convert-color-hsl-rgb
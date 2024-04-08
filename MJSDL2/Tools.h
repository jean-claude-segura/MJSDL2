#pragma once
#include <cmath>
#include <tuple>

uint32_t HSLtoRGB(double H, double S, double L);
uint32_t HSLtoARGB(double H, double S, double L, uint8_t Alpha);
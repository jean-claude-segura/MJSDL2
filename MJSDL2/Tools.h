#pragma once
#include <cmath>
#include <tuple>

uint32_t HSLtoRGB(const double H, const double S, const double L);
uint32_t HSLtoARGB8888(const double H, const double S, const double L, const uint8_t Alpha);
void GenerateFirePalette(uint32_t* palette, const uint32_t size, const uint32_t Alpha = 0x00);
void GenerateFireWithBluePalette(uint32_t* palette, const uint32_t size, const uint32_t Alpha = 0x00);
void GenerateGreyPalette(uint32_t* palette, const uint32_t size, const uint32_t Alpha = 0x00);

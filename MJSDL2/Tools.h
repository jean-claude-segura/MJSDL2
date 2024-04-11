#pragma once
#include <cmath>
#include <tuple>

uint32_t HSLtoRGB(double H, double S, double L);
uint32_t HSLtoARGB8888(double H, double S, double L, uint8_t Alpha);
void GenerateFirePalette(uint32_t* palette, uint32_t size, uint32_t Alpha = 0x00);
void GenerateFireWithBluePalette(uint32_t* palette, uint32_t size, uint32_t Alpha = 0x00);
void GenerateGreyPalette(uint32_t* palette, uint32_t size, uint32_t Alpha = 0x00);

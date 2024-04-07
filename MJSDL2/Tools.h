#pragma once
#include <cmath>
#include <tuple>

std::tuple<int, int, int> hslToRgb(int h, int s, int l);
int hueToRgb(int p, int q, int t);

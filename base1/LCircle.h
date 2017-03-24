#ifndef GUARD_LCIRCLE
#define GUARD_LCIRCLE

#include <SDL.h>
#include <cmath>
#include <vector>

#define PI 3.141592
#define SMALL 50

const double TO_DEGREES = 180/PI;

struct Circle {
    int x, y;
    int oldX, oldY;
    int r;

    SDL_Point small_points[SMALL];
};

void renderCircle(Circle* circle, SDL_Color color, SDL_Renderer* gameRenderer);
bool checkCircles(Circle* a, Circle* b);
#endif

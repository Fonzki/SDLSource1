#include "LCircle.h"

#define PI 3.141592

void renderCircle(Circle* circle, SDL_Color color, 
                  SDL_Renderer* gameRenderer){
    if(!circle) return;

    SDL_SetRenderDrawColor(gameRenderer, color.r, color.g, color.b, color.a);

    for(double angle = 0;angle < 2*PI;angle+=2*PI/SMALL){
        int angle_index = (int)(angle/(2*PI/SMALL));
        circle->small_points[angle_index].x = 
            std::cos(angle) * circle->r + circle->x;
        circle->small_points[angle_index].y = 
            std::sin(angle) * circle->r + circle->y;
    }
    SDL_RenderDrawPoints(gameRenderer, circle->small_points, SMALL);
}

bool checkCircles(Circle* a, Circle* b){
    int squareDist = (b->x - a->x) * (b->x - a->x);
    squareDist += (b->y - a->y) * (b->y - a->y);

    int squareRadiiSum = (b->r + a->r) * (b->r + a->r);

    return squareDist < squareRadiiSum;
}

#ifndef GUARD_LPLAYER
#define GUARD_LPLAYER
#include <SDL.h>
#include <string>
#include <vector>
#include <cmath>
#include "LTexture.h"
#include "LCircle.h"

using std::string;      using std::vector; 

class LPlayer{
    public:
        LPlayer(SDL_Renderer* renderer,
                string texturePath, int init_x, int init_y);

        void handleKeyStates();
        void handleEvent(SDL_Event e);
        void act();
        void render(int cameraX, int cameraY);

        void setSpeed(int newSpeed);

        void setVelocity(int xv, int yv);
        int getXVelocity();
        int getYVelocity();

        void setPosition(int xp, int yp);
        int getXPosition();
        int getYPosition();

        SDL_Rect& getHitbox();
        void addCollider(SDL_Rect& box);
        bool checkCollision();
        void clearCollisions();

        SDL_Point getCenter();

        Circle* getHitCircle();
        void addCircleCollider(Circle& circle);
        void enableCircleCollider(int radius);
        bool checkCircleCollisions();
        bool checkCircleCollision(Circle& circle);
        bool isCircleColliding();

    private:
        SDL_Renderer* gameRenderer;
        LTexture myTexture;
        bool movingUp, movingDown, movingLeft, movingRight;
        bool holdPosition;
        int x_position, y_position;
        int x_velocity, y_velocity;
        SDL_Rect hitbox, nextpos, nextX, nextY;
        vector<SDL_Rect> colliders; 
        vector<Circle> circleColliders;
        bool colliding, collidingX, collidingY, circleColliding;
        Circle* hitCircle;
        int speed;
};
#endif

#ifndef GUARD_LBULLET
#define GUARD_LBULLET
#include "..\LTexture.h"
#include "LProjectile.h"
#include "..\LCircle.h"
#include <vector>

class LBullet{
    public:
        LBullet(std::string texturePath, SDL_Renderer* renderer,
                SDL_Point position, int lifeTime);
        ~LBullet();

        void act();
        void render();
        LProjectile* getProjectile();

    private:
        SDL_Renderer* renderer;
        LProjectile* projectile;
        LTexture myTexture;
};
#endif

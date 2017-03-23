#include "LBullet.h"

LBullet::LBullet(std::string texturePath, SDL_Renderer* renderer, 
                 SDL_Point position, int lifetime){
    this->renderer = renderer;
    myTexture.loadFromFile(texturePath, renderer);

    projectile = new LProjectile(renderer, position.x, position.y, 
                                 myTexture.getWidth()/2-1, lifetime);
}
LBullet::~LBullet(){
    delete projectile;
    projectile = NULL;
    myTexture.free();
}

void LBullet::act(){
    projectile->act();
}

void LBullet::render(){
    Circle circle = projectile->getCircle();
    SDL_Color orange = {0xFF, 0x55, 0x00, 0xFF};
    SDL_Point position = {circle.x-myTexture.getWidth()/2, 
                          circle.y-myTexture.getHeight()/2};

    SDL_Point textureCenter = {myTexture.getWidth()/2,
                               myTexture.getHeight()/2};

    myTexture.render(renderer,
                     position.x, position.y,
                     NULL,
                     (projectile->getRenderAngle()+PI/2) * TO_DEGREES,
                     &textureCenter,
                     SDL_FLIP_NONE);
//    renderCircle(&circle, orange, renderer);
}

LProjectile* LBullet::getProjectile(){
    return projectile;
}

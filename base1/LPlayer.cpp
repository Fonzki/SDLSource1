#include "LPlayer.h"

LPlayer::LPlayer(SDL_Renderer* renderer,
                 string texturePath, int init_x, int init_y){
    x_position = init_x;
    y_position = init_y;

    x_velocity = 0;
    y_velocity = 0;

    speed = 1;

    myTexture.loadFromFile(texturePath.c_str(), renderer);

    hitbox.w = myTexture.getWidth();
    hitbox.h = myTexture.getHeight();
    hitbox.x = x_position;
    hitbox.y = y_position;

    nextpos.w = hitbox.w;
    nextpos.h = hitbox.h;
    nextpos.x = hitbox.x;
    nextpos.y = hitbox.y;

    nextX.w = hitbox.w;
    nextX.h = hitbox.h;
    nextX.x = hitbox.x;
    nextX.y = hitbox.y;

    nextY.w = hitbox.w;
    nextY.h = hitbox.h;
    nextY.x = hitbox.x;
    nextY.y = hitbox.y;

   // SDL_Log("w:%d h:%d\n", hitbox.w, hitbox.h);

    gameRenderer = renderer;
}

void LPlayer::handleKeyStates(){
    const Uint8* cKeyStates = SDL_GetKeyboardState(NULL);
    
    if(cKeyStates[SDL_SCANCODE_W]){
        movingUp = true; 
    } else {
        movingUp = false;
    }

    if(cKeyStates[SDL_SCANCODE_S]){
        movingDown = true;
    } else {
        movingDown = false;
    }

    if(cKeyStates[SDL_SCANCODE_A]){
        movingLeft = true;
    } else {
        movingLeft = false;
    }

    if(cKeyStates[SDL_SCANCODE_D]){
        movingRight = true;
    } else {
        movingRight = false;
    }

    if(cKeyStates[SDL_SCANCODE_H]){
        holdPosition = true;
    } else {
        holdPosition = false;
    }

    if(cKeyStates[SDL_SCANCODE_LSHIFT]){
        speed = 1;
    } else {
        speed = 3;
    }
}

void LPlayer::handleEvent(SDL_Event e){

}

void LPlayer::act(){
    //setVelocity(0,0);

    if(movingUp)    y_velocity -= speed;
    if(movingDown)  y_velocity += speed;
    if(movingLeft)  x_velocity -= speed;
    if(movingRight) x_velocity += speed;

    checkCollision();
    if(hitCircle) checkCircleCollisions(); 

    //if(circleColliding) SDL_Log("circle overlap\n");

    /*SDL_Log("colliding: %s collidingX: %s collidingY: %s\n",
            colliding ? "true" : "false", collidingX ? "true" : "false",
            collidingY ? "true" : "false");*/

    if(!colliding && !holdPosition){
        x_position += x_velocity;
        y_position += y_velocity;
    } else if(colliding) {
        if(!collidingX) x_position += x_velocity;
        if(!collidingY) y_position += y_velocity;
    }

    hitbox.x = x_position;
    hitbox.y = y_position;

    float drag = .6f;
    float dragX = x_velocity * drag;
    float dragY = y_velocity * drag;

    setVelocity((int)dragX, (int)dragY);
}

void LPlayer::render(int cameraX, int cameraY){
    myTexture.render(gameRenderer,
                     x_position-cameraX, y_position-cameraY,
                     NULL, 0, NULL, SDL_FLIP_NONE);
/*
    hitbox.x = hitbox.x-cameraX;
    hitbox.y = hitbox.y-cameraY;

    nextpos.x = nextpos.x-cameraX;
    nextpos.y = nextpos.y-cameraY;

    nextX.x = nextpos.x;
    nextX.y = hitbox.y;

    nextY.x = hitbox.x;
    nextY.y = nextpos.y;

    SDL_SetRenderDrawColor(gameRenderer, 0x00, 0x11, 0xFF, 0xFF);
    SDL_RenderDrawRect(gameRenderer, &hitbox);

    SDL_SetRenderDrawColor(gameRenderer, 0x11, 0xFF, 0x00, 0xFF);
    SDL_RenderDrawRect(gameRenderer, &nextpos);

    SDL_SetRenderDrawColor(gameRenderer, 0xFF, 0x88, 0x00, 0xFF);
    SDL_RenderDrawRect(gameRenderer, &nextX);

    SDL_SetRenderDrawColor(gameRenderer, 0xFF, 0x00, 0xFF, 0xFF);
    SDL_RenderDrawRect(gameRenderer, &nextY);
    */
}

void LPlayer::setVelocity(int xv, int yv){
    x_velocity = xv;
    y_velocity = yv;
}

int LPlayer::getXVelocity(){
    return x_velocity;
}

int LPlayer::getYVelocity(){
    return y_velocity;
}

void LPlayer::setPosition(int xp, int yp){
    x_position = xp;
    y_position = yp;
}

int LPlayer::getXPosition(){
    return x_position;
}

int LPlayer::getYPosition(){
    return y_position;
}

SDL_Rect& LPlayer::getHitbox(){
    return hitbox;
}

void LPlayer::addCollider(SDL_Rect& box){
    colliders.push_back(box);
}

bool LPlayer::checkCollision(){
    if(colliders.empty()){ 
        nextpos.x = x_position + x_velocity;
        nextpos.y = y_position + y_velocity;
        return false;
    }

    int myLeftSide, yourLeftSide;
    int myRightSide, yourRightSide;
    int myTopSide, yourTopSide;
    int myBottomSide, yourBottomSide;

    nextpos.x = x_position + x_velocity;
    nextpos.y = y_position + y_velocity;

    myLeftSide = nextpos.x;
    myRightSide = nextpos.x + nextpos.w;
    myTopSide = y_position;
    myBottomSide = y_position + hitbox.h;
    

    vector<SDL_Rect>::iterator i = colliders.begin();
    collidingX = false;

    while(i != colliders.end()){
        yourLeftSide = i->x;
        yourRightSide = i->x + i->w;
        yourTopSide = i->y;
        yourBottomSide = i->y + i->h;
        
        if(myBottomSide <= yourTopSide 
            || myTopSide >= yourBottomSide
            || myRightSide <= yourLeftSide 
            || myLeftSide >= yourRightSide){
            ++i;
            continue;
        }
        collidingX = true;
        break;
    }
    
    i = colliders.begin();
    
    collidingY = false;

    myLeftSide = x_position;
    myRightSide = x_position + hitbox.w;
    myTopSide = nextpos.y;
    myBottomSide = nextpos.y + nextpos.h;

    while(i != colliders.end()){
        yourLeftSide = i->x;
        yourRightSide = i->x + i->w;
        yourTopSide = i->y;
        yourBottomSide = i->y + i->h;
        
        if(myBottomSide <= yourTopSide 
            || myTopSide >= yourBottomSide
            || myRightSide <= yourLeftSide 
            || myLeftSide >= yourRightSide){
            ++i;
            continue;
        }
        collidingY = true;
        break;
    }
    
    i = colliders.begin();
    
    colliding = false;

    myLeftSide = nextpos.x;
    myRightSide = nextpos.x + nextpos.w;
    myTopSide = nextpos.y;
    myBottomSide = nextpos.y + nextpos.h;

    while(i != colliders.end()){
        yourLeftSide = i->x;
        yourRightSide = i->x + i->w;
        yourTopSide = i->y;
        yourBottomSide = i->y + i->h;
        
        if(myBottomSide <= yourTopSide 
            || myTopSide >= yourBottomSide
            || myRightSide <= yourLeftSide 
            || myLeftSide >= yourRightSide){
            ++i;
            continue;
        }
        colliding = true;
        break;
    }

    return colliding;
}

SDL_Point LPlayer::getCenter(){
    SDL_Point center;

    center.x = nextpos.x + hitbox.w / 2;
    center.y = nextpos.y + hitbox.h / 2;

    return center;
}

void LPlayer::enableCircleCollider(int radius){
    hitCircle = new Circle;
    SDL_Point center = getCenter();
    hitCircle->x = center.x;
    hitCircle->y = center.y;
    hitCircle->r = radius;
    circleColliding = false;
}

int distanceSquared(int ax, int ay, int bx, int by){
    return (bx - ax) * (bx - ax) + (by - ay) * (by - ay); 
}

bool LPlayer::checkCircleCollisions(){
    SDL_Point center = getCenter();
    hitCircle->x = center.x;
    hitCircle->y = center.y;
    //hitCircle->r = hitbox.w/2;

    if(circleColliders.empty()){
        return false;
    }

    vector<Circle>::iterator i = circleColliders.begin();
    for(;i!=circleColliders.end();i++){
        int bothRadiiSquared = hitCircle->r + i->r;
        bothRadiiSquared *= bothRadiiSquared;
        int distSquared = distanceSquared(hitCircle->x, hitCircle->y,
                                          i->x, i->y);
        if(distSquared < bothRadiiSquared){
            circleColliding = true;
            return true;
        }
    }

    circleColliding = false;
    return false;
}

bool LPlayer::checkCircleCollision(Circle& circle){
    int bothRadiiSquared = hitCircle->r + circle.r;
    bothRadiiSquared *= bothRadiiSquared;
    int distSquared = distanceSquared(hitCircle->x, hitCircle->y,
                                    circle.x, circle.y);
    if(distSquared < bothRadiiSquared){
        return true;
    }
    return false;
}

Circle* LPlayer::getHitCircle(){
    return hitCircle;
}

void LPlayer::addCircleCollider(Circle& circle){
    circleColliders.push_back(circle);
}

bool LPlayer::isCircleColliding(){
    return circleColliding;
}

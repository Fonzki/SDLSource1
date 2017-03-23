#include "LProjectile.h"

LProjectile::LProjectile(SDL_Renderer* gameRenderer, int x, int y,
                         int hitRadius, int lifetime){
    isDead = false;
    position.x = x;
    position.y = y;

    hitCircle.x = x;
    hitCircle.y = y;
    hitCircle.r = hitRadius;

    renderer = gameRenderer;
    
    timer = new LTimer();
    timer->start();

    goingForward = true;
    motion = none;

    speed = 2;
    this->lifetime = lifetime;

    usingCustomPoints = false;
    travelTime = -1;
    ticksSinceLastAction = 0;
    ticksAtLastAction = timer->getTicks();

    orange.r = 0xFF;
    orange.g = 0x55;
    orange.b = 0;
    orange.a = 0xFF;
}

LProjectile::~LProjectile(){
    delete timer;
    timer = NULL;
}

void LProjectile::act(){
    int ticks = timer->getTicks();
    int ticksLeft = lifetime - ticks;
    accel = ticks / 1000;
    ticksSinceLastAction = ticks - ticksAtLastAction;
    //SDL_Log("dt: %d\n", ticksSinceLastAction);
    isDead = (ticksLeft < 0);
    //isDead = false;
    if(isDead){ 
        hitCircle.r = 0;
        motion = none;
        return;
    }

    usingCustomPoints = false;
    //SDL_Log("motion: %d\n", motion);
    switch(motion){
        case horizontal:{
            if(goingForward){
                position.x+=speed;
            } else {
                position.x-=speed;
            }
            break;
        }
        case vertical:{
            if(goingForward){
                position.y+=speed;
            } else {
                position.y-=speed;
            }
            break;
        }
        case lerp:{
            doLerp();
            break;
        }
        case orbit:{
            doOrbit();
            //SDL_Log("radius: %d\n", orbitRadius);
            break;            
        }
        case radiation:{
            radiate();
            break;
        }
        case spiral:{
            doSpiral();
            break;
        }
        case polar:{
            doPolar();
            break;
        }
        case custom:{
            usingCustomPoints = true;
            break;
        }
        case none:{
            // do nothing
            break;
        }
    }

    //SDL_Log("ticks to death: %d\n", ticksLeft);

    //if(usingCustomPoints);
    
    hitCircle.x = position.x;
    hitCircle.y = position.y;
    ticksAtLastAction = ticks;
}

void LProjectile::render(){
    renderCircle(&hitCircle, orange, renderer);
}

void LProjectile::setMotionDirection(bool goForward){
    goingForward = goForward;
}

void LProjectile::setCustomMotion(){

}

void LProjectile::setSpeed(double newSpeed){
    speed = newSpeed;
}

double LProjectile::getSpeed(){
    return speed;
}

void LProjectile::setMotion(Motion newMotion){
    motion = newMotion;
}

void LProjectile::setLerp(SDL_Point end, int timeToComplete, 
                          bool stopAtEnd){
    doneLerp = false;
    motion = lerp;
    lerpOrigin = position;
    lerpEnd = end;
    travelTime = timeToComplete;
    ticksAtLerpStart = timer->getTicks();
    stopAtLerpEnd = stopAtEnd;

    int dx = lerpEnd.x - position.x;
    int dy = lerpEnd.y - position.y; 
    renderAngle = std::atan2(dy, dx);
}

void LProjectile::doLerp(){
    int ticksSinceLerpStart = timer->getTicks() - ticksAtLerpStart;
    double lerpRatio = (double)ticksSinceLerpStart / (double) travelTime;
    
    int lerpX = (int)((1-lerpRatio) * lerpOrigin.x + (lerpRatio) * lerpEnd.x);
    int lerpY = (int)((1-lerpRatio) * lerpOrigin.y + (lerpRatio) * lerpEnd.y);

    if(lerpRatio < 1 || !stopAtLerpEnd){
        position.x = lerpX;
        position.y = lerpY;
    } else {
        position.x = lerpEnd.x;
        position.y = lerpEnd.y;
        doneLerp = true;
    }
}

bool LProjectile::getDeathState(){
    return isDead;
}

void LProjectile::setOrbit(SDL_Point center,int radius, int period){
    motion = orbit;
    travelTime = period;
    orbitCenter = center;
    
    int xDiff = position.x - orbitCenter.x;
    int yDiff = position.y - orbitCenter.y;

    currentAngle = std::atan2((double)yDiff, (double)xDiff);
    //if(yDiff < 0) currentAngle += PI/2;

    orbitRadius = radius;
}

void LProjectile::setOrbit(double startingAngle, int radius, int period){
    motion = orbit;
    travelTime = period;
    currentAngle = startingAngle;
    orbitCenter = position;
    orbitRadius = radius;
}

void LProjectile::doOrbit(){
    double deltaAngle = 2.0*PI/*(double)orbitRadius*/ / (double)travelTime;
    deltaAngle *= (double)ticksSinceLastAction;
    renderAngle = 0;
    if(goingForward){
        currentAngle += deltaAngle;
    } else {
        currentAngle -= deltaAngle;
        renderAngle += PI;
    }

    int newX = std::cos(currentAngle) * orbitRadius + orbitCenter.x;
    int newY = std::sin(currentAngle) * orbitRadius + orbitCenter.y;

    renderAngle += currentAngle + PI/2;

    position.x = newX;
    position.y = newY;
}

void LProjectile::setRadiation(SDL_Point source){
    motion = radiation;
    radiationSource = source;

    int xDiff = position.x - radiationSource.x;
    int yDiff = position.y - radiationSource.y;

    currentAngle = std::atan2((double)yDiff, (double)xDiff);
    renderAngle = currentAngle;

    if(!goingForward)
        renderAngle += PI;

    radiationRadius = xDiff * xDiff + yDiff * yDiff;
    radiationRadius = std::sqrt(radiationRadius);
}

void LProjectile::setRadiation(double angleInRadians){
    motion = radiation;
    SDL_Point currentPosition = {position.x, position.y};

    radiationSource = currentPosition;
    radiationRadius = 0;

    currentAngle = angleInRadians;
    renderAngle = currentAngle;
}

void LProjectile::radiate(){
    //speed += accel;
    if(goingForward)
        radiationRadius += speed;
    else 
        radiationRadius -= speed;

    double xPosition = std::cos(currentAngle) * radiationRadius;
    xPosition += radiationSource.x;

    double yPosition = std::sin(currentAngle) * radiationRadius;
    yPosition += radiationSource.y;

    position.x = xPosition;
    position.y = yPosition;
}

void LProjectile::setSpiral(double startingAngle, int period){
    setOrbit(startingAngle,5,period);
    motion = spiral;
}

void LProjectile::doSpiral(){
    doOrbit();
    if(goingForward)
        orbitRadius += speed;
    else 
        orbitRadius -= speed;
}

void LProjectile::setPolar(double startingAngle, int period, 
                           int polarConstant,
                           double cosCoef, double cosArgCoef,
                           double sinCoef, double sinArgCoef,
                           double tilt){
    setOrbit(startingAngle,0,period);
    this->cosCoef = cosCoef;
    this->cosArgCoef = cosArgCoef;
    this->sinCoef = sinCoef;
    this->sinArgCoef = sinArgCoef;
    
    this->polarConstant = polarConstant;
    this->tilt = tilt;
    motion = polar;
}

void LProjectile::doPolar(){
    int ix = position.x;
    int iy = position.y;
    orbitRadius = cosCoef * std::cos(cosArgCoef * currentAngle + tilt) +
                  sinCoef * std::sin(sinArgCoef * currentAngle + tilt) +
                  polarConstant;
    /*
    SDL_Log("%fcos(%fx) + %fsin(%fx) + %d\n", cosCoef, cosArgCoef,
            sinCoef, sinArgCoef, polarConstant);
    SDL_Log("radius = %d\n", orbitRadius);
    //*/
    doOrbit();
    if(currentAngle > 2*PI) currentAngle -= 2*PI;
    
    int fx = position.x;
    int fy = position.y;

    renderAngle = std::atan2(fy-iy, fx-ix);
    //renderAngle = currentAngle+PI/2;
}

Circle& LProjectile::getCircle(){
    return hitCircle;
}

double LProjectile::getRenderAngle(){
    return renderAngle;
}

void LProjectile::setPosition(SDL_Point newPos){
    prevPosition = position;
    position.x = newPos.x;
    position.y = newPos.y;
}

void LProjectile::killProjectile(){
    isDead = true;
    lifetime = 0;
}

SDL_Point LProjectile::getPosition(){
    return position;
}

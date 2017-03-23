#include "LEntity.h"

LEntity::LEntity(Tagtype type, SDL_Renderer* renderer, int x, int y){
    this->type = type;
    this->renderer = renderer;
    
    switch(type){
        case bullet:{
            texture.loadFromFile(PATH_BULLET, renderer);
            HP = 1;
            break;
        }
        case enemy:{
            texture.loadFromFile(PATH_ENEMY, renderer);
            HP = 1;
            break;
        }
        case player_bullet:{
            texture.loadFromFile(PATH_PBULLET, renderer);
            HP = 1;
            break;
        }
    }

    mount = new LProjectile(renderer, x, y, texture.getWidth()/2, 600000);
    hitCircle = mount->getCircle();

    framesLeft = 0;
    firstCall = true;
}

LEntity::~LEntity(){
    delete mount;
    mount = NULL;

    renderer = NULL;
    texture.free();
}

Motion LEntity::getMotion(){
    return currentMotion;
}

void pushMotion(std::queue<MotionInfo> &motionQ, MotionInfo nextMotion){
    motionQ.push(nextMotion);
}

void LEntity::pushMotion(MotionInfo nextMotion){
    motionQueue.push(nextMotion);
}

void LEntity::processMotionInfo(){
    if(!firstCall){
        motionQueue.pop();
    }
    if(motionQueue.empty()){
        framesLeft = 0;
        return;
    }

    MotionInfo front = motionQueue.front();
    mount->setMotionDirection(front.goForward);
    Motion m = front.motion;
    currentMotion = m;
    switch(m){
        case horizontal:{
            if(front.params.size() < 1) {
                SDL_Log("not enough horizontal parameters\n");
                break;
            }
            mount->setMotion(m);
            mount->setSpeed((int)front.params[0]);
            framesLeft = front.frames;
            break;
        }
        case vertical:{
                //SDL_Log("moving vertically...\n");
            if(front.params.size() < 1) {
                SDL_Log("not enough vertical parameters\n");
                break;
            }
            mount->setMotion(m);
            mount->setSpeed((int)front.params[0]);
            framesLeft = front.frames;
            break;
        }
        case lerp:{
            if(front.params.size() < 4){
                SDL_Log("not enough lerp params\n");
                break;
            }
            SDL_Point end = {(int)front.params[0], (int)front.params[1]};
            int timeToComplete = (int)front.params[2];
            bool stopAtEnd = (bool) front.params[3];
            
            mount->setLerp(end,timeToComplete,stopAtEnd);
            
            framesLeft = front.frames;
            break;
        }
        case orbit:{
            if(front.params.size() < 4){
                //SDL_Log("using orbit 2\n");
                if(front.params.size() < 3){
                    SDL_Log("not enough params for orbit 1 or 2\n");
                    break;
                }
                double startingAngle = front.params[0];
                int radius = (int) front.params[1];
                int period = (int) front.params[2];
                mount->setOrbit(startingAngle, radius, period);

                framesLeft = front.frames;
                break;
            }
            SDL_Point center = {(int)front.params[0], (int)front.params[1]};
            int radius = (int) front.params[2];
            int period = (int) front.params[3];

            mount->setOrbit(center, radius, period);
            framesLeft = front.frames;

            break;            
        }
        case radiation:{
            if(front.params.size() < 2){
                if(front.params.size() < 1){
                    SDL_Log("not enough params to radiate 1 or 2\n");
                    break;
                }
                
                double angleInRadians = front.params[0];
                mount->setRadiation(angleInRadians);
                framesLeft = front.frames;
                break;
            }
            SDL_Point source = {(int) front.params[0], (int) front.params[1]};
            
            mount->setRadiation(source);
            framesLeft = front.frames;
            break;
        }
        case spiral:{
            if(front.params.size() < 2){
                SDL_Log("not enough params to spiral\n");
                break;
            }
            double startingAngle = front.params[0];
            int period = (int) front.params[1];

            mount->setSpiral(startingAngle, period);
            framesLeft = front.frames;
            break;
        }
        case polar:{
            if(front.params.size() < 8){
                SDL_Log("not enough params for polar motion\n");
                break;
            }
            double startingAngle = front.params[0];
            int period = (int) front.params[1];
            int polarConstant = (int) front.params[2];
            double cosCoef = front.params[3];
            double cosArgCoef = front.params[4];
            double sinCoef = front.params[5];
            double sinArgCoef = front.params[6];
            double tilt = front.params[7];

            mount->setPolar(startingAngle, period, polarConstant, 
                            cosCoef, cosArgCoef, sinCoef, sinArgCoef,
                            tilt);

            framesLeft = front.frames;

            break;
        }
        case custom:{
            break;
        }
        case none:{
            // do nothing
            break;
        }
        case kill:{
            mount->killProjectile();
            break;
        }
    }
    if(firstCall) firstCall = false;
}

void LEntity::act(){
    if(type == bullet){
        //SDL_Log("pew!\n");
        if(!motionQueue.empty()){
            //SDL_Log("Motion: %d\n", motionQueue.front().motion);
        }
    }

    if(mount->getDeathState() || HP <= 0){
        delete mount;
        mount = NULL;
    }
    
    if(!mount) return;

    if(!subEntities.empty()){
        if(subEntities.front().framesBeforeSpawning > 0)
            subEntities.front().framesBeforeSpawning--;
        else 
            subEntities.front().framesBeforeSpawning = 0;
    }

    if(framesLeft <= 0 && !motionQueue.empty()){
        processMotionInfo();
    }
    
    mount->act();
    hitCircle = mount->getCircle();
}

void LEntity::render(){
    if(!mount){ return;}
    SDL_Point texCenter = {texture.getWidth()/2, texture.getHeight()/2};
    texture.render(renderer, 
                   hitCircle.x - texture.getWidth()/2, 
                   hitCircle.y - texture.getHeight()/2,
                   NULL,
                   //(mount->getRenderAngle() - PI/2) * TO_DEGREES,
                   0.0,
                   NULL, SDL_FLIP_NONE);
    SDL_Color orange = {0xFF, 0x55, 0, 0xFF};
    renderCircle(&hitCircle, orange, renderer);
    if(framesLeft > 0) framesLeft--;
}

bool LEntity::isDead(){
    return mount->getDeathState() || HP <= 0;
}

void LEntity::pushEntity(EntityInfo ent){
    subEntities.push(ent);
}

void printEntityInfo(EntityInfo info){
    SDL_Log("type: %d\n", info.type);
    SDL_Log("position: (%d, %d) \n", info.position.x, info.position.y);
    SDL_Log("MQ count: %d\n", info.motionQueue.size());
    SDL_Log("SQ count: %d\n", !info.subEnts.size());
    SDL_Log("frames: %d\n", info.framesBeforeSpawning);
}

LEntity* spawnEntity(EntityInfo params){
    LEntity* spawn = new LEntity(params.type, params.renderer,
                                 params.position.x, params.position.y);
    
        //printEntityInfo(params.subEnts.front());
    while(!params.motionQueue.empty()){
        spawn->pushMotion(params.motionQueue.front());
        params.motionQueue.pop();
    }

    while(!params.subEnts.empty()){
         spawn->pushEntity(params.subEnts.front());
         //SDL_Log("motion: %d\n", 
                 //params.subEnts.front().motionQueue.front().motion);
         params.subEnts.pop();
    }
    return spawn;
}

bool LEntity::isSubQueueEmpty(){
    return subEntities.empty();
}

bool LEntity::spawnFromSubQueue(){
    if(subEntities.empty()){
        return false;
    }

    return subEntities.front().framesBeforeSpawning == 0;
}

EntityInfo* LEntity::getFrontSubEntityInfo(){
    EntityInfo* subEnt = new EntityInfo();
    subEnt->type = invalid;
    if(subEntities.empty()){
        return subEnt; 
    }

    delete subEnt;
    subEnt = &(subEntities.front());
    subEntities.pop();

    if(!subEntities.empty())
        framesTillSubSpawn = subEntities.front().framesBeforeSpawning--;
    else 
        framesTillSubSpawn = 0;

    return subEnt;
}

SDL_Point LEntity::getPosition(){
    return mount->getPosition();
}

Circle* LEntity::getCircle(){
    return &hitCircle;
}

Tagtype LEntity::getType(){
    return type;
}

void LEntity::takeDamage(int dmg){
    HP -= dmg;
}

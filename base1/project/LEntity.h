#ifndef GUARD_LENTITY
#define GUARD_LENTITY

#include <SDL.h>
#include <vector>
#include <queue>
#include <string>
#include "LProjectile.h"
#include "..\LCircle.h"
#include "..\LTexture.h"

const std::string PATH_BULLET("images/bullet.png");
const std::string PATH_ENEMY("images/enemy.png");
const std::string PATH_PBULLET("images/pbullet.png");

enum Tagtype {
    invalid,
    enemy,
    bullet,
    player_bullet
};

// a single motion that can be applied to any entity
// that can be done one after another to create animation patterns
struct MotionInfo {
    Motion motion;
    std::vector<double> params;
    int frames;
    bool goForward;    
};

void pushMotion(std::queue<MotionInfo> &motionQ, MotionInfo nextMotion);

// encodes an entity
// has a tag to mark what type it is
// has a queue to store its motions
// has a queue to store any entities it may spawn during its life
struct EntityInfo {
    Tagtype type;
    SDL_Point position;
    SDL_Renderer* renderer;
    std::queue<MotionInfo> motionQueue;
    int framesBeforeSpawning;
    std::queue<EntityInfo> subEnts;
}; 

class LEntity {
    public:
        LEntity(Tagtype type, SDL_Renderer* renderer, int x, int y);
        ~LEntity();
        
        void pushMotion(MotionInfo nextMotion);
        void act();
        void render();

        bool isDead();
		
        bool isSubQueueEmpty();
        int framesTillSubSpawn;
		
        // gets entity info to spawn
		EntityInfo* getFrontSubEntityInfo();
        bool spawnFromSubQueue();
		
		// add an entity to this entity's sub queue
        void pushEntity(EntityInfo ent);

        SDL_Point getPosition(); 
        Motion getMotion();
        Circle* getCircle();

        Tagtype getType();
        void takeDamage(int dmg);
    private:
        void processMotionInfo();

        int HP;

        std::queue<MotionInfo> motionQueue;
        int framesLeft;
        Tagtype type;
		
		// an entity is basically a sprite "riding" on a projectile
		// controlled by its motion queue hence the name mount
        LProjectile* mount;
        LTexture texture;
        Circle hitCircle;
        SDL_Renderer* renderer;
        bool firstCall;

        Motion currentMotion;
        std::queue<EntityInfo> subEntities;
};

LEntity* spawnEntity(EntityInfo params);
void printEntityInfo(EntityInfo info);

#endif

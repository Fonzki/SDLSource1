#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <list>
#include <cmath>
#include <string>
#include <cstdlib>
#include "LProjectile.h"
#include "LBullet.h"
#include "LEntity.h"
#include "..\LTimer.h"
#include "..\LCircle.h"
#include "..\LPlayer.h"
#include "..\LTexture.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int FPS = 60;

class JGame {
    public:
        JGame();
        ~JGame();

        void play();

        LEntity* ent;
    private:
        // initialize SDL window & renderer, SDL_image, SDL_font
        bool init();
        void reset();

        bool loadMedia();
        
        // triggers corresponding game element functions
        void handleEvents();
        void handleKeystates();
        void act();
        void render();

        // clean up
        void close();

        // creates a bullet on the player and shoots it to the 
        // top of the screen
        void shootFromPlayer();

        void spawnOrbittingEnemy(bool fromLeft);

        bool initSuccess, loadSuccess;
        bool playing;
        SDL_Window* gameWindow;
        SDL_Renderer* gameRenderer;
        LTimer* gameTimer;

		// Container for each wave of enemies to spawn
        std::queue<std::queue<EntityInfo> > entityWaves;
		
        std::list<LEntity*> enemyEntities;
        std::list<LEntity*> allyEntities;

        Circle centerMarker;
        Circle hLinear;
        int hLinLimit;
        int hLinModifier;

	    int frames;
        int score;
        int maxScore;
        bool gameJustEnded;

        LPlayer* player;
        LTexture background;
		
		// # of invincibility frames
        int iFrames;

		// Functions that add a wave of enemies to entityWaves
        void wave1();
        void RLRL_Wave();
        void level1();
        void stall(int frames);
        void alternateSliderPair();
        void clumpSpawn();
};

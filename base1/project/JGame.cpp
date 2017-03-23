#include "JGame.h"

JGame::JGame(){
    initSuccess = true;
    loadSuccess = true;
    
    if(!init()){
        SDL_Log("failed to initialize\n");
        initSuccess = false;
    }

    if(!loadMedia()){
        SDL_Log("failed to load media\n");
        loadSuccess = false;
    }
}

JGame::~JGame(){
    close();
}

void JGame::play(){
    playing = initSuccess;

    while(playing){
        handleEvents();
        handleKeystates();
        act();
        render();
    }
}

bool JGame::init(){
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        ("failed to initialize SDL! SDL Error: %s\n", SDL_GetError());
        initSuccess = false;  
    }

    gameWindow = SDL_CreateWindow("Projectiles!", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                               SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(!gameWindow){
        SDL_Log("failed to create the window! SDL Error: %s\n",
                SDL_GetError());
        initSuccess = false;
    }

    gameRenderer = SDL_CreateRenderer(gameWindow, -1, 
                                      SDL_RENDERER_ACCELERATED |
                                      SDL_RENDERER_PRESENTVSYNC);
    
    if(!gameRenderer){
        SDL_Log("failed to create the renderer! SDL Error: %s\n",
                SDL_GetError());
        initSuccess = false;
    }

    SDL_SetRenderDrawColor(gameRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
   
    int imgFlags = IMG_INIT_PNG; 
    if(!(IMG_Init(imgFlags) & imgFlags)){
        SDL_Log("failed to initialize SDL_image! SDL Error: %s\n",
                IMG_GetError());
        initSuccess = false; 
    }

    if(TTF_Init() == -1){
        SDL_Log("failed to initialize SDL_ttf! SDL Error: %s\n",
                TTF_GetError());
        initSuccess = false;
    }

    loadSuccess = loadMedia();

    gameTimer = new LTimer();
    gameTimer->start();

    centerMarker.x = 320;
    centerMarker.y = 240;
    centerMarker.r = 5;

    hLinear.x = 64;
    hLinear.y = 50;
    hLinear.r = 5;

    hLinLimit = 576;
    hLinModifier = 1;

    frames = 0;

    std::string texturePath("images/player.png");
    player = new LPlayer(gameRenderer, texturePath, 
                         SCREEN_WIDTH/2, SCREEN_HEIGHT-40);
    
    player->enableCircleCollider(4);
    iFrames = 0;

    maxScore = 0;

	// play level 1
    level1();

    score = 0;
    gameJustEnded = true;
    return initSuccess && loadSuccess;
}

void JGame::reset(){
    player->setPosition(SCREEN_WIDTH/2, SCREEN_HEIGHT-40);
    maxScore = 0;
    level1();
    gameJustEnded = true;
    score = 0;
}

bool JGame::loadMedia(){
    std::string bgTexturePath = "images/background.png";
    if(!background.loadFromFile(bgTexturePath, gameRenderer)){
        SDL_Log("failed to load bg!\n");
        return false;
    }
    return true;
}

void JGame::handleEvents(){
    SDL_Event e;
    while(SDL_PollEvent(&e) > 0){
        if(e.type == SDL_QUIT){
            playing = false;
            return;
        }
        if(e.type == SDL_KEYUP){
            switch(e.key.keysym.sym){
                case SDLK_f:{
                    SDL_Log("ally b: %d\n", allyEntities.size());
                    break;
                }
                case SDLK_g:{
                    SDL_Log("emenies: %d\n", enemyEntities.size());
                    break;
                }
                case SDLK_r:{
                    allyEntities.clear();
                    enemyEntities.clear();
                    while(!entityWaves.empty()){
                        entityWaves.pop();
                    }
                    reset();
                }

            }
        }
    }
}

void JGame::handleKeystates(){
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    player->handleKeyStates();
    if(keystates[SDL_SCANCODE_SPACE]){
        if(frames % 4 == 0) shootFromPlayer();
    }
}

void JGame::act(){
	// if there is another wave of enemies, spawn them
	// else the game is over and print out player score
    if(!entityWaves.empty()){
        if(!entityWaves.front().empty()){
            std::queue<EntityInfo> *currentWave = &entityWaves.front();
            if(currentWave->front().framesBeforeSpawning <= 0){
                enemyEntities.push_back(spawnEntity(currentWave->front()));
                currentWave->pop();
            } else {
                currentWave->front().framesBeforeSpawning--;
            }
        } else {
            entityWaves.pop();
        }
    } else {
        if(enemyEntities.empty() && gameJustEnded){
            SDL_Log("final score: %d/%d\n", score, maxScore);
            if(!score) score = 1;
            int performance = maxScore * 10 / score - 10;
            switch(performance){
                case 0:{
                    if(score == maxScore){
                        SDL_Log("PERFECT! YOU HAVE ACHIEVED EXCELLENCE!\n");
                        break;
                    }
                }
                case 1:{
                    SDL_Log("GREAT! YOU CAN STILL DO BETTER!\n");
                    break;
                }
                case 2:{
                    SDL_Log("YOU'RE GETTING THE HANG OF IT!\n");
                    break;
                }
                case 3:{
                    SDL_Log("YOU CAN DO THIS! I BELIEVE IN YOU!\n");
                    break;
                }
                default: {
                    SDL_Log("KEEP TRYING! DON'T GIVE UP!\n");
                    break;
                }
            }
            SDL_Log("R to retry\n");
            gameJustEnded = false;
        }
    }
	
	// ignore this -------------------------------------
    if(hLinear.x > hLinLimit || hLinear.x < 64){
        hLinModifier *= -1;
    }
    hLinear.x += hLinModifier;
	//--------------------------------------------------

	// do player actions
    player->act();

	// do friendly entity actions
    std::list<LEntity*>::iterator i = allyEntities.begin();
    for(;i != allyEntities.end();i++){
        if(!(*i)->isDead()){
            (*i)->act();
        } else {
            i = allyEntities.erase(i);
        }
    }
    
    Circle* pC = player->getHitCircle();
	
	// do enemy collisions with player or bullet
	// on hit with player add 2 seconds of iFrames
	// on hit with bullet take damage, 'hurt' the bullet back so it is erased in the next loop
	// if enemy has a bullet to spawn, in its subqueue mark it to be spawned to the game
    std::list<EntityInfo> enemiesToSpawn;
    std::list<LEntity*>::iterator j = enemyEntities.begin();
    for(;j != enemyEntities.end();j++){
        if(!(*j)->isDead()){
            if((*j)->getType() == enemy){
                i = allyEntities.begin();
                for(;i != allyEntities.end();i++){
                    if((*i)->getType()== player_bullet){
                        if(checkCircles((*j)->getCircle(), (*i)->getCircle())){
                            (*j)->takeDamage(1);
                            (*i)->takeDamage(1);
                        }
                    }
                }
            }
            if(!(*j)->isDead()){
                (*j)->act();
                Circle* eC = (*j)->getCircle();        
                if(checkCircles(eC,pC) && iFrames == 0){
                    iFrames = FPS * 2;
                }
                if((*j)->spawnFromSubQueue()){
                    EntityInfo* frontInfo = (*j)->getFrontSubEntityInfo();
                    frontInfo->position = (*j)->getPosition();
                
                    enemiesToSpawn.push_back(*frontInfo);
                }
            } else {
                SDL_Log("enemy down! +50!\n");
                score += 50;
                if(!iFrames){
                    SDL_Log("NO BLINKING BONUS +50!");
                    score += 50;
                }
            }
        } else {
            j = enemyEntities.erase(j);
        }
    }

	// spawn enemy bullets to the field and lerp them at the player
    std::list<EntityInfo>::iterator k = enemiesToSpawn.begin();
    for(;k != enemiesToSpawn.end();k++){
        EntityInfo info = (*k);

        LEntity* spawn = new LEntity(info.type, info.renderer,
                                     info.position.x, info.position.y);
        MotionInfo* lerpToPlayer = new MotionInfo();
        MotionInfo* end = new MotionInfo();

        lerpToPlayer->motion = lerp;
        lerpToPlayer->frames = 4 * FPS;
        lerpToPlayer->goForward = true;

        lerpToPlayer->params.push_back(player->getCenter().x);
        lerpToPlayer->params.push_back(player->getCenter().y);
        lerpToPlayer->params.push_back(lerpToPlayer->frames / 2 * 1000/FPS);
        lerpToPlayer->params.push_back(false);

        end->motion = kill;
        end->frames = 1;

        spawn->pushMotion(*lerpToPlayer);
        spawn->pushMotion(*end);

        enemyEntities.push_back(spawn);
    }
}

void JGame::render(){
    SDL_SetRenderDrawColor(gameRenderer, 0,0,0,0xFF);
    SDL_RenderClear(gameRenderer);

    background.render(gameRenderer, 0, 0, NULL, 0, NULL, SDL_FLIP_NONE);

    //run render functions below

    SDL_Color green = {0,0xFF,0x55,0xFF};
    renderCircle(&hLinear, green, gameRenderer);

    player->render(0,0);

    SDL_Color red = {0xFF,0,0,0xFF};
    renderCircle(&centerMarker, red, gameRenderer);

    SDL_Color cyan = {0,0xFF,0xFF,0xFF};

    if(!iFrames){
        renderCircle(player->getHitCircle(), cyan, gameRenderer);
    } else {
        if(iFrames % 6 < 2){
			// make player hitcircle blink
            renderCircle(player->getHitCircle(), cyan, gameRenderer);
        }
    }

    std::list<LEntity*>::iterator i = allyEntities.begin();
    for(;i != allyEntities.end();i++){
        (*i)->render();
    }

    std::list<LEntity*>::iterator j = enemyEntities.begin();
    for(;j != enemyEntities.end();j++){
        (*j)->render();
    }

    SDL_RenderPresent(gameRenderer);
    frames++;
    if(iFrames) iFrames--;
}


void JGame::close(){
    delete player;
    player = NULL;

    SDL_DestroyWindow(gameWindow);
    gameWindow = NULL;

    SDL_DestroyRenderer(gameRenderer);
    gameRenderer = NULL;

    IMG_Quit();
    TTF_Quit();
    SDL_Quit(); 
}

void JGame::shootFromPlayer(){
    LEntity* temp;
    int px = player->getCenter().x;
    int py = player->getCenter().y;

    temp = new LEntity(player_bullet, gameRenderer, px, py);

    MotionInfo* upFast;
    MotionInfo* end;
    upFast = new MotionInfo();
    end = new MotionInfo();
    
    upFast->motion = vertical;
    upFast->goForward = false;
    upFast->params.push_back(8);
    upFast->frames = 2 * FPS;

    end->motion = kill;
    end->frames = 1;

    temp->pushMotion(*upFast);
    temp->pushMotion(*end);

    allyEntities.push_back(temp);

    delete upFast;
    delete end;

    upFast = NULL;
    end = NULL;
}

void addBullet(EntityInfo *super, int frameDelay){
    EntityInfo* sub = new EntityInfo();
    
    sub->type = bullet;
    sub->renderer = super->renderer;
    sub->framesBeforeSpawning = frameDelay;
    super->subEnts.push(*sub);
}

void JGame::wave1(){
    std::queue<EntityInfo> theWave;

    EntityInfo* rightSlider = new EntityInfo();
    
    rightSlider->type = enemy;
    rightSlider->position.x = 0;
    rightSlider->position.y = 100;
    rightSlider->renderer = gameRenderer;

    MotionInfo* moveRight = new MotionInfo();
    MotionInfo* end = new MotionInfo();

    moveRight->motion = horizontal;
    moveRight->frames = 6 * FPS;
    moveRight->goForward = true;

    moveRight->params.push_back(3);

    end->motion = kill;
    end->frames = 1;

    pushMotion(rightSlider->motionQueue, *moveRight);
    pushMotion(rightSlider->motionQueue, *end);

    EntityInfo* sliders[4];
    int waveSize = 4;
    
    for(int i = 0; i<4; i++){
        sliders[i] = new EntityInfo();
        memcpy(sliders[i], rightSlider, sizeof(EntityInfo));
        sliders[i]->position.y += 100 * i;
        sliders[i]->framesBeforeSpawning = FPS;
        addBullet(sliders[i], 0);
        addBullet(sliders[i], FPS);
        addBullet(sliders[i], FPS);
        addBullet(sliders[i], FPS);
        theWave.push(*sliders[i]);
    }

    entityWaves.push(theWave);

    maxScore += waveSize * 100;

    delete rightSlider;
    delete moveRight;
    delete end;
}

void JGame::RLRL_Wave(){
    std::queue<EntityInfo> theWave;

    EntityInfo* downSlider = new EntityInfo();
    
    downSlider->type = enemy;
    downSlider->position.x = 0;
    downSlider->position.y = 0;
    downSlider->renderer = gameRenderer;

    MotionInfo* moveDown = new MotionInfo();
    MotionInfo* end = new MotionInfo();

    moveDown->motion = vertical;
    moveDown->frames = 6 * FPS;
    moveDown->goForward = true;

    moveDown->params.push_back(3);

    end->motion = kill;
    end->frames = 1;

    pushMotion(downSlider->motionQueue, *moveDown);
    pushMotion(downSlider->motionQueue, *end);

    EntityInfo* sliders[2];
    int waveSize = 2;
    
    for(int i = 0; i<2; i++){
        sliders[i] = new EntityInfo();
        memcpy(sliders[i], downSlider, sizeof(EntityInfo));
        sliders[i]->position.x = SCREEN_WIDTH / 2 + 100 - 200 * i;
        sliders[i]->framesBeforeSpawning = FPS;
        addBullet(sliders[i], 0);
        theWave.push(*sliders[i]);
    }

    entityWaves.push(theWave);

    maxScore += waveSize * 100;

    delete downSlider;
    delete moveDown;
    delete end;
}

// stalls by waiting <frames> and spawning an entity that instantly dies
void JGame::stall(int frames){
    std::queue<EntityInfo> theWave;

    EntityInfo* instantlyDie = new EntityInfo();

    MotionInfo* end = new MotionInfo();

    end->motion = kill;
    end->frames = 1;

    pushMotion(instantlyDie->motionQueue, *end);

    EntityInfo* dummy = new EntityInfo();
    memcpy(dummy, instantlyDie, sizeof(EntityInfo));
    dummy->framesBeforeSpawning = frames;
    theWave.push(*dummy);

    entityWaves.push(theWave);

    delete instantlyDie;
    delete end;

}

void JGame::alternateSliderPair(){
    std::queue<EntityInfo> theWave;
    int waveSize = 2;

    EntityInfo* TSlider = new EntityInfo();
    EntityInfo* BSlider = new EntityInfo();

    MotionInfo* right = new MotionInfo();
    MotionInfo* left = new MotionInfo();
    MotionInfo* end = new MotionInfo();

    TSlider->position.y = SCREEN_HEIGHT / 2 - 75;
    BSlider->position.y = SCREEN_HEIGHT / 2 + 75;

    TSlider->position.x = 0;
    BSlider->position.x = SCREEN_WIDTH;

    right->motion = horizontal;
    left->motion = horizontal;

    right->goForward = true;
    left->goForward = false;

    right->frames = 6 * FPS;
    left->frames = 6 * FPS;
    
    right->params.push_back(2);
    left->params.push_back(2);

    end->motion = kill;
    end->frames = 1;

    pushMotion(TSlider->motionQueue, *right);
    pushMotion(BSlider->motionQueue, *left);

    pushMotion(TSlider->motionQueue, *end);
    pushMotion(BSlider->motionQueue, *end);

    TSlider->type = enemy;
    BSlider->type = enemy;

    TSlider->renderer = gameRenderer;
    BSlider->renderer = gameRenderer;

    TSlider->framesBeforeSpawning = 0;
    BSlider->framesBeforeSpawning = 0;
    
    addBullet(TSlider, 0);
    addBullet(BSlider, 0);
    
    theWave.push(*TSlider);
    theWave.push(*BSlider);

    entityWaves.push(theWave);
    maxScore += waveSize * 100;

    delete right;
    delete left;
    delete end;
}

void JGame::clumpSpawn(){
    std::queue<EntityInfo> theWave;
    int waveSize = 30;

    EntityInfo* clump = new EntityInfo();

    clump->type = enemy;
    clump->renderer = gameRenderer;

    int startingY = -25;
    clump->position.y = startingY;

    MotionInfo* end = new MotionInfo();

    end->motion = kill;
    end->frames = 1;


    EntityInfo* enemies[30];
    for(int i = 0; i<waveSize; i++){
        enemies[i] = new EntityInfo();
        memcpy(enemies[i], clump, sizeof(EntityInfo));

        int clumpLeft = 50;
        int hSpacing = 56;
        int variation = 20;
        int vSpacing = 56;
        int clumpTop = 70;
        int lerpX = (i%10) * hSpacing + rand() % variation + clumpLeft; 
        int downY = ((i/10)+1) * vSpacing + clumpTop + rand() % variation;

        enemies[i]->position.x = lerpX;
        enemies[i]->framesBeforeSpawning = 0;

        MotionInfo* lerpDown = new MotionInfo();
        MotionInfo* lerpUp = new MotionInfo();

        lerpDown->motion = lerp;
        lerpDown->frames = 4*FPS;
        lerpDown->goForward = true;

        lerpDown->params.push_back(lerpX);
        lerpDown->params.push_back(downY);
        lerpDown->params.push_back(6*FPS);
        lerpDown->params.push_back(true);

        lerpUp->motion = lerp;
        lerpUp->frames = 3 * FPS;
        lerpUp->goForward = true;
        
        lerpUp->params.push_back(lerpX);
        lerpUp->params.push_back(startingY);
        lerpUp->params.push_back(16 * FPS);
        lerpUp->params.push_back(false);

        pushMotion(enemies[i]->motionQueue, *lerpDown);
        pushMotion(enemies[i]->motionQueue, *lerpUp);
        pushMotion(enemies[i]->motionQueue, *end);
        
        theWave.push(*enemies[i]);
    }

    entityWaves.push(theWave);
    maxScore += waveSize * 100;

    delete clump;
    delete end;
}

void JGame::level1(){
    RLRL_Wave();
    RLRL_Wave();
    stall(FPS);
    wave1();
    wave1();
    stall(FPS);
    int ASPCount = 10;
    for(int i=0; i<ASPCount; i++){
        alternateSliderPair();
        stall(FPS/2);
    }
    stall(4*FPS);
    clumpSpawn();
    stall(6*FPS);
    clumpSpawn();
    stall(6*FPS);
}

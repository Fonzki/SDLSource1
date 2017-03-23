#ifndef GUARD_LPROJECTILE
#define GUARD_LPROJECTILE

#include <SDL.h>
#include <vector>
#include <cmath>
#include "..\LTimer.h"
#include "..\LCircle.h"

enum Motion {
    none,
    horizontal,
    vertical,
    lerp,
    orbit,
    radiation,
    spiral,
    polar,
    custom,
    kill
};

class LProjectile {
    public:
        LProjectile(SDL_Renderer* gameRenderer, int x, int y,
                    int hitRadius, int lifetime);
        ~LProjectile();
        
        void act();
        void render();

        void setSpeed(double newSpeed);
        double getSpeed();

        void setMotionDirection(bool goForward);
        void setMotion(Motion newMotion);
        void setCustomMotion();
        void setLerp(SDL_Point end, int time, bool stopAtEnd);

        // for orbiting, forward = ccw
        // orbiting sets moves the projectile on the circumfrence of
        // the specified circle the period is stored in travelTime
        void setOrbit(SDL_Point center, int radius, int period);

        // similar to before but takes an initial angle and orbits 
        // the circle around its original position
        void setOrbit(double startingAngle, int radius, int period);

        // for radiation, forward = outward
        // radiating moves the projectile along the line between
        // the projectile's position and the source by calculating
        // the angle and increasing the distance between the projectile
        // and the circle
        void setRadiation(SDL_Point source);

        // sets the starting position as the source and moves at the
        // angle specified
        void setRadiation(double angleInRadians);

        // for spiraling, forward = ccw
        // a spiral is an orbit with a increasing radius 
        // so this function will initiate an orbit and expand the radius
        // using the speed property
        void setSpiral(double startingAngle, int period);

        // forward = ccw
        // orbits but adjusts radius according to the
        // specified polar function
        void setPolar(double startingAngle, int period,
                      int polarConstant,
                      double cosCoef, double cosArgCoef,
                      double sinCoef, double sinArgCoef,
                      double tilt);

        bool getDeathState();
        double getRenderAngle();
        Circle& getCircle();
        void setPosition(SDL_Point newPos);
        SDL_Point getPosition();

        void killProjectile();


    private:
        SDL_Renderer* renderer;
        SDL_Color orange;

        bool goingForward, usingCustomPoints;
        double speed;
        double accel;

        LTimer* timer;
        int lifetime;
        bool isDead;

        SDL_Point prevPosition;
        SDL_Point position;

        double renderAngle;

        Circle hitCircle;

        Motion motion;
        std::vector<SDL_Point> motionLocus;
        SDL_Point lerpOrigin, lerpEnd;
        int ticksAtLerpStart;
        bool doneLerp, stopAtLerpEnd;
        int travelTime;
        int ticksAtLastAction;
        int ticksSinceLastAction;

        // assumes lerp parameters: lerpOrigin, lerpEnd, & travelTime
        // are valid
        //
        // peforms lerp calculation and assigns the position based on the
        // ratio between the start time and the travel time 
        void doLerp();

        // assumes orbit parameters: orbitCenter & orbitRadius
        // are valid
        //
        // calculates next angle by dividing the circumfrence by the 
        // period in ticks then assigns the position with sin/cos
        void doOrbit();

        SDL_Point orbitCenter;
        double currentAngle;
        int orbitRadius;

        SDL_Point radiationSource;
        double radiationRadius;

        // assumes radiation parameters: radiationSource
        // is valid
        //
        // moves the projectile along the line connecting the projectile
        // and source point
        void radiate();

        // assumes setSpiral has been called first
        // calls doOrbit and then increases the orbit radius
        void doSpiral();

        double cosCoef, cosArgCoef, sinCoef, sinArgCoef, tilt;
        int polarConstant;
        void doPolar();
};

#endif

/*
 * Modification History
 *
 * 2004-June-18   Jason Rohrer
 * Created.
 *
 * 2004-June-20   Jason Rohrer
 * Added fade-out of last explosion frame.  Added scaling.
 *
 * 2004-August-6   Jason Rohrer
 * Added enemy shot sounds.
 *
 * 2004-August-17   Jason Rohrer
 * Removed outdated enemy shot sound.  Still need to replace explosion sound.
 *
 * 2004-October-14   Jason Rohrer
 * Added function for exploding all active enemies.
 *
 * 2005-August-21   Jason Rohrer
 * Added fade-in upon enemy creation.
 * Made target-switching rotations smooth.
 */



#ifndef ENEMY_MANAGER_INCLUDED
#define ENEMY_MANAGER_INCLUDED



#include "Enemy.h"
#include "BulletSound.h"
#include "ShipBulletManager.h"
#include "SculptureManager.h"
#include "SoundPlayer.h"
#include "SoundSamples.h"

#include "minorGems/util/SimpleVector.h"
#include "minorGems/math/geometry/Vector3D.h"
#include "minorGems/math/geometry/Angle3D.h"



/**
 * A class that manages and draws all enemies in the environment.
 *
 * @author Jason Rohrer.
 */
class EnemyManager {


    public:


        
        /**
         * Constructs a manager.
         *
         * @param inEnemyTemplate the class to use to draw all enemies.
         *   Will be destroyed by this class.
         * @param inEnemyScale the value to multiply enemy size by.
         * @param inExplosionScale the value to multiply explosion size by.
         * @param inEnemyVelocity the speed of enemies.
         * @param inSculptureManager the sculpture manager.
         *   Must be destroyed by caller after this class is destroyed.
         * @param inShipBulletManager the ship bullet manager.
         *   Must be destroyed by caller after this class is destroyed.
         * @param inEnemyBulletManager the enemy bullet manager.
         *   Must be destroyed by caller after this class is destroyed.
         * @param inEnemyBulletRange the range of enemy bullets.
         * @param inEnemyBulletBaseVelocity the velocity of enemy bullets.
         * @param inEnemyBulletsPerSecond the number of bullets an
         *   enemy can fire per second.
         * @param inSoundPlayer the player to send enemy sounds to.
         *   Must be destroyed by caller after this class is destroyed.
         * @param inEnemyExplosionSoundTemplate the class to use to play
         *   enemy explosion sounds.
         *   Will be destroyed by this class.
         * @param inWorldWidth the width of the world (for wrap-around).
         * @param inWorldHeight the height of the world (for wrap-around).
         */
        EnemyManager( Enemy *inEnemyTemplate,
                      double inEnemyScale,
                      double inExplosionScale,
                      double inEnemyVelocity,
                      SculptureManager *inSculptureManager,
                      ShipBulletManager *inShipBulletManager,
                      ShipBulletManager *inEnemyBulletManager,
                      double inEnemyBulletRange,
                      double inEnemyBulletBaseVelocity,
                      double inEnemyBulletsPerSecond,
                      SoundPlayer *inSoundPlayer,
                      BulletSound *inEnemyExplosionSoundTemplate,
                      double inWorldWidth,
                      double inWorldHeight );


        
        virtual ~EnemyManager();

        

        /**
         * Adds a enemy to this manager.
         *
         * @param inEnemyShapeParameter a parameter in the range [0,1] to
         *   control the shape of the enemy.
         * @param inExplosionShapeParameter a parameter in the range [0,1] to
         *   control the shape of the enemy's explosion.
         * @param inExplosionTimeInSeconds the time it takes this enemy
         *   to complete its explosion.
         * @param inBulletCloseParameter a parameter in the range [0,1] to
         *   control the shape of the enemy's close-range bullets.
         * @param inBulletFarParameter a parameter in the range [0,1] to
         *   control the shape of the enemy's far-range bullets.
         * @param inStartingPosition the starting position of this enemy.
         *   Will be destroyed by this class.
         * @param inStartingRotation the starting rotation angle. 
         *   Will be destroyed by this class.
         */
        void addEnemy( double inEnemyShapeParameter,
                       double inExplosionShapeParameter,
                       double inExplosionTimeInSeconds,
                       double inBulletCloseParameter,
                       double inBulletFarParameter,
                       Vector3D *inStartingPosition,
                       Angle3D *inStartingRotation  );

        

        /**
         * Causes all active enemies to start exploding.
         */
        void explodeAllEnemies();

        
        
        /**
         * Gets the number of enemies.
         *
         * @return the number of enemies.
         */
        int getEnemyCount();


        
        /**
         * Tell this manager that time has passed.
         *
         * @param inTimeDeltaInSeconds the amount of time that has passed.
         * @param inShipPosition the position of the ship (enemies head
         *   toward it).
         *   Must be destroyed by caller.
         */
        void passTime( double inTimeDeltaInSeconds,
                       Vector3D *inShipPosition );

        
        
        /**
         * Gets drawable objects for all enemies in their current
         * positions/states.
         *
         * @return all enemies as a collection of drawable objects.
         *   Vector and objects must be destroyed by caller.
         */
        SimpleVector<DrawableObject *> *getDrawableObjects();


        
    protected:

        Enemy *mEnemyTemplate;
        SculptureManager *mSculptureManager;
        ShipBulletManager *mShipBulletManager;
        ShipBulletManager *mEnemyBulletManager;

        double mEnemyBulletRange;
        double mEnemyBulletBaseVelocity;
        double mEnemyBulletsPerSecond;
        double mEnemyTimeBetweenBullets;

        SoundPlayer *mSoundPlayer;
        BulletSound *mEnemyExplosionSoundTemplate;
        
        double mEnemyScale;
        double mExplosionScale;
        double mEnemyVelocity;
        
        double mMaxXPosition;
        double mMinXPosition;
        double mMaxYPosition;
        double mMinYPosition;
        
        
        SimpleVector<double> *mEnemyShapeParameters;
        SimpleVector<double> *mExplosionShapeParameters;
        
        // the explosion time for each enemy
        SimpleVector<double> *mExplosionTimesInSeconds;

        // flags indicating which enemies are exploding
        SimpleVector<char> *mCurrentlyExplodingFlags;
        
        // how far each enemy is towards a complete explosion, in [0,1]
        SimpleVector<double> *mExplosionProgress;

        // how far each enemy is through its explosion fade-out or its
        // initial creation fade-in, in [0,1]
        SimpleVector<double> *mFadeProgress;

        SimpleVector<double> *mBulletCloseParameters;
        SimpleVector<double> *mBulletFarParameters;
        SimpleVector<double> *mTimesSinceLastBullet;
        
        SimpleVector<Vector3D *> *mCurrentPositions;
        SimpleVector<Angle3D *> *mCurrentAnglesToPointAt;
        
        SimpleVector<double> *mCurrentRadii;


        // rotation rates can vary across a enemy's lifespan, so we must
        // keep a current angle for each enemy and adjust the angle
        // for each time delta
        SimpleVector<Angle3D *> *mCurrentRotations;


        SimpleVector<double> *mCurrentRotationRates;

        SimpleVector<char> *mSholdBeDestroyedFlags;

        // parameters for each enemy in the range [0,1] representing
        // distance of enemy from the ship
        SimpleVector<double> *mShipDistanceParameters;
    };



#endif

/*
 * Modification History
 *
 * 2004-June-29   Jason Rohrer
 * Created.
 *
 * 2004-August-19   Jason Rohrer
 * Added explosion sounds.
 *
 * 2004-August-24   Jason Rohrer
 * Added extra parameter for distance from ship.
 *
 * 2004-August-25   Jason Rohrer
 * Added function to check for boss death.
 *
 * 2005-August-22   Jason Rohrer
 * Started work on boss damage graphics.
 */



#ifndef BOSS_MANAGER_INCLUDED
#define BOSS_MANAGER_INCLUDED



#include "Enemy.h"
#include "ShipBulletManager.h"
#include "SoundPlayer.h"
#include "BulletSound.h"

#include "minorGems/util/SimpleVector.h"
#include "minorGems/math/geometry/Vector3D.h"
#include "minorGems/math/geometry/Angle3D.h"



/**
 * A class that manages and draws all enemies in the environment.
 *
 * @author Jason Rohrer.
 */
class BossManager {


    public:


        
        /**
         * Constructs a manager.
         *
         * @param inBossTemplate the class to use to draw all enemies.
         *   Will be destroyed by this class.
         * @param inBossScale the value to multiply boss size by.
         * @param inExplosionScale the value to multiply explosion size by.
         * @param inExplosionTimeInSeconds the time it takes the explosion
         *   to complete.
         * @param inExplosionShapeParameter the parameter that controls
         *   explosion shape, in [0,1].
         * @param inBossMinVelocity the minimum speed of the boss.
         * @param inBossMaxVelocity the maximum speed of the boss.
         * @param inShipBulletManager the ship bullet manager.
         *   Must be destroyed by caller after this class is destroyed.
         * @param inBossBulletManager the boss bullet manager.
         *   Must be destroyed by caller after this class is destroyed.
         * @param inBossDamageManager the manager for boss damage objects.
         *   Must be destroyed by caller after this class is destroyed.
         * @param inBossBulletRange the range of boss bullets.
         * @param inBossBulletVelocity the velocity of boss bullets.
         * @param inMinBossBulletsPerSecond the minimum number of bullets a
         *   boss can fire per second.
         * @param inMaxBossBulletsPerSecond the maximum number of bullets a
         *   boss can fire per second.
         * @param inBossDamageTime how long each damage object is displayed
         *   in seconds.
         * @param inTimeUntilFullShootingPower how long the ship has to
         *   be in range before the boss reaches its full shooting power.
         * @param inMaxBossHealth the maximum health level of the boss.
         * @param inBossHealthRecoveryRate how fast the boss recovers health.
         * @param inBossPosition the position of the boss.
         *   Will be destroyed when this class is destroyed.
         * @param inSoundPlayer the player to send boss sounds to.
         *   Must be destroyed by caller after this class is destroyed.
         * @param inBossExplosionSoundTemplate the class to use to play
         *   boss explosion sounds.
         *   Will be destroyed by this class.
         */
        BossManager( Enemy *inBossTemplate,
                     double inBossScale,
                     double inExplosionScale,
                     double inExplosionTimeInSeconds,
                     double inExplosionShapeParameter,
                     double inBossMinVelocity,
                     double inBossMaxVelocity,
                     ShipBulletManager *inShipBulletManager,
                     ShipBulletManager *inBossBulletManager,
                     ShipBulletManager *inBossDamageManager,
                     double inBossBulletRange,
                     double inBossBulletBaseVelocity,
                     double inMinBossBulletsPerSecond,
                     double inMaxBossBulletsPerSecond,
                     double inBossDamageTime,
                     double inTimeUntilFullShootingPower,
                     double inMaxBossHealth,
                     double inBossHealthRecoveryRate,
                     Vector3D *inBossPosition,
                     SoundPlayer *inSoundPlayer,
                     BulletSound *inBossExplosionSoundTemplate );


        
        virtual ~BossManager();



        /**
         * Tell this manager that time has passed.
         *
         * @param inTimeDeltaInSeconds the amount of time that has passed.
         * @param inShipPosition the position of the ship (boss shoots
         *   toward it).
         *   Must be destroyed by caller.
         * @param inShipVelocity the velocity of the ship (boss compensates
         *   shooting aim).
         *   Must be destroyed by caller.
         */
        void passTime( double inTimeDeltaInSeconds,
                       Vector3D *inShipPosition,
                       Vector3D *inShipVelocity );

        
        
        /**
         * Gets drawable objects for the boss.
         *
         * @return boss as a collection of drawable objects.
         *   Vector and objects must be destroyed by caller.
         */
        SimpleVector<DrawableObject *> *getDrawableObjects();



        /**
         * Gets the position of the boss.
         *
         * @return the position.
         *   Must be destroyed by caller.
         */
        Vector3D *getBossPosition();



        /**
         * Gets whether the boss is dead (explosion complete).
         *
         * @return true if the boss is dead.
         */
        char isBossDead();


        
    protected:

        Enemy *mBossTemplate;
        
        ShipBulletManager *mShipBulletManager;
        ShipBulletManager *mBossBulletManager;
        ShipBulletManager *mBossDamageManager;

        double mBossScale;
        double mExplosionScale;
        double mExplosionTimeInSeconds;
        double mExplosionShapeParameter;

        double mBossMinVelocity;
        double mBossMaxVelocity;
            
        
        double mBossBulletRange;
        double mBossBulletBaseVelocity;

        double mMinBossTimeBetweenBullets;
        double mMaxBossTimeBetweenBullets;

        double mBossDamageTime;
        
        double mTimeUntilFullShootingPower;

        double mMaxBossHealth;
        double mBossHealth;

        double mHealthRecoveryRate;

        Vector3D *mBossPosition;


        
        SoundPlayer *mSoundPlayer;
        BulletSound *mBossExplosionSoundTemplate;

        
        double mBossDistanceFromCenter;
        
        
        char mCurrentlyExploding;

        
        double mExplosionProgress;
        double mExplosionFadeProgress;

        double mTimeSinceLastBullet;

        double mTimeSinceShipEnteredRange;
        double mAngerLevel;
        
        double mCurrentRadius;

        double mCurrentRotationRate;
        Angle3D *mCurrentRotation;

        double mShipDistanceParameter;
        
    };



#endif

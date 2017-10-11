/*
 * Modification History
 *
 * 2004-June-29   Jason Rohrer
 * Created.
 *
 * 2004-July-3   Jason Rohrer
 * Improved boss ship-in-range detection.
 *
 * 2004-July-5   Jason Rohrer
 * Fixed memory leaks.
 *
 * 2004-August-19   Jason Rohrer
 * Added explosion sounds.
 *
 * 2004-August-20   Jason Rohrer
 * Added sound priorities.
 *
 * 2004-August-25   Jason Rohrer
 * Added function to check for boss death.
 *
 * 2004-August-29   Jason Rohrer
 * Improved distance-from-target compression formula.
 *
 * 2004-October-21   Jason Rohrer
 * Fixed bug that caused explosion progress to get larger than 1.
 *
 * 2005-August-22   Jason Rohrer
 * Started work on boss damage graphics.
 *
 * 2005-August-23   Jason Rohrer
 * Finished boss damage graphics.
 *
 * 2005-August-26   Jason Rohrer
 * Added limit to number of damage graphics on screen at once.
 *
 * 2005-August-29   Jason Rohrer
 * Removed print message.
 */



#include "BossManager.h"



#include <math.h>



BossManager::BossManager( Enemy *inBossTemplate,
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
                          BulletSound *inBossExplosionSoundTemplate )
    : mBossTemplate( inBossTemplate ),
      mShipBulletManager( inShipBulletManager ),
      mBossBulletManager( inBossBulletManager ),
      mBossDamageManager( inBossDamageManager ),
      mBossScale( inBossScale ),
      mExplosionScale( inExplosionScale ),
      mExplosionTimeInSeconds( inExplosionTimeInSeconds ),
      mExplosionShapeParameter( inExplosionShapeParameter ),
      mBossMinVelocity( inBossMinVelocity ),
      mBossMaxVelocity( inBossMaxVelocity ),
      mBossBulletRange( inBossBulletRange ),
      mBossBulletBaseVelocity( inBossBulletBaseVelocity ),
      mMinBossTimeBetweenBullets( 1 / inMinBossBulletsPerSecond ),
      mMaxBossTimeBetweenBullets( 1 / inMaxBossBulletsPerSecond ),
      mBossDamageTime( inBossDamageTime ),
      mTimeUntilFullShootingPower( inTimeUntilFullShootingPower ),
      mMaxBossHealth( inMaxBossHealth ),
      mBossHealth( inMaxBossHealth ),
      mHealthRecoveryRate( inBossHealthRecoveryRate ),
      mBossPosition( inBossPosition ),
      mSoundPlayer( inSoundPlayer ),
      mBossExplosionSoundTemplate( inBossExplosionSoundTemplate ),
      mCurrentlyExploding( false ),
      mExplosionProgress( 0 ),
      mExplosionFadeProgress( 0 ),
      mTimeSinceLastBullet( 0 ),
      mTimeSinceShipEnteredRange( 0 ),
      mAngerLevel( 0 ),
      mCurrentRadius( 0 ),
      mCurrentRotationRate( 0 ),
      mCurrentRotation( new Angle3D( 0, 0, 0 ) ),
      mShipDistanceParameter( 0 ) {

    mBossDistanceFromCenter = mBossPosition->getLength();
    
    }
      


        
BossManager::~BossManager() {
    delete mBossTemplate;
    delete mBossPosition;
    delete mCurrentRotation;

    delete mBossExplosionSoundTemplate;
    }



void BossManager::passTime( double inTimeDeltaInSeconds,
                            Vector3D *inShipPosition,
                            Vector3D *inShipVelocity ) {

    mCurrentRotation->mZ += mCurrentRotationRate * inTimeDeltaInSeconds;

    
    // check if we are being hit by ship bullets
            
    double bulletPowerNearBoss =
        mShipBulletManager->getBulletPowerInCircle(
            mBossPosition, mCurrentRadius );
            
    if( bulletPowerNearBoss > 0 ) {
        // boss hit by bullet

        // scale power by time delta (power is power per second)
        mBossHealth -= bulletPowerNearBoss * inTimeDeltaInSeconds;

        

        
        if( mBossHealth <= 0 ) {

            if( !mCurrentlyExploding ) {
                // start explosion
                mCurrentlyExploding = true;

                // play the sound
                PlayableSound *sound =
                    mBossExplosionSoundTemplate->getPlayableSound(
                        mAngerLevel,
                        mExplosionShapeParameter,
                        mSoundPlayer->getSampleRate() );

                // boss explosion is high priority
                mSoundPlayer->playSoundNow( sound, true, 1 );
                
                delete sound;
                }

            // don't allow it to go below 0
            mBossHealth = 0;
            }
        }
    else {
        // not hit, recover health
        mBossHealth += mHealthRecoveryRate * inTimeDeltaInSeconds;

        if( mBossHealth > mMaxBossHealth ) {
            mBossHealth = mMaxBossHealth;
            }
        }

        
    if( mCurrentlyExploding ) {
        // boss is exploding
            
        // compute new explosion progress

        // convert time delta to a progress increment for the progress
        // range [0,1]
        double progressFractionDelta =
                inTimeDeltaInSeconds / mExplosionTimeInSeconds;

        mExplosionProgress += progressFractionDelta;

        if( mExplosionProgress > 1 ) {
            mExplosionProgress = 1;
            } 
            
        if( mExplosionProgress == 1 ) {
            // end of explosion
            // fade out last frame

            mExplosionFadeProgress += progressFractionDelta;

            if( mExplosionFadeProgress >= 1 ) {
                // explosion done fading
                mExplosionFadeProgress = 1;
                }
            }        
        }


    


    if( !mCurrentlyExploding ) {
    
        double distanceToShip = inShipPosition->getDistance( mBossPosition );
    
    
        // compute anger level based on how long ship has been in range
    

        if( distanceToShip <= mBossBulletRange ) {
            // ship in range, get angry
            mTimeSinceShipEnteredRange += inTimeDeltaInSeconds;
            }
        else {
            // ship out of range, calm down
            mTimeSinceShipEnteredRange -= inTimeDeltaInSeconds;
            
            if( mTimeSinceShipEnteredRange < 0 ) {
                mTimeSinceShipEnteredRange = 0;
                }
            }

        mAngerLevel = mTimeSinceShipEnteredRange / mTimeUntilFullShootingPower;

        if( mAngerLevel > 1 ) {
            mAngerLevel = 1;
            }


        // boss moves in a cirular path
        double currentVelocity =
            mAngerLevel * ( mBossMaxVelocity - mBossMinVelocity ) +
            mBossMinVelocity;

        // move perpendicular to center radius vector

        // center radius vector is our position vector

        Vector3D *bossVelocityVector = new Vector3D( mBossPosition );

        Angle3D *perpendicularAngle = new Angle3D( 0, 0, M_PI / 2 );

        bossVelocityVector->rotate( perpendicularAngle );

        delete perpendicularAngle;
        
        bossVelocityVector->normalize();

        bossVelocityVector->scale( currentVelocity );

        Vector3D *bossMoveVector = new Vector3D( bossVelocityVector );

        bossMoveVector->scale( inTimeDeltaInSeconds );

        
        mBossPosition->add( bossMoveVector );
        delete bossMoveVector;

        // lock down distance from center to avoid cumulative errors
        double newDistanceFromCenter = mBossPosition->getLength();

        double adjustment = mBossDistanceFromCenter / newDistanceFromCenter;

        mBossPosition->scale( adjustment );
        
        
        
        double currentTimeBetweenBullets =
            mAngerLevel *
                ( mMaxBossTimeBetweenBullets - mMinBossTimeBetweenBullets ) +
            mMinBossTimeBetweenBullets;


        mTimeSinceLastBullet += inTimeDeltaInSeconds;


        if( mTimeSinceLastBullet >= currentTimeBetweenBullets ) {

            if( distanceToShip <= mBossBulletRange ) {

                // close enough to hit target

                // fire
                double bulletMoveRate = mBossBulletBaseVelocity;


                // compute a vector for bullet and the angle of that vector
                Vector3D *vectorToShip = new Vector3D( inShipPosition );

                vectorToShip->subtract( mBossPosition );

                
                // compensate for our velocity when we aim at ship
                vectorToShip->normalize();
                vectorToShip->scale( bulletMoveRate );
                vectorToShip->subtract( bossVelocityVector );

                // compensate for the velocity of the ship
                vectorToShip->add( inShipVelocity );

                // normalize to turn compensated vector into a length
                // 1 direction
                vectorToShip->normalize();

                
                Vector3D *yVector = new Vector3D( 0, -1, 0 );
        
                Angle3D *angleToPointAt =
                    yVector->getZAngleTo( vectorToShip );
                delete yVector;
                
                vectorToShip->scale( bulletMoveRate );

                // adjust by boss motion
                vectorToShip->add( bossVelocityVector );
                    
                // close range based on health
                // long range based on anger level
                double healthFraction = mBossHealth / mMaxBossHealth;

                // get the final move rate of the bullet
                bulletMoveRate = vectorToShip->getLength();
                
                mBossBulletManager->addBullet(
                        healthFraction,
                        mAngerLevel,
                        1,
                        bulletMoveRate,  // range proportional to move rate
                        new Vector3D( mBossPosition ),
                        angleToPointAt,
                        vectorToShip );
                
                mTimeSinceLastBullet = 0;
                }
            }


        if( bulletPowerNearBoss > 0 ) {
            // emit damage wherever we are hit by ship bullets

            // do this down here since we have computed the boss
            // velocity by now

            // limit to at most 10 damage graphics in progress at once
            int limit = 10;
            
            double damageParameter = mBossHealth / mMaxBossHealth; 
            
            int numBullets;
            Vector3D **bulletPositions =
                mShipBulletManager->getBulletPositionsInCircle(
                    mBossPosition,
                    // smaller radius to ensure damage
                    // appears only on boss
                    mCurrentRadius / 2,   
                    &numBullets );

            double bossVelocity = currentVelocity;
            // how far the damage graphics will travel before they die
            double damageTravelDistance = bossVelocity * mBossDamageTime;
            
            for( int i=0; i<numBullets; i++ ) {

                if( mBossDamageManager->getBulletCount() < limit ) {
                    mBossDamageManager->addBullet(
                        damageParameter,
                        damageParameter,
                        1.0,  // full power
                        damageTravelDistance,
                        new Vector3D( bulletPositions[i] ),
                        new Angle3D( mCurrentRotation ),
                        // damage graphics have same velocity as boss
                        new Vector3D( bossVelocityVector ) );
                    }
                delete bulletPositions[i];
                }
            delete [] bulletPositions;
            }
        
        
        delete bossVelocityVector;


        // convert the distance to ship to the range [0,1] and save it
        double distanceToTarget = distanceToShip;
        
        // starting at 40 units, and up to 20 units from target, is the range
        double compressedDistance =
            ( distanceToTarget - 20 ) /
            ( 40 );

        // limit the range
        if( compressedDistance > 1 ) {
            compressedDistance = 1;
            }
        else if( compressedDistance < 0 ) {
            compressedDistance = 0;
            }

        mShipDistanceParameter = compressedDistance;

        }
    }



SimpleVector<DrawableObject *> *BossManager::getDrawableObjects() {

    double healthFraction = mBossHealth / mMaxBossHealth;

    
    SimpleVector<DrawableObject *> *bossObjects =
        mBossTemplate->getDrawableObjects(
            healthFraction,
            mShipDistanceParameter,
            mExplosionShapeParameter,
            mExplosionProgress,
            &mCurrentRotationRate );
    
    // fade out at end of explosion
    double alphaMultiplier = 1 - mExplosionFadeProgress;
    
    // compute the scale by weighting the enemy and explosion scales
    double scale =
        mExplosionProgress * mExplosionScale +
        ( 1 - mExplosionProgress ) * mBossScale;
    
    
    int numObjects = bossObjects->size();
        
    // compute the maximum radius of this boss
    double maxRadius = 0;
        
    for( int j=0; j<numObjects; j++ ) {

        DrawableObject *currentObject =
            *( bossObjects->getElement( j ) );
        
        currentObject->scale( scale );
        currentObject->rotate( mCurrentRotation );
        currentObject->move( mBossPosition );
        currentObject->fade( alphaMultiplier );
        
        double radius = currentObject->getBorderMaxDistance( mBossPosition );

        if( radius > maxRadius ) {
            maxRadius = radius;
            }
        }    
        
    mCurrentRadius = maxRadius;
    
    return bossObjects;
    }



Vector3D *BossManager::getBossPosition() {
    return new Vector3D( mBossPosition );
    }



char BossManager::isBossDead() {
    if( mExplosionFadeProgress >= 1 ) {
        return true;
        }
    else {
        return false;
        }
    }




/*
 * Modification History
 *
 * 2004-June-18   Jason Rohrer
 * Created.
 *
 * 2004-June-20   Jason Rohrer
 * Added fade-out of last explosion frame.  Added scaling.
 *
 * 2004-June-21   Jason Rohrer
 * Added support for enemies attacking sculpture pieces.
 *
 * 2004-June-29   Jason Rohrer
 * Fixed bug in passTime when time delta is 0.
 *
 * 2004-August-6   Jason Rohrer
 * Added enemy shot sounds.
 *
 * 2004-August-17   Jason Rohrer
 * Removed outdated enemy shot sound.  Still need to replace explosion sound.
 *
 * 2004-August-20   Jason Rohrer
 * Added sound priorities.
 *
 * 2004-August-24   Jason Rohrer
 * Added extra parameter for enemy distance from ship.
 *
 * 2004-August-29   Jason Rohrer
 * Improved distance-from-target compression formula.
 *
 * 2004-August-30   Jason Rohrer
 * Fixed bug in enemy explosion rotation.
 *
 * 2004-September-3   Jason Rohrer
 * Changed so that enemies back off if the ship has been knocked to the center.
 *
 * 2004-October-14   Jason Rohrer
 * Added function for exploding all active enemies.
 *
 * 2004-October-21   Jason Rohrer
 * Fixed bug that caused explosion progress to get larger than 1.
 *
 * 2005-August-21   Jason Rohrer
 * Added fade-in upon enemy creation.
 * Fixed bug when enemy distance to target is 0.
 * Made target-switching rotations smooth.
 */



#include "EnemyManager.h"
#include "ShipBulletManager.h"



#include <float.h>



EnemyManager::EnemyManager( Enemy *inEnemyTemplate,
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
                            double inWorldHeight )
    : mEnemyTemplate( inEnemyTemplate ),
      mSculptureManager( inSculptureManager ),
      mShipBulletManager( inShipBulletManager ),
      mEnemyBulletManager( inEnemyBulletManager ),
      mEnemyBulletRange( inEnemyBulletRange ),
      mEnemyBulletBaseVelocity( inEnemyBulletBaseVelocity ),
      mEnemyBulletsPerSecond( inEnemyBulletsPerSecond ),
      mEnemyTimeBetweenBullets( 1 / inEnemyBulletsPerSecond ),
      mSoundPlayer( inSoundPlayer ),
      mEnemyExplosionSoundTemplate( inEnemyExplosionSoundTemplate ),
      mEnemyScale( inEnemyScale ),
      mExplosionScale( inExplosionScale ),
      mEnemyVelocity( inEnemyVelocity ),
      mEnemyShapeParameters( new SimpleVector<double>() ),
      mExplosionShapeParameters( new SimpleVector<double>() ),
      mExplosionTimesInSeconds( new SimpleVector<double>() ),
      mCurrentlyExplodingFlags( new SimpleVector<char>() ),
      mExplosionProgress( new SimpleVector<double>() ),
      mFadeProgress( new SimpleVector<double>() ),
      mBulletCloseParameters( new SimpleVector<double>() ),
      mBulletFarParameters( new SimpleVector<double>() ),
      mTimesSinceLastBullet( new SimpleVector<double>() ),
      mCurrentPositions( new SimpleVector<Vector3D*>() ),
      mCurrentAnglesToPointAt( new SimpleVector<Angle3D*>() ),
      mCurrentRadii( new SimpleVector<double>() ),
      mCurrentRotations( new SimpleVector<Angle3D*>() ),
      mCurrentRotationRates( new SimpleVector<double>() ),
      mSholdBeDestroyedFlags( new SimpleVector<char>() ),
      mShipDistanceParameters( new SimpleVector<double>() ) {

    mMaxXPosition = inWorldWidth / 2;
    mMinXPosition = -mMaxXPosition;

    mMaxYPosition = inWorldHeight / 2;
    mMinYPosition = -mMaxYPosition;


    /*
    // create explosion sound
    int numSamples = 5513;
    mEnemyExplosionSound = new SoundSamples( numSamples );
    for( int j=0; j<numSamples; j++ ) {
        double weight = (double)j / (double)numSamples;
        double wavelength = 11.25 * weight + 7.5 * ( 1 - weight );

        
        mEnemyExplosionSound->mLeftChannel[j] =
            0.1 *
            sin( (double)j / wavelength ) *
            ( 1 - weight );
        mEnemyExplosionSound->mRightChannel[j] =
            0.1 *
            sin( (double)j / wavelength ) *
            ( 1 - weight );
        }
    */
    }


        
EnemyManager::~EnemyManager() {
    delete mEnemyTemplate;


    delete mEnemyShapeParameters;
    delete mExplosionShapeParameters;

    delete mCurrentlyExplodingFlags;
    delete mExplosionTimesInSeconds;
    delete mExplosionProgress;
    delete mFadeProgress;
    delete mBulletCloseParameters;
    delete mBulletFarParameters;
    delete mTimesSinceLastBullet;
    
    int numEnemies = mCurrentPositions->size();

    for( int i=0; i<numEnemies; i++ ) {

        delete *( mCurrentPositions->getElement( i ) );
        delete *( mCurrentAnglesToPointAt->getElement( i ) );
        delete *( mCurrentRotations->getElement( i ) );
        }

    delete mCurrentPositions;
    delete mCurrentAnglesToPointAt;
    delete mCurrentRotations;
    delete mCurrentRadii;
    
    delete mCurrentRotationRates;
    delete mSholdBeDestroyedFlags;

    delete mShipDistanceParameters;
    
    delete mEnemyExplosionSoundTemplate;
    }



void EnemyManager::addEnemy(
    double inEnemyShapeParameter,
    double inExplosionShapeParameter,
    double inExplosionTimeInSeconds,
    double inBulletCloseParameter,
    double inBulletFarParameter,
    Vector3D *inStartingPosition,
    Angle3D *inStartingRotation ) {

    mEnemyShapeParameters->push_back( inEnemyShapeParameter );
    mExplosionShapeParameters->push_back( inExplosionShapeParameter );

    mCurrentlyExplodingFlags->push_back( false );
    mExplosionTimesInSeconds->push_back( inExplosionTimeInSeconds );
    mExplosionProgress->push_back( 0 );

    // enemy starts out completly faded
    mFadeProgress->push_back( 1 );

    mBulletCloseParameters->push_back( inBulletCloseParameter );
    mBulletFarParameters->push_back( inBulletFarParameter );
    mTimesSinceLastBullet->push_back( 0 );
    
    mCurrentRotations->push_back( inStartingRotation );
    mCurrentRotationRates->push_back( 0 );

    mCurrentPositions->push_back( inStartingPosition );
    mCurrentAnglesToPointAt->push_back( new Angle3D( inStartingRotation ) );
    mCurrentRadii->push_back( 0 );
    
    mSholdBeDestroyedFlags->push_back( false );

    mShipDistanceParameters->push_back( 0 );
    }



void EnemyManager::explodeAllEnemies() {
    int numEnemies = mCurrentPositions->size();

    for( int i=0; i<numEnemies; i++ ) {
        *( mCurrentlyExplodingFlags->getElement( i ) ) = true;
        }
    }



int EnemyManager::getEnemyCount() {
    return mCurrentPositions->size();
    }



void EnemyManager::passTime( double inTimeDeltaInSeconds,
                             Vector3D *inShipPosition ) {

    int numEnemies = mCurrentPositions->size();

    Vector3D *centerPosition = new Vector3D( 0, 0, 0 );
    
    int i;
    for( i=0; i<numEnemies; i++ ) {
    
        Angle3D *currentRotation = *( mCurrentRotations->getElement( i ) );

        double rotationRate = *( mCurrentRotationRates->getElement( i ) );
        
        currentRotation->mZ += rotationRate * inTimeDeltaInSeconds;

        
        Vector3D *currentPosition =
            *( mCurrentPositions->getElement( i ) );

        // fly toward either the ship or a sculpture piece, whichever is
        // closer
        char targetIsShip = false;
        char targetIsBorder = false;
        Vector3D *targetPosition;

        double distanceToPiece = DBL_MAX;
        double distanceToShip = DBL_MAX;

        double closestApproachToTarget;
        
        
        Vector3D *closestSculpturePiecePosition =
            mSculptureManager->getPositionOfClosestSculpturePiece(
                currentPosition );

        if( closestSculpturePiecePosition != NULL ) {
            distanceToPiece =
                closestSculpturePiecePosition->getDistance( currentPosition );
            }

        distanceToShip = inShipPosition->getDistance( currentPosition );


        // if ship closer than closest piece
        // AND if ship not already knocked to center
        if( distanceToShip < distanceToPiece &&
            inShipPosition->getDistance( centerPosition ) >= 10 ) {

            
            targetIsShip = true;
            targetPosition = new Vector3D( inShipPosition );

            closestApproachToTarget = 10;
            
            if( closestSculpturePiecePosition != NULL ) {
                delete closestSculpturePiecePosition;
                }
            }
        else if( closestSculpturePiecePosition != NULL ) {
            targetPosition = closestSculpturePiecePosition;

            closestApproachToTarget = 10;
            }
        else {
            // ship has already been knocked to center
            // and there are no active sculpture pieces to attack
            
            // lose interest and head toward border
            targetIsBorder = true;

            // don't stop until enemy is right on top
            // of the border.
            closestApproachToTarget = 0;
            
            // pick closest border
            double x, y;
            
            if( fabs( currentPosition->mX ) < fabs( currentPosition->mY ) ) {
                // head to y border
                x = currentPosition->mX;
                
                if( currentPosition->mY < 0 ) {
                    y = mMinYPosition;
                    }
                else {
                    y = mMaxYPosition;
                    }
                }
            else {
                // head to x border
                y = currentPosition->mY;
                
                if( currentPosition->mX < 0 ) {
                    x = mMinXPosition;
                    }
                else {
                    x = mMaxXPosition;
                    }
                }
            targetPosition = new Vector3D( x, y, 0 );
            }

        
        // move enemy toward target
        
        Vector3D *moveVector = new Vector3D( targetPosition );

        moveVector->subtract( currentPosition );

        double distanceToTarget = moveVector->getLength();

        // Save a copy of the normalized move vector,
        // since we may be scaling it by zero
        Vector3D *normalizedMoveVector;
        
        // we cannot properly normalize a length-zero vector
        if( distanceToTarget == 0 ) {

            // use a default normalized vector as the direction below

            // point at center
            Vector3D *defaultMoveVector = new Vector3D( centerPosition );
            defaultMoveVector->subtract( currentPosition );

            if( defaultMoveVector->getLength() > 0 ) {
                defaultMoveVector->normalize();

                normalizedMoveVector = new Vector3D( defaultMoveVector );
                }
            else {
                // we must be at the center, so use a default move direction
                normalizedMoveVector = new Vector3D( 0, 1, 0 );
                }
            delete defaultMoveVector;
            }
        else {
        
            moveVector->normalize();

            normalizedMoveVector = new Vector3D( moveVector );
            /*
            moveVector->scale( mEnemyVelocity * inTimeDeltaInSeconds );

            double moveLength = moveVector->getLength();
            if( distanceToTarget - moveLength < closestApproachToTarget ) {

                // this move will put us too close to the target
                
                // don't move at all
                }
            else {
                // not already too close to target
                
                if( ! *( mCurrentlyExplodingFlags->getElement( i ) ) ) {
                    // not exploding
                    currentPosition->add( moveVector );
                    }
                }
            */
            }
        delete moveVector;

        // point enemy toward target
        Vector3D *yVector = new Vector3D( 0, -1, 0 );
        
        Angle3D *angleToPointAt = yVector->getZAngleTo( normalizedMoveVector );

        if( ! *( mCurrentlyExplodingFlags->getElement( i ) ) ) {
            // not exploding

            Angle3D *currentAngleToPointAt =
                *( mCurrentAnglesToPointAt->getElement(i) );

            // the angle we want to point at
            double goalZAngle = angleToPointAt->mZ;

            // the angle we are currently pointing at
            double currentZAngle = currentAngleToPointAt->mZ;

            // we want a smooth transition between these angles to
            // ensure a smooth rotation when targets change

            
            // use enemy velocity here as rotation rate in radians per sec
            double rotDelta = mEnemyVelocity * inTimeDeltaInSeconds;

            double angleLeftToRotate = goalZAngle - currentZAngle;
            
            if( fabs( angleLeftToRotate ) > M_PI ) {

                if( angleLeftToRotate < 0 ) {
                    angleLeftToRotate += 2 * M_PI;
                    }
                else {
                    angleLeftToRotate -= 2 * M_PI;
                    }
                //angleLeftToRotate = -angleLeftToRotate;
                }

            if( angleLeftToRotate < 0 ) {
                rotDelta = -rotDelta;
                }
            
            if( fabs( angleLeftToRotate ) < fabs( rotDelta ) ) {
                rotDelta = angleLeftToRotate;
                }
            

            
            double newZAngle = currentZAngle + rotDelta;
            currentRotation->mZ = newZAngle;

            // save the new z angle as our current angle
            currentAngleToPointAt->mZ = newZAngle;


            // compute our true move vector using the angle we are pointing at
            Vector3D *trueMoveVector = new Vector3D( yVector );
            trueMoveVector->rotate( currentAngleToPointAt );
            trueMoveVector->scale( mEnemyVelocity * inTimeDeltaInSeconds );

            double moveLength = trueMoveVector->getLength();
            if( distanceToTarget - moveLength < closestApproachToTarget ) {

                // this move will put us too close to the target
                
                // don't move at all
                }
            else {
                // not already too close to target
                currentPosition->add( trueMoveVector );
                }
            
            delete trueMoveVector;
            }

        delete yVector;
    
        // check if enemy should fire
        double timeSinceLastFire = *( mTimesSinceLastBullet->getElement( i ) );

        timeSinceLastFire += inTimeDeltaInSeconds;


        if( timeSinceLastFire >= mEnemyTimeBetweenBullets &&
            ! targetIsBorder ) {   // don't fire if heading toward border

            // don't fire if exploding
            if( ! *( mCurrentlyExplodingFlags->getElement( i ) ) ) {

                if( currentPosition->getDistance( targetPosition ) <=
                    mEnemyBulletRange ) {

                    // close enough to hit target

                    // fire
                    double bulletMoveRate = mEnemyBulletBaseVelocity;
                    
                    Vector3D *bulletVelocityVector =
                        new Vector3D( 0, -bulletMoveRate, 0 );
                    bulletVelocityVector->rotate( angleToPointAt );
                    
                    mEnemyBulletManager->addBullet(
                        *( mBulletCloseParameters->getElement( i ) ),
                        *( mBulletFarParameters->getElement( i ) ),
                        1,
                        mEnemyBulletRange,
                        new Vector3D( currentPosition ),
                        new Angle3D( angleToPointAt ),
                        bulletVelocityVector );
                
                    timeSinceLastFire = 0;
                    }
                }
            }
        *( mTimesSinceLastBullet->getElement( i ) ) = timeSinceLastFire;


        ;
        delete angleToPointAt;
                        
        delete normalizedMoveVector;

        delete targetPosition;
        
        
        double bulletPowerNearThisEnemy =
            mShipBulletManager->getBulletPowerInCircle(
                *( mCurrentPositions->getElement( i ) ),
                *( mCurrentRadii->getElement( i ) ) );
            
        if( bulletPowerNearThisEnemy > 0 ) {
            // enemy hit by bullet

            if( ! *( mCurrentlyExplodingFlags->getElement( i ) ) ) {
                // not already exploding

                // start exploding
                *( mCurrentlyExplodingFlags->getElement( i ) ) = true;

                // play the sound
                PlayableSound *sound =
                    mEnemyExplosionSoundTemplate->getPlayableSound(
                        *( mBulletCloseParameters->getElement( i ) ),
                        *( mBulletFarParameters->getElement( i ) ),
                        mSoundPlayer->getSampleRate() );

                // enemy explosion is low priority
                mSoundPlayer->playSoundNow(
                    sound, false, 1 );
                //*( mExplosionShapeParameters->getElement( i ) ) );

                delete sound;
                }
            }
        
        if( *( mCurrentlyExplodingFlags->getElement( i ) ) ) {
            // this enemy is exploding
            
            // compute new explosion progress

            double explosionTime =
                *( mExplosionTimesInSeconds->getElement( i ) );

            // convert time delta to a progress increment for the progress
            // range [0,1]
            double progressFractionDelta =
                inTimeDeltaInSeconds / explosionTime;

            double progress = *( mExplosionProgress->getElement( i ) );

            progress += progressFractionDelta;

            if( progress > 1 ) {
                progress = 1;
                }
            
            *( mExplosionProgress->getElement( i ) ) = progress;
            
            if( progress == 1 ) {
                // end of explosion
                // fade out last frame

                double fadeProgress =
                    *( mFadeProgress->getElement( i ) );

                fadeProgress += progressFractionDelta;

                
                if( fadeProgress >= 1 ) {
                    // explosion done fading
                    fadeProgress = 1;
                    
                    // enemy should be destroyed
                    *( mSholdBeDestroyedFlags->getElement( i ) ) = true;
                    }

                *( mFadeProgress->getElement( i ) ) = fadeProgress;

                }        
            }
        else {
            // not exploding
            // check if we are fading in after enemy creation
            double fadeProgress =
                *( mFadeProgress->getElement( i ) );

            if( fadeProgress != 0 ) {
                // still fading in

                // use explosion time as fade-in time
                // (makes sense, since we also use the explosion time for
                //  the fade-out time after the explosion finishes)
                double totalFadeTime =
                    *( mExplosionTimesInSeconds->getElement( i ) );

                // convert time delta to a progress increment for the progress
                // range [0,1]
                double progressFractionDelta =
                    inTimeDeltaInSeconds / totalFadeTime;

                fadeProgress -= progressFractionDelta;

                if( fadeProgress < 0 ) {
                    fadeProgress = 0;
                    }
                *( mFadeProgress->getElement( i ) ) = fadeProgress;
                }
            }

        
        // convert the distance to target to the range [0,1] and save it

        if( targetIsBorder ) {
            // don't have enemy change as it gets closer to the border...

            // use the distance from the ship to control its form
            distanceToTarget = distanceToShip;
            }
        
        // 25 units, up to 20 units from target, is the range
        double compressedDistance =
            ( distanceToTarget - 20 ) /
            ( 25 );

        // limit the range
        if( compressedDistance > 1 ) {
            compressedDistance = 1;
            }
        else if( compressedDistance < 0 ) {
            compressedDistance = 0;
            }
        
        *( mShipDistanceParameters->getElement( i ) ) =
            compressedDistance;
        
        }

    
    // walk through vectors and destroy enemies that are flagged
    // note that our loop range is adjusted as the vector length shrinks
    for( i=0; i<mEnemyShapeParameters->size(); i++ ) {
        if( *( mSholdBeDestroyedFlags->getElement( i ) ) ) {

            mEnemyShapeParameters->deleteElement( i );
            mExplosionShapeParameters->deleteElement( i );

            mCurrentlyExplodingFlags->deleteElement( i );
            mExplosionTimesInSeconds->deleteElement( i );
            mExplosionProgress->deleteElement( i );
            mFadeProgress->deleteElement( i );

            mBulletCloseParameters->deleteElement( i );
            mBulletFarParameters->deleteElement( i );
            mTimesSinceLastBullet->deleteElement( i );
            
            delete *( mCurrentRotations->getElement( i ) );
            mCurrentRotations->deleteElement( i );

            mCurrentRotationRates->deleteElement( i );

            delete *( mCurrentPositions->getElement( i ) );
            mCurrentPositions->deleteElement( i );

            delete *( mCurrentAnglesToPointAt->getElement( i ) );
            mCurrentAnglesToPointAt->deleteElement( i );
            
            mCurrentRadii->deleteElement( i );
            
            mSholdBeDestroyedFlags->deleteElement( i );
            mShipDistanceParameters->deleteElement( i );
            }
        }

    delete centerPosition;        
    }



SimpleVector<DrawableObject*> *EnemyManager::getDrawableObjects() {

    SimpleVector<DrawableObject*> *returnVector =
        new SimpleVector<DrawableObject*>();
    
    int numEnemies = mEnemyShapeParameters->size();

    for( int i=0; i<numEnemies; i++ ) {
        double currentRotationRate;
        
        SimpleVector<DrawableObject *> *enemyObjects =
            mEnemyTemplate->getDrawableObjects(
                *( mEnemyShapeParameters->getElement( i ) ),
                *( mShipDistanceParameters->getElement( i ) ),
                *( mExplosionShapeParameters->getElement( i ) ),
                *( mExplosionProgress->getElement( i ) ),
                &currentRotationRate );

        *( mCurrentRotationRates->getElement( i ) ) = currentRotationRate;

        // fade out at end of explosion
        double fadeValue = *( mFadeProgress->getElement( i ) );
        double alphaMultiplier = 1 - fadeValue;

        
        // compute the scale by weighting the enemy and explosion scales
        double explosionProgress = *( mExplosionProgress->getElement( i ) );
        double scale =
            explosionProgress * mExplosionScale +
            ( 1 - explosionProgress ) * mEnemyScale;

        
        int numObjects = enemyObjects->size();

        // compute the maximum radius of this enemy
        double maxRadius = 0;
        
        for( int j=0; j<numObjects; j++ ) {

            DrawableObject *currentObject =
                *( enemyObjects->getElement( j ) );

            currentObject->scale( scale );
            currentObject->rotate( *( mCurrentRotations->getElement( i ) ) );
            currentObject->move( *( mCurrentPositions->getElement( i ) ) );
            currentObject->fade( alphaMultiplier );

            double radius = currentObject->getBorderMaxDistance(
                *( mCurrentPositions->getElement( i ) ) );

            if( radius > maxRadius ) {
                maxRadius = radius;
                }
            
            returnVector->push_back( currentObject );
            }

        *( mCurrentRadii->getElement( i ) ) = maxRadius;
        
        delete enemyObjects;
        }

    return returnVector;
    }

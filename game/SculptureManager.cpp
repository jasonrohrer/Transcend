/*
 * Modification History
 *
 * 2004-June-21   Jason Rohrer
 * Created.
 *
 * 2004-August-6   Jason Rohrer
 * Fixed consistency bugs in test for in/out of sculpture status.
 *
 * 2004-August-22   Jason Rohrer
 * Added music parts.
 *
 * 2004-August-23   Jason Rohrer
 * Added function for getting number of pieces in sculpture.
 *
 * 2004-August-29   Jason Rohrer
 * Changed so that only pieces in sculpture are animated.
 *
 * 2004-August-30   Jason Rohrer
 * Optimization:  avoid object blending whenever possible.
 * Added function for getting piece position needed to recenter.
 * Weakened the power modifier curve.
 *
 * 2004-September-3   Jason Rohrer
 * Changed to use pure distance between pieces instead of gap for in-sculpture
 * test.
 *
 * 2005-August-22   Jason Rohrer
 * Changed so that isPieceJarred returns true only if jar force is increasing.
 * Added magnet mode to smooth piece pick-up and drop.
 */



#include "SculptureManager.h"



#include <float.h>



SculptureManager::SculptureManager(
    ParameterizedObject *inFirstSculptureTemplate,
    ParameterizedObject *inSecondSculptureTemplate,
    double inSculptureScale,
    double inMaxDistanceToBeInSculpture,
    double inAnimationLoopTime,
    int inNumSculpturePieces,
    double *inPieceShapeParameters,
    Vector3D **inPieceStartingPositions,
    Angle3D **inPieceStartingRotations,
    MusicPart **inPieceMusicParts,
    FILE *inSculpturePowerUpSpaceFILE,
    char *outError,
    ShipBulletManager *inEnemyBulletManager,
    double inEnemyBulletJarPower,
    ShipBulletManager *inBossBulletManager,
    double inBossBulletJarPower,
    double inFriction,
    double inWorldWidth,
    double inWorldHeight )
    : mFirstSculpturePieceTemplate( inFirstSculptureTemplate ),
      mSecondSculpturePieceTemplate( inSecondSculptureTemplate ),
      mSculptureScale( inSculptureScale ),
      mMaxDistanceToBePartOfSculpture( inMaxDistanceToBeInSculpture ),
      mAnimationLoopTime( inAnimationLoopTime ),
      mCurrentAnimationPosition( 0 ),
      mCurrentAnimationDirection( 1 ),
      mNumSculpturePieces( inNumSculpturePieces ),
      mSculpturePieceParameters( inPieceShapeParameters ),
      mCurrentPiecePositions( inPieceStartingPositions ),
      mCurrentPieceRotations( inPieceStartingRotations ),
      mPieceMusicParts( inPieceMusicParts ),
      mEnemyBulletManager( inEnemyBulletManager ),
      mEnemyBulletJarPower( inEnemyBulletJarPower ),
      mBossBulletManager( inBossBulletManager ),
      mBossBulletJarPower( inBossBulletJarPower ),
      mFriction( inFriction ),
      mMaxXPosition( inWorldWidth / 2 ),
      mMinXPosition( - inWorldWidth / 2 ),
      mMaxYPosition( inWorldHeight / 2 ),
      mMinYPosition( - inWorldHeight / 2 ) {

    mPieceMagnetModes = new char[ mNumSculpturePieces ];
    mCurrentPieceTargetPositions = new Vector3D*[ mNumSculpturePieces ];
    mCurrentTowardTargetVelocities = new double[ mNumSculpturePieces ];
    
    mCurrentPieceRotationRates = new double[ mNumSculpturePieces ];
    mCurrentPieceRadii = new double[ mNumSculpturePieces ];
    mCurrentJarForces = new double[ mNumSculpturePieces ];
    mJarForcesIncreasing = new char[ mNumSculpturePieces ];
    
    mNumPiecesInSculpture = 0;
    mInSculptureFlags = new char[ mNumSculpturePieces ];
    mDelayAnimationStartFlags = new char[ mNumSculpturePieces ];
    mDelayAnimationStopFlags = new char[ mNumSculpturePieces ];
 
    int i;
    
    for( i=0; i<mNumSculpturePieces; i++ ) {
        mPieceMagnetModes[i] = false;
        mCurrentPieceTargetPositions[i] =
            new Vector3D( mCurrentPiecePositions[i] );
        mCurrentTowardTargetVelocities[i] = 0;
        
        mCurrentPieceRotationRates[i] = 0;
        mCurrentPieceRadii[i] = 0;
        mCurrentJarForces[i] = 0;
        mJarForcesIncreasing[i] = 0;
        
        mInSculptureFlags[i] = false;
        mDelayAnimationStartFlags[i] = false;
        mDelayAnimationStopFlags[i] = false;
        }
    
    
    int numRead = fscanf( inSculpturePowerUpSpaceFILE, "%d",
                          &mNumParameterMapAnchors );

    if( numRead != 1 ) {
        // default
        mNumParameterMapAnchors = 1;
        *outError = true;
        }

    mParameterMapAnchors = new double[ mNumParameterMapAnchors ];
    mParameterMapCloseRangeValues = new double[ mNumParameterMapAnchors ];
    mParameterMapFarRangeValues = new double[ mNumParameterMapAnchors ];


    for( i=0; i<mNumParameterMapAnchors; i++ ) {
        numRead = fscanf( inSculpturePowerUpSpaceFILE, "%lf",
                          &( mParameterMapAnchors[i] ) );
        if( numRead != 1 ) {
            // default
            mParameterMapAnchors[i] = 0.5;
            *outError = true;
            }

        numRead = fscanf( inSculpturePowerUpSpaceFILE, "%lf",
                          &( mParameterMapCloseRangeValues[i] ) );
        if( numRead != 1 ) {
            // default
            mParameterMapCloseRangeValues[i] = 0.5;
            *outError = true;
            }

        
        numRead = fscanf( inSculpturePowerUpSpaceFILE, "%lf",
                          &( mParameterMapFarRangeValues[i] ) );
        if( numRead != 1 ) {
            // default
            mParameterMapFarRangeValues[i] = 0.5;
            *outError = true;
            }
        }
    }



SculptureManager::~SculptureManager() {
    delete mFirstSculpturePieceTemplate;
    delete mSecondSculpturePieceTemplate;
    
    for( int i=0; i<mNumSculpturePieces; i++ ) {
        delete mCurrentPiecePositions[i];
        delete mCurrentPieceTargetPositions[i];
        delete mCurrentPieceRotations[i];
        delete mPieceMusicParts[i];
        }

    delete [] mSculpturePieceParameters;
    delete [] mCurrentPiecePositions;

    delete [] mPieceMagnetModes;
    delete [] mCurrentPieceTargetPositions;
    delete [] mCurrentTowardTargetVelocities;
    
    delete [] mCurrentPieceRotations;
    delete [] mPieceMusicParts;
    delete [] mCurrentPieceRotationRates;
    delete [] mCurrentPieceRadii;
    delete [] mCurrentJarForces;
    delete [] mJarForcesIncreasing;
    
    delete [] mInSculptureFlags;
    delete [] mDelayAnimationStartFlags;
    delete [] mDelayAnimationStopFlags;
    
    delete [] mParameterMapAnchors;
    delete [] mParameterMapCloseRangeValues;
    delete [] mParameterMapFarRangeValues;
    }



double SculptureManager::getCloseRangeBulletParameter() {

    double closeRangeSum = 0;
    
    for( int i=0; i<mNumSculpturePieces; i++ ) {
        if( mInSculptureFlags[i] ) {
            // piece is in sculpture

            // add its power ups
            double closeRange;
            double farRange;

            mapParameters( mSculpturePieceParameters[i],
                           &closeRange, &farRange );

            closeRangeSum += closeRange;
            }
        }

    if( closeRangeSum > 1 ) {
        closeRangeSum = 1;
        }
    
    return closeRangeSum;
    }



double SculptureManager::getFarRangeBulletParameter() {
    double farRangeSum = 0;
    
    for( int i=0; i<mNumSculpturePieces; i++ ) {
        if( mInSculptureFlags[i] ) {
            // piece is in sculpture

            // add its power ups
            double closeRange;
            double farRange;

            mapParameters( mSculpturePieceParameters[i],
                           &closeRange, &farRange );

            farRangeSum += farRange;
            }
        }

    if( farRangeSum > 1 ) {
        farRangeSum = 1;
        }
    return farRangeSum;
    }



Vector3D *SculptureManager::getSculptureCenter() {
    Vector3D *positionSum = new Vector3D( 0, 0, 0 );

    int numPiecesInSculpture = 0;
    for( int i=0; i<mNumSculpturePieces; i++ ) {
        if( mInSculptureFlags[i] ) {

            positionSum->add( mCurrentPiecePositions[i] );

            numPiecesInSculpture++;
            }
        }
    
    if( numPiecesInSculpture > 0 ) {
        // convert sum to average
        positionSum->scale( 1.0 / (double)numPiecesInSculpture );
        }

    return positionSum;
    }



double SculptureManager::getBulletPowerModifier() {
    Vector3D *center = getSculptureCenter();
    
    Vector3D *zeroVector = new Vector3D( 0, 0, 0 );

    double distanceFromCenter = zeroVector->getDistance( center );

    delete zeroVector;
    delete center;

    // use inverse exponential (power of e) to map the [0, inf] distance
    // into [1,0]
    // scale the distance somewhat so the curve is not so extreme
    double returnValue = 1 / exp( 0.05 * distanceFromCenter );
    
    // lower limit of 0.25
    return returnValue * 0.75 + 0.25;
    }



void SculptureManager::passTime( double inTimeDeltaInSeconds ) {

    // full animation loop (forward and back) is 2 parameter space units
    double animationDelta = 2 * inTimeDeltaInSeconds / mAnimationLoopTime;

    mCurrentAnimationPosition += mCurrentAnimationDirection * animationDelta;

    if( mCurrentAnimationPosition > 1 ) {
        // bounce back into range
        double extra = mCurrentAnimationPosition - 1;
        mCurrentAnimationPosition = 1 - extra;
        
        // reverse animation direction
        mCurrentAnimationDirection = -1;

        if( mCurrentAnimationPosition < 0 ) {
            // we bounced back too far

            // default to known safe state
            mCurrentAnimationPosition = 0;
            mCurrentAnimationDirection = 1;
            }            
        }
    else if( mCurrentAnimationPosition < 0 ) {
        // bounce back into range
        double extra = 0 - mCurrentAnimationPosition;
        mCurrentAnimationPosition = 0 + extra;
        
        // reverse animation direction
        mCurrentAnimationDirection = 1;

        if( mCurrentAnimationPosition > 1 ) {
            // we bounced back too far

            // default to known safe state
            mCurrentAnimationPosition = 1;
            mCurrentAnimationDirection = -1;
            }

        // we have reached the animation starting point
        // start any animations that are waiting to start, and stop
        // any that are waiting to stop (to ensure clean starts and stops)
        for( int i=0; i<mNumSculpturePieces; i++ ) {
            mDelayAnimationStartFlags[i] = false;
            mDelayAnimationStopFlags[i] = false;
            }
        }

    
    for( int i=0; i<mNumSculpturePieces; i++ ) {
        // rotate piece at its current rate
        Angle3D *currentPieceRotation = mCurrentPieceRotations[i];

        currentPieceRotation->mZ +=
            inTimeDeltaInSeconds * mCurrentPieceRotationRates[i];

        
        // check if piece being hit by bullet
        
        double enemyBulletPower =
            mEnemyBulletManager->getBulletPowerInCircle(
                mCurrentPiecePositions[i],
                mCurrentPieceRadii[i] );
        double bossBulletPower = 
            mBossBulletManager->getBulletPowerInCircle(
                mCurrentPiecePositions[i],
                mCurrentPieceRadii[i] );

        // scale bullet power by time delta
        double jarForceIncrease =
            inTimeDeltaInSeconds * enemyBulletPower * mEnemyBulletJarPower +
            inTimeDeltaInSeconds * bossBulletPower * mBossBulletJarPower;


        // save last jar force to detect whether force has increased
        double lastJarForce = mCurrentJarForces[i];
        
        // increased by being hit by bullets
        mCurrentJarForces[i] += jarForceIncrease;
        
        // decayed by friction
        mCurrentJarForces[i] -= mFriction * inTimeDeltaInSeconds;

        // never negative
        if( mCurrentJarForces[i] < 0 ) {
            mCurrentJarForces[i] = 0;
            }

        double jarForceDelta = mCurrentJarForces[i] - lastJarForce;

        if( jarForceDelta > 0 ) {
            mJarForcesIncreasing[i] = true;
            }
        else {
            mJarForcesIncreasing[i] = false;
            }
        
        
        if( mCurrentJarForces[i] > 0 ) {

            // jar piece away from center
            Vector3D *jarVector = new Vector3D( 0, 0, 0 );
            jarVector->subtract( mCurrentPiecePositions[i] );

            jarVector->scale( -1 );
            
            if( jarVector->getLength() > 0 ) {
                jarVector->normalize();
                }
            else {
                // cannot compute a proper vector for moving piece "away"
                // from center, since it is at center.
                
                // default to moving piece in y direction
                jarVector->mY = 1;
                }
            
            jarVector->scale( mCurrentJarForces[i] * inTimeDeltaInSeconds );
            mCurrentPiecePositions[i]->add( jarVector );

            Vector3D *currentPosition =
                mCurrentPiecePositions[i];

            // bound jarring action to inside of world

            // jarring stops when we hit a world edge
            
            if( currentPosition->mX > mMaxXPosition ) {
                currentPosition->mX = mMaxXPosition;
                mCurrentJarForces[i] = 0;
                }
            if( currentPosition->mX < mMinXPosition ) {
                currentPosition->mX = mMinXPosition;
                mCurrentJarForces[i] = 0;
                }
            if( currentPosition->mY > mMaxYPosition ) {
                currentPosition->mY = mMaxYPosition;
                mCurrentJarForces[i] = 0;
                }
            if( currentPosition->mY < mMinYPosition ) {
                currentPosition->mY = mMinYPosition;
                mCurrentJarForces[i] = 0;
                }
            
            delete jarVector;

            updateInOutStatusOfAllPieces();
            }

        
        if( mPieceMagnetModes[i] ) {
            // move piece toward target

            // velocity toward target increases at rate of 20 unit/sec per sec
            mCurrentTowardTargetVelocities[i] += 20;

            double totalDistance =
                mCurrentPieceTargetPositions[i]->getDistance(
                    mCurrentPiecePositions[i] );
            
            double moveDistance =
                inTimeDeltaInSeconds *
                mCurrentTowardTargetVelocities[i];

            if( moveDistance >= totalDistance ) {
                // we will hit or pass our target with this move
                delete mCurrentPiecePositions[i];
                mCurrentPiecePositions[i] =
                    new Vector3D( mCurrentPieceTargetPositions[i] );

                // velocity goes back to 0 instantly
                mCurrentTowardTargetVelocities[i] = 0;

                // turn off magnet mode
                mPieceMagnetModes[i] = false;
                }
            else {
                // this move does not hit or pass our target, so execute it
                Vector3D *moveVector =
                    new Vector3D( mCurrentPieceTargetPositions[i] );

                moveVector->subtract( mCurrentPiecePositions[i] );
                
                moveVector->normalize();
                
                moveVector->scale( moveDistance );
                
                mCurrentPiecePositions[i]->add( moveVector );
                
                delete moveVector;
                }
            }
        
        }
        
    }



int SculptureManager::getSculpturePieceInCircle( Vector3D *inCenter,
                                                 double inRadius ) {
    int closestPiece = -1;
    double closestDistance = DBL_MAX;
    
    for( int i=0; i<mNumSculpturePieces; i++ ) {
        double distance = inCenter->getDistance( mCurrentPiecePositions[i] );

        distance = distance - mCurrentPieceRadii[i];
        
        if( distance <= inRadius && distance < closestDistance ) {
            closestPiece = i;
            closestDistance = distance;
            }
        }

    return closestPiece;
    }



Vector3D *SculptureManager::getPositionOfClosestSculpturePiece(
    Vector3D *inPosition ) {
    
    Vector3D *closestPiecePosition = NULL;
    double closestDistance = DBL_MAX;
    
    for( int i=0; i<mNumSculpturePieces; i++ ) {
        if( mInSculptureFlags[i] ) {
            double distance =
                inPosition->getDistance( mCurrentPiecePositions[i] );
            
            if( distance < closestDistance ) {
                closestPiecePosition = mCurrentPiecePositions[i];
                closestDistance = distance;
                }
            }
        }

    if( closestPiecePosition != NULL ) {
        return new Vector3D( closestPiecePosition );
        }
    else {
        return NULL;
        }
    }



int SculptureManager::getNumPiecesInSculpture() {
    int count = 0;
    for( int i=0; i<mNumSculpturePieces; i++ ) {
        if( mInSculptureFlags[i] ) {
            count++;
            }
        }
    return count;
    }



Vector3D **SculptureManager::getPiecePositions( int *outNumPieces ) {

    SimpleVector<Vector3D*> *positions = new SimpleVector<Vector3D*>();

    for( int i=0; i<mNumSculpturePieces; i++ ) {
        if( mInSculptureFlags[i] ) {

            positions->push_back( new Vector3D( mCurrentPiecePositions[i] ) );
            }
        }

    *outNumPieces = positions->size();

    Vector3D **returnArray = positions->getElementArray();

    delete positions;

    return returnArray;
    }



MusicPart **SculptureManager::getPieceMusicParts( int *outNumPieces ) {

    SimpleVector<MusicPart*> *parts = new SimpleVector<MusicPart*>();

    for( int i=0; i<mNumSculpturePieces; i++ ) {
        if( mInSculptureFlags[i] ) {

            parts->push_back( mPieceMusicParts[i] );
            }
        }

    *outNumPieces = parts->size();

    MusicPart **returnArray = parts->getElementArray();

    delete parts;

    return returnArray;
    }



void SculptureManager::updateInOutStatusOfAllPieces() {
    // first, flag all pieces as out, except those that are close to origin

    // origin position
    Vector3D *zeroVector = new Vector3D( 0, 0, 0 );

    // update our count
    int oldCount = mNumPiecesInSculpture;
    mNumPiecesInSculpture = 0;
    

    // remember old flags
    char *oldFlags = new char[ mNumSculpturePieces ];

    memcpy( oldFlags, mInSculptureFlags,
            sizeof( char ) * mNumSculpturePieces );
    
    int i;
    for( i=0; i<mNumSculpturePieces; i++ ) {
        double distanceToOrigin =
            zeroVector->getDistance( mCurrentPiecePositions[ i ] );
        
        if( distanceToOrigin <= mMaxDistanceToBePartOfSculpture ) {
            mInSculptureFlags[i] = true;
            mNumPiecesInSculpture++;
            }
        else {
            mInSculptureFlags[i] = false;
            }
        }
    
    delete zeroVector;

    // now walk through pieces over and over and expand the set by looking
    // for pieces not in the set that are close to pieces that are already
    // in the set

    // stop when no piece was added in the last round
    char piecesAdded = true;

    while( piecesAdded ) {
        piecesAdded = false;

        for( i=0; i<mNumSculpturePieces; i++ ) {

            if( mInSculptureFlags[i] == false ) {
                // a piece not in the set yet
                
                char added = false;
                
                for( int j=0; j<mNumSculpturePieces && !added; j++ ) {

                    if( mInSculptureFlags[j] == true ) {
                        // a piece in the set

                        double distance =
                            mCurrentPiecePositions[i]->getDistance(
                                mCurrentPiecePositions[j] );

                        if( distance <= mMaxDistanceToBePartOfSculpture ) {
                            mInSculptureFlags[i] = true;
                            mNumPiecesInSculpture++;
                            
                            added = true;
                            piecesAdded = true;
                            }
                        }                
                    }
                }
            }
        }


    
    char animationReset = false;
    
    if( oldCount == 0 &&
        mNumPiecesInSculpture != 0 ) {

        // sculpture used to be empty... but now has pieces

        // make sure no piece is in the process of its stop animation
        char pieceStopping = false;
        for( i=0; i<mNumSculpturePieces && !pieceStopping; i++ ) {
            pieceStopping = mDelayAnimationStopFlags[i];
            }

        if( !pieceStopping ) {
            animationReset = true;
            // reset the animation to give instant gratification

            mCurrentAnimationPosition = 0;
            mCurrentAnimationDirection = 1;

            // do not delay animations at all
            for( i=0; i<mNumSculpturePieces; i++ ) {
                mDelayAnimationStartFlags[i] = false;
                }
            }
        }

    if( !animationReset ) {

        // check for pieces that have changed status
    
        for( i=0; i<mNumSculpturePieces; i++ ) {

            if( oldFlags[i] != mInSculptureFlags[i] ) {

                if( oldFlags[i] ) {
                    // piece just removed

                // make sure piece not waiting to start animation
                    if( ! mDelayAnimationStartFlags[i] ) {
                        mDelayAnimationStopFlags[i] = true;
                        }
                    else {
                        // piece added and removed before it got a chance
                        // to start its animation
                        mDelayAnimationStartFlags[i] = false;
                        }
                    }
                else{
                    // piece just added

                    // make sure piece not waiting to stop animation
                    if( ! mDelayAnimationStopFlags[i] ) {
                        mDelayAnimationStartFlags[i] = true;
                        }
                    else {
                        // piece removed and re-added before it got a chance
                        // to stop its animation
                        mDelayAnimationStopFlags[i] = false;
                        }
                    }
                }
            }
        }

    delete [] oldFlags;
    }



Vector3D *SculptureManager::getPiecePositionNeededToRecenter() {

    int numPieces = getNumPiecesInSculpture();

    Vector3D *center = getSculptureCenter();


    // opposite weighted center
    center->scale( -numPieces );

    return center;    
    }



void SculptureManager::turnMagnetModeOn( int inPieceHandle ) {
    mPieceMagnetModes[ inPieceHandle ] = true;
    }



void SculptureManager::setPiecePosition( int inPieceHandle,
                                         Vector3D *inNewPosition ) {
    Vector3D *position;

    if( mPieceMagnetModes[ inPieceHandle ] ) {
        // piece in magnet mode, set target position
        position = mCurrentPieceTargetPositions[ inPieceHandle ];
        }
    else {
        // not in magnet mode, set current position
        position = mCurrentPiecePositions[ inPieceHandle ];
        }
    
    position->mX = inNewPosition->mX;
    position->mY = inNewPosition->mY;
    position->mZ = inNewPosition->mZ;

    updateInOutStatusOfAllPieces();
    }



SimpleVector<DrawableObject *> *SculptureManager::getDrawableObjects() {
    SimpleVector<DrawableObject *> *returnVector =
        new SimpleVector<DrawableObject *>();

    for( int i=0; i<mNumSculpturePieces; i++ ) {
        double pieceRotationRate;


        double animPosition = 0;

        // if piece is part of sculpture and it is not waiting to start
        // its animation
        // or if piece is no longer part of sculpture but its animation
        // is finishing up
        if( ( mInSculptureFlags[i] && ! mDelayAnimationStartFlags[i] )
            ||
            mDelayAnimationStopFlags[i] ) {
            
            // animate it
            animPosition = mCurrentAnimationPosition;
            }
        
        // avoid blending if possible
        ObjectParameterSpaceControlPoint *animationPoint;

        if( animPosition == 0 ) {
            // use pure first point
            animationPoint =
                mFirstSculpturePieceTemplate->getBlendedControlPoint(
                    mSculpturePieceParameters[i] );
            }
        else if( animPosition == 1 ) {
            // use pure second point
            animationPoint =
                mSecondSculpturePieceTemplate->getBlendedControlPoint(
                    mSculpturePieceParameters[i] );
            }
        else {
            ObjectParameterSpaceControlPoint *firstControlPoint =
                mFirstSculpturePieceTemplate->getBlendedControlPoint(
                    mSculpturePieceParameters[i] );

            ObjectParameterSpaceControlPoint *secondControlPoint =
                mSecondSculpturePieceTemplate->getBlendedControlPoint(
                    mSculpturePieceParameters[i] );


        
            animationPoint =
                (ObjectParameterSpaceControlPoint *)(
                    firstControlPoint->createLinearBlend(
                        secondControlPoint,
                        animPosition ) );
            delete firstControlPoint;
            delete secondControlPoint;
            }

        pieceRotationRate = animationPoint->getRotationRate();
        
        SimpleVector<DrawableObject *> *pieceObjects =
            animationPoint->getDrawableObjects();

        delete animationPoint;
        
        mCurrentPieceRotationRates[i] = pieceRotationRate;

        int numObjects = pieceObjects->size();

        double maxRadius = 0;
        
        for( int j=0; j<numObjects; j++ ) {
            DrawableObject *currentObject = *( pieceObjects->getElement( j ) );

            currentObject->scale( mSculptureScale );
            
            currentObject->rotate(
                mCurrentPieceRotations[i] );

            currentObject->move( mCurrentPiecePositions[i] );

            double radius =
                currentObject->getBorderMaxDistance(
                    mCurrentPiecePositions[i] );

            if( radius > maxRadius ) {
                maxRadius = radius;
                }
            
            returnVector->push_back( currentObject );
            }

        mCurrentPieceRadii[i] = maxRadius;
        
        delete pieceObjects;
        }

    return returnVector;
    }



char SculptureManager::isPieceJarred( int inPieceHandle ) {
    // piece is only being jarred if force is increasing
    if( mJarForcesIncreasing[ inPieceHandle ] > 0 ) {
        return true;
        }
    else {
        return false;
        }
    }



void SculptureManager::mapParameters( double inParameter,
                                      double *outCloseRangePowerUp,
                                      double *outFarRangePowerUp ) {

    
    if( mNumParameterMapAnchors >= 2 ) {
        // find the 2 surrounding anchor points

        // all distances will be no more than 1
        double distanceFromClosestLargerPoint = 2;
        double distanceFromClosestSmallerPoint = 3;
        
        int indexOfClosestLargerPoint = -1;
        int indexOfClosestSmallerPoint = -1;

        for( int i=0; i<mNumParameterMapAnchors; i++ ) {

            double distanceFromThisPoint =
                fabs( mParameterMapAnchors[i] - inParameter );  

            if( mParameterMapAnchors[i] >= inParameter ) {
                // a larger point
                if( distanceFromThisPoint < distanceFromClosestLargerPoint ) {
                    // closer than our closest largerpoint
                    
                    indexOfClosestLargerPoint = i;
                    distanceFromClosestLargerPoint = distanceFromThisPoint;
                    }
                }
            else {
                // a smaller point
                if( distanceFromThisPoint < distanceFromClosestSmallerPoint ) {
                    // closer than our closest smallerpoint

                    indexOfClosestSmallerPoint = i;
                    distanceFromClosestSmallerPoint = distanceFromThisPoint;
                    }
                }
            }
        if( indexOfClosestLargerPoint != -1  &&
            indexOfClosestSmallerPoint != -1 ) {
            // found two points
            
            // compute weights
            double distanceBetweenSurroundingPoints =
                fabs( mParameterMapAnchors[
                          indexOfClosestLargerPoint ] -
                      mParameterMapAnchors[
                          indexOfClosestSmallerPoint ] );

            double weightOnLargerPoint =
                distanceFromClosestSmallerPoint /
                distanceBetweenSurroundingPoints;
            double weightOnSmallerPoint = 1 - weightOnLargerPoint;

            // blend the two points, using the distance to weight them
            *outCloseRangePowerUp =
                weightOnLargerPoint *
                mParameterMapCloseRangeValues[ indexOfClosestLargerPoint ] +
                weightOnSmallerPoint *
                mParameterMapCloseRangeValues[ indexOfClosestSmallerPoint ];

            *outFarRangePowerUp =
                weightOnLargerPoint *
                mParameterMapFarRangeValues[ indexOfClosestLargerPoint ] +
                weightOnSmallerPoint *
                mParameterMapFarRangeValues[ indexOfClosestSmallerPoint ];
            }
        else {
            // found only one point

            int indexOfPoint;
            
            if( indexOfClosestLargerPoint != -1 ) {
                indexOfPoint = indexOfClosestLargerPoint;
                }
            else if( indexOfClosestLargerPoint != -1 ) {
                indexOfPoint = indexOfClosestSmallerPoint;
                }
            else {
                printf( "Error:  found no closest sculpture parameter map "
                        " anchor point.\n" );
                indexOfPoint = 0;
                }

            // return this point's power ups
            *outCloseRangePowerUp =
                mParameterMapCloseRangeValues[ indexOfPoint ];
            *outCloseRangePowerUp =
                mParameterMapCloseRangeValues[ indexOfPoint ];
            }
        }
    else if( mNumParameterMapAnchors == 1 ) {
        // only one anchor point... return its power ups

        *outCloseRangePowerUp =
            mParameterMapCloseRangeValues[ 0 ];
        *outCloseRangePowerUp =
            mParameterMapCloseRangeValues[ 0  ];
        }
    else {
        printf( "Error:  no anchor points in sculpture parameter space.\n" );

        *outCloseRangePowerUp = 0;
        *outFarRangePowerUp = 0;
        }
    }

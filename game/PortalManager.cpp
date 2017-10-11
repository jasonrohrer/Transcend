/*
 * Modification History
 *
 * 2004-October-13   Jason Rohrer
 * Created.
 */



#include "PortalManager.h"



PortalManager::PortalManager( ParameterizedObject *inPortalTemplate,
                              double inPortalScale,
                              double inPortalFadeInTimeSeconds,
                              double inGridWidth )
    : mPortalTemplate( inPortalTemplate ),
      mPortalScale( inPortalScale ),
      mFadeInTimeSeconds( inPortalFadeInTimeSeconds ),
      mFadeFactor( 0 ),
      mGridWidth( inGridWidth ),
      mCurrentPosition( NULL ),
      mCurrentRadius( 0 ),
      mCurrentRotation( new Angle3D( 0, 0, 0 ) ) {

    }
  


PortalManager::~PortalManager() {
    delete mPortalTemplate;

    if( mCurrentPosition != NULL ) {
        delete mCurrentPosition;
        }
    delete mCurrentRotation;
    
    }



void PortalManager::showPortal( Vector3D *inPosition ) {
    mCurrentPosition = inPosition;
    }



char PortalManager::isPortalVisible() {
    if( mCurrentPosition != NULL ) {
        return true;
        }
    else {
        return false;
        }
    }



void PortalManager::passTime( double inTimeDeltaInSeconds,
                              Vector3D *inShipPosition ) {

    // if portal has been shown
    if( mCurrentPosition != NULL ) {
        
        double distanceToShip =
            mCurrentPosition->getDistance( inShipPosition );
    
        // 25 units, up to 20 units from target, is the range
        double compressedDistance =
            ( distanceToShip - 20 ) /
            ( 25 );
        
        // limit the range
        if( compressedDistance > 1 ) {
            compressedDistance = 1;
            }
        else if( compressedDistance < 0 ) {
            compressedDistance = 0;
            }

        // change shape based on ship distance
        mPortalShapeParameter = compressedDistance;
    

        // rotate
        mCurrentRotation->mZ += mCurrentRotationRate * inTimeDeltaInSeconds;

        // fade in
        if( mFadeFactor < 1 ) {
            mFadeFactor +=
                (double)inTimeDeltaInSeconds / (double) mFadeInTimeSeconds;

            if( mFadeFactor > 1 ) {
                mFadeFactor = 1;
                }
            }
        }
    }



char PortalManager::isShipInPortal( Vector3D *inShipPosition ) {

    if( mCurrentPosition != NULL ) {
        double distance = mCurrentPosition->getDistance( inShipPosition );

        if( distance < mCurrentRadius ) {
            return true;
            }
        else {
            return false;
            }
        }
    else {
        // portal not shown
        return false;
        }
    }


SimpleVector<DrawableObject *> *PortalManager::getDrawableObjects() {

    // if portal has been shown
    if( mCurrentPosition != NULL ) {
        
        SimpleVector<DrawableObject *> *objects =
            mPortalTemplate->getDrawableObjects(
                mPortalShapeParameter, &mCurrentRotationRate ); 

        // scale, rotate, position, and fade the objects
    
        int numObjects = objects->size();
    
        // compute the maximum radius of this enemy
        double maxRadius = 0;
        
        for( int j=0; j<numObjects; j++ ) {
            
            DrawableObject *currentObject =
                *( objects->getElement( j ) );

            currentObject->scale( mPortalScale );
            currentObject->rotate( mCurrentRotation );
            currentObject->move( mCurrentPosition );
            currentObject->fade( mFadeFactor );

            double radius = currentObject->getBorderMaxDistance(
                mCurrentPosition );

            if( radius > maxRadius ) {
                maxRadius = radius;
                }
            }

        mCurrentRadius = maxRadius;        

        return objects;
        }
    else {
        // return an empty vector
        return new SimpleVector<DrawableObject *>();
        }
    }

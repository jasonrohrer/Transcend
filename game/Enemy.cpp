/*
 * Modification History
 *
 * 2004-June-18   Jason Rohrer
 * Created.
 *
 * 2004-August-24   Jason Rohrer
 * Added extra parameter for enemy distance from ship.
 *
 * 2004-August-30   Jason Rohrer
 * Optimization:  avoid object blending whenever possible.
 */



#include "Enemy.h"
#include "LevelDirectoryManager.h"

#include <stdio.h>



Enemy::Enemy( FILE *inFILE, char *outError )
    : mEnemyCloseShapeObject( NULL ), mEnemyFarShapeObject( NULL ),
      mExplosionShapeObject( NULL ) {
    
    char *enemyCloseFileName = new char[ 100 ];
    char *enemyFarFileName = new char[ 100 ];
    char *explosionFileName = new char[ 100 ];

    int numRead = 0;

    numRead += fscanf( inFILE, "%99s", enemyCloseFileName );
    numRead += fscanf( inFILE, "%99s", enemyFarFileName );
    numRead += fscanf( inFILE, "%99s", explosionFileName );
    
    if( numRead != 3 ) {
        *outError = true;

        delete [] enemyCloseFileName;
        delete [] enemyFarFileName;
        delete [] explosionFileName;
        return;
        }

    
    File *levelDirectory = LevelDirectoryManager::getLevelDirectory();

    File *enemyCloseFile = levelDirectory->getChildFile( enemyCloseFileName );
    File *enemyFarFile = levelDirectory->getChildFile( enemyFarFileName );
    File *explosionFile = levelDirectory->getChildFile( explosionFileName );

    delete [] enemyCloseFileName;
    delete [] enemyFarFileName;
    delete [] explosionFileName;
    
    delete levelDirectory;


    
    char *enemyCloseFilePath = enemyCloseFile->getFullFileName();
    char *enemyFarFilePath = enemyFarFile->getFullFileName();
    char *explosionFilePath = explosionFile->getFullFileName();

    delete enemyCloseFile;
    delete enemyFarFile;
    delete explosionFile;
    
    
    FILE *enemyCloseFILE = fopen( enemyCloseFilePath, "r" );

    if( enemyCloseFILE == NULL ) {
        printf( "Failed to open file %s\n", enemyCloseFilePath );

        *outError = true;
        
        delete [] enemyCloseFilePath;
        delete [] enemyFarFilePath;
        delete [] explosionFilePath;
        return;
        }

    FILE *enemyFarFILE = fopen( enemyFarFilePath, "r" );

    if( enemyFarFILE == NULL ) {
        printf( "Failed to open file %s\n", enemyFarFilePath );

        *outError = true;

        fclose( enemyCloseFILE );
        
        delete [] enemyCloseFilePath;
        delete [] enemyFarFilePath;
        delete [] explosionFilePath;
        return;
        }

    
    FILE *explosionFILE = fopen( explosionFilePath, "r" );

    if( explosionFILE == NULL ) {
        printf( "Failed to open file %s\n", explosionFilePath );

        *outError = true;

        fclose( enemyCloseFILE );
        fclose( enemyFarFILE );
        
        delete [] enemyCloseFilePath;
        delete [] enemyFarFilePath;
        delete [] explosionFilePath;

        return;
        }


    delete [] enemyCloseFilePath;
    delete [] enemyFarFilePath;
    delete [] explosionFilePath;


    
    mEnemyCloseShapeObject = new ParameterizedObject( enemyCloseFILE,
                                                      outError );
    mEnemyFarShapeObject = new ParameterizedObject( enemyFarFILE,
                                                    outError );

    mExplosionShapeObject = new ParameterizedObject( explosionFILE, outError );

    fclose( enemyCloseFILE );
    fclose( enemyFarFILE );
    fclose( explosionFILE );    
    }


        
Enemy::~Enemy() {
    delete mEnemyCloseShapeObject;
    delete mEnemyFarShapeObject;
    delete mExplosionShapeObject;
    }



SimpleVector<DrawableObject *> *Enemy::getDrawableObjects(
    double inEnemyShapeParameter,
    double inEnemyDistanceFromShipParameter,
    double inExplosionShapeParameter,
    double inExplosionProgress,
    double *outRotationRate ) {

    double explosionWeight = inExplosionProgress;

    ObjectParameterSpaceControlPoint *enemyControlPoint = NULL;

    // skip blending whenver we can (at either end of our parameter range)


    // don't even compute enemy shape if our explosion progress is 1
    if( inExplosionProgress < 1 ) {
    

        // avoid blending close/far points if ship is at either end of
        // the distance range
        if( inEnemyDistanceFromShipParameter == 0 ) {
            // use close control point
            
            enemyControlPoint = mEnemyCloseShapeObject->getBlendedControlPoint(
                inEnemyShapeParameter );
            }
        else if( inEnemyDistanceFromShipParameter == 1 ) {
            // use far control point

            enemyControlPoint = mEnemyFarShapeObject->getBlendedControlPoint(
                inEnemyShapeParameter );
            }
        else {
            // use a blend of the points
            
            
            ObjectParameterSpaceControlPoint *enemyCloseControlPoint =
                mEnemyCloseShapeObject->getBlendedControlPoint(
                    inEnemyShapeParameter );
            
            ObjectParameterSpaceControlPoint *enemyFarControlPoint =
                mEnemyFarShapeObject->getBlendedControlPoint(
                    inEnemyShapeParameter );
            
            enemyControlPoint =
                (ObjectParameterSpaceControlPoint*)(
                    enemyCloseControlPoint->createLinearBlend(
                        enemyFarControlPoint,
                        inEnemyDistanceFromShipParameter ) );
            
            delete enemyCloseControlPoint;
            delete enemyFarControlPoint;
            }
        }


    // again, try to skip blending...
    // this time, we might need to blend the enemy shape with its explosion
    // shape
    ObjectParameterSpaceControlPoint *blendedPoint;


    if( inExplosionProgress == 0 ) {
        // use the pure enemy shape
        blendedPoint = enemyControlPoint;
        }
    else if( inExplosionProgress == 1 ) {
        // enemyControlPoint is NULL

        // use the pure explosion shape
        
        blendedPoint = 
            mExplosionShapeObject->getBlendedControlPoint(
                inExplosionShapeParameter );
        }
    else {

        // blend the enemy with the explosion shape
        
        ObjectParameterSpaceControlPoint *explosionControlPoint =
            mExplosionShapeObject->getBlendedControlPoint(
                inExplosionShapeParameter );

        blendedPoint =
            (ObjectParameterSpaceControlPoint *)(
                enemyControlPoint->createLinearBlend( explosionControlPoint,
                                                      explosionWeight ) );

        delete enemyControlPoint;
        delete explosionControlPoint;
        }


    SimpleVector<DrawableObject*> *drawableObjects =
            blendedPoint->getDrawableObjects();
        
    *outRotationRate = blendedPoint->getRotationRate();
        
    delete blendedPoint;
        
    return drawableObjects;
    }

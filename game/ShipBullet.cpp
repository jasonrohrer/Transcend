/*
 * Modification History
 *
 * 2004-June-15   Jason Rohrer
 * Created.
 */



#include "ShipBullet.h"
#include "LevelDirectoryManager.h"

#include <stdio.h>



ShipBullet::ShipBullet( FILE *inFILE, char *outError )
    : mCloseRangeObject( NULL ), mFarRangeObject( NULL ) {
    
    char *closeRangeFileName = new char[ 100 ];
    char *farRangeFileName = new char[ 100 ];

    int numRead;

    numRead = fscanf( inFILE, "%99s", closeRangeFileName );
    
    if( numRead != 1 ) {
        *outError = true;

        delete [] closeRangeFileName;
        delete [] farRangeFileName;
        return;
        }

    numRead = fscanf( inFILE, "%99s", farRangeFileName );

    if( numRead != 1 ) {
        *outError = true;
        
        delete [] closeRangeFileName;
        delete [] farRangeFileName;
        return;
        }

    
    File *levelDirectory = LevelDirectoryManager::getLevelDirectory();

    File *closeRangeFile = levelDirectory->getChildFile( closeRangeFileName );
    File *farRangeFile = levelDirectory->getChildFile( farRangeFileName );

    delete [] closeRangeFileName;
    delete [] farRangeFileName;
    
    delete levelDirectory;


    
    char *closeRangeFilePath = closeRangeFile->getFullFileName();
    char *farRangeFilePath = farRangeFile->getFullFileName();

    delete closeRangeFile;
    delete farRangeFile;
    
    
    FILE *closeRangeFILE = fopen( closeRangeFilePath, "r" );

    if( closeRangeFILE == NULL ) {
        printf( "Failed to open file %s\n", closeRangeFilePath );

        *outError = true;
        
        delete [] closeRangeFilePath;
        delete [] farRangeFilePath;
        return;
        }
        
    FILE *farRangeFILE = fopen( farRangeFilePath, "r" );

    if( farRangeFILE == NULL ) {
        printf( "Failed to open file %s\n", farRangeFilePath );

        *outError = true;

        fclose( farRangeFILE );
        
        delete [] closeRangeFilePath;
        delete [] farRangeFilePath;
        return;
        }


    delete [] closeRangeFilePath;
    delete [] farRangeFilePath;


    
    mCloseRangeObject = new ParameterizedObject( closeRangeFILE, outError );

    mFarRangeObject = new ParameterizedObject( farRangeFILE, outError );

    fclose( closeRangeFILE );
    fclose( farRangeFILE );    
    }


        
ShipBullet::~ShipBullet() {
    delete mCloseRangeObject;
    delete mFarRangeObject;
    }



SimpleVector<DrawableObject *> *ShipBullet::getDrawableObjects(
    double inCloseRangeParameter,
    double inFarRangeParameter,
    double inPositionInRange,
    double *outPower,
    double *outRotationRate ) {

    double farWeight = inPositionInRange;
    double closeWeight = 1 - inPositionInRange;
    
    
    *outPower =
        farWeight * inFarRangeParameter +
        closeWeight * inCloseRangeParameter;

    ObjectParameterSpaceControlPoint *closeControlPoint =
        mCloseRangeObject->getBlendedControlPoint( inCloseRangeParameter );

    ObjectParameterSpaceControlPoint *farControlPoint =
        mFarRangeObject->getBlendedControlPoint( inFarRangeParameter );

    ObjectParameterSpaceControlPoint *blendedPoint =
        (ObjectParameterSpaceControlPoint *)(
            closeControlPoint->createLinearBlend( farControlPoint,
                                                  farWeight ) );

    delete closeControlPoint;
    delete farControlPoint;


    SimpleVector<DrawableObject*> *drawableObjects =
            blendedPoint->getDrawableObjects();
        
    *outRotationRate = blendedPoint->getRotationRate();
        
    delete blendedPoint;
        
    return drawableObjects;
    }

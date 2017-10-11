/*
 * Modification History
 *
 * 2004-August-15   Jason Rohrer
 * Created.
 */



#include "BulletSound.h"
#include "LevelDirectoryManager.h"

#include <stdio.h>



BulletSound::BulletSound( FILE *inFILE, char *outError )
    : mCloseRangeSpace( NULL ), mFarRangeSpace( NULL ) {
    
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


    
    mCloseRangeSpace =
        new ParameterizedStereoSound( closeRangeFILE, outError );

    mFarRangeSpace =
        new ParameterizedStereoSound( farRangeFILE, outError );

    fclose( closeRangeFILE );
    fclose( farRangeFILE );    
    }


        
BulletSound::~BulletSound() {
    delete mCloseRangeSpace;
    delete mFarRangeSpace;
    }



PlayableSound *BulletSound::getPlayableSound(
    double inCloseRangeParameter,
    double inFarRangeParameter,
    unsigned long inSamplesPerSecond ) {

    StereoSoundParameterSpaceControlPoint *closeControlPoint =
        mCloseRangeSpace->getBlendedControlPoint( inCloseRangeParameter );

    StereoSoundParameterSpaceControlPoint *farControlPoint =
        mFarRangeSpace->getBlendedControlPoint( inFarRangeParameter );

    // create a 50/50 blend of close and far sounds
    StereoSoundParameterSpaceControlPoint *blendedPoint =
        (StereoSoundParameterSpaceControlPoint *)(
            closeControlPoint->createLinearBlend( farControlPoint,
                                                  0.5 ) );


    // blend lengths
    double soundLength =
        0.5 * (
            mCloseRangeSpace->getSoundLengthInSeconds() +
            mFarRangeSpace->getSoundLengthInSeconds() );

    
    delete closeControlPoint;
    delete farControlPoint;

    
    PlayableSound *sound =
            blendedPoint->getPlayableSound( inSamplesPerSecond,
                                            soundLength );

    delete blendedPoint;
        
    return sound;
    }

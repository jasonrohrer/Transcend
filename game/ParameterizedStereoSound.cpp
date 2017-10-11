/*
 * Modification History
 *
 * 2004-August-9   Jason Rohrer
 * Created.
 */



#include "ParameterizedStereoSound.h"
#include "StereoSoundParameterSpaceControlPoint.h"
#include "SoundParameterSpaceControlPoint.h"
#include "OnePointPlayableSound.h"



ParameterizedStereoSound::ParameterizedStereoSound( FILE *inFILE,
                                                    char *outError ) {

    char readError = false;

    // read the sound length
    int numRead = fscanf( inFILE, "%lf", &mSoundLengthInSeconds );

    if( numRead != 1 ) {
        readError = true;
        
        printf( "Error:  failed to read sound length from sound space.\n" );
        }
    
    SimpleVector<ParameterSpaceControlPoint *> *controlPoints =
        new SimpleVector<ParameterSpaceControlPoint*>();
    SimpleVector<double> *controlPointParameterAnchors =
        new SimpleVector<double>();

    // keep reading parameter anchors and control points until we
    // can read no more
    
    while( !readError ) {
        
        // read the parameter space anchor
        double anchor = 0;
        numRead = fscanf( inFILE, "%lf", &anchor );

        if( numRead != 1 ) {
            readError = true;
            }
        else {

            // read the control point
            StereoSoundParameterSpaceControlPoint *point =
                new StereoSoundParameterSpaceControlPoint( inFILE,
                                                      &readError );

            if( !readError ) {
                controlPointParameterAnchors->push_back( anchor );
                controlPoints->push_back( point );
                }
            else {
                delete point;
                }
            }
        }

    mNumControlPoints = controlPoints->size();
    mControlPoints = controlPoints->getElementArray();
    mControlPointParameterAnchors =
        controlPointParameterAnchors->getElementArray();

    delete controlPoints;
    delete controlPointParameterAnchors;

    if( mNumControlPoints >= 2 ) {
        *outError = false;
        }
    else {
        // we didn't read enough control points
        *outError = true;
        }
    }



PlayableSound *ParameterizedStereoSound::getPlayableSound(
    double inParameter,
    unsigned long inSamplesPerSecond ) {

    // blend the two points, using the distance to weight them
    StereoSoundParameterSpaceControlPoint *blendedPoint =
        getBlendedControlPoint( inParameter );
    
    if( blendedPoint != NULL ) {

        return new OnePointPlayableSound( blendedPoint,
                                          mSoundLengthInSeconds,
                                          inSamplesPerSecond );
        }
    else {
        printf( "Error:  no control points in sound space.\n" );

        return NULL;
        }

    }



StereoSoundParameterSpaceControlPoint *ParameterizedStereoSound::
    getBlendedControlPoint(
        double inParameter ) {
    
    // cast result of super-class function call and return it
    return (StereoSoundParameterSpaceControlPoint*)
        ParameterizedSpace::getBlendedControlPoint( inParameter );
    
    }



double ParameterizedStereoSound::getSoundLengthInSeconds() {
    return mSoundLengthInSeconds;
    }


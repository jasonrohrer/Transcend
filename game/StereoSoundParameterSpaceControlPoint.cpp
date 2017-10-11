/*
 * Modification History
 *
 * 2004-August-9   Jason Rohrer
 * Created.
 *
 * 2004-August-12   Jason Rohrer
 * Added support for getting blocks of samples.
 *
 * 2004-August-15   Jason Rohrer
 * Added function that generates a playable sound.
 */



#include "StereoSoundParameterSpaceControlPoint.h"
#include "OnePointPlayableSound.h"



StereoSoundParameterSpaceControlPoint::StereoSoundParameterSpaceControlPoint(
    SoundParameterSpaceControlPoint *inLeftPoint,
    SoundParameterSpaceControlPoint *inRightPoint )
    : mLeftPoint( inLeftPoint ), mRightPoint( inRightPoint ) {

    }



StereoSoundParameterSpaceControlPoint::StereoSoundParameterSpaceControlPoint(
    FILE *inFILE, char *outError ) {

    char errorLeft = false;
    char errorRight = false;
    
    mLeftPoint = new SoundParameterSpaceControlPoint( inFILE, &errorLeft );

    mRightPoint = new SoundParameterSpaceControlPoint( inFILE, &errorRight );


    *outError = errorLeft || errorRight;

    }


        
StereoSoundParameterSpaceControlPoint::
    ~StereoSoundParameterSpaceControlPoint() {

    delete mLeftPoint;
    delete mRightPoint;
    }



ParameterSpaceControlPoint *StereoSoundParameterSpaceControlPoint::copy() {

    // make copy of each channel point
    return new StereoSoundParameterSpaceControlPoint(
        (SoundParameterSpaceControlPoint *)( mLeftPoint->copy() ),
        (SoundParameterSpaceControlPoint *)( mRightPoint->copy() ) );
    
    }



ParameterSpaceControlPoint *StereoSoundParameterSpaceControlPoint::
    createLinearBlend( ParameterSpaceControlPoint *inOtherPoint,
                       double inWeightOfOtherPoint ) {

    StereoSoundParameterSpaceControlPoint *otherPoint =
        (StereoSoundParameterSpaceControlPoint *)inOtherPoint;
    
    SoundParameterSpaceControlPoint *blendLeft =
        (SoundParameterSpaceControlPoint *)(
            mLeftPoint->createLinearBlend( otherPoint->mLeftPoint,
                                           inWeightOfOtherPoint ) );

    SoundParameterSpaceControlPoint *blendRight =
        (SoundParameterSpaceControlPoint *)(
            mRightPoint->createLinearBlend( otherPoint->mRightPoint,
                                            inWeightOfOtherPoint ) );

    return new StereoSoundParameterSpaceControlPoint( blendLeft,
                                                      blendRight );    
    }



SoundSamples *StereoSoundParameterSpaceControlPoint::getSoundSamples(
    unsigned long inStartSample,
    unsigned long inSampleCount,
    unsigned long inSamplesPerSecond,
    double inSoundLengthInSeconds ) {


    float *leftSamples = mLeftPoint->getSoundSamples( inStartSample,
                                                      inSampleCount,
                                                      inSamplesPerSecond,
                                                      inSoundLengthInSeconds );
    float *rightSamples = mRightPoint->getSoundSamples( inStartSample,
                                                      inSampleCount,
                                                      inSamplesPerSecond,
                                                      inSoundLengthInSeconds );

    return new SoundSamples( inSampleCount, leftSamples, rightSamples );
    }



PlayableSound *StereoSoundParameterSpaceControlPoint::getPlayableSound(
    unsigned long inSamplesPerSecond,
    double inSoundLengthInSeconds ) {
    
    return new OnePointPlayableSound(
        (StereoSoundParameterSpaceControlPoint *)( this->copy() ),
        inSoundLengthInSeconds,
        inSamplesPerSecond );
    }

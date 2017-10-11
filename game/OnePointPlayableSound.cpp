/*
 * Modification History
 *
 * 2004-August-9   Jason Rohrer
 * Created.
 *
 * 2004-August-12   Jason Rohrer
 * Added support for getting blocks of samples.
 */


#include "OnePointPlayableSound.h"



OnePointPlayableSound::OnePointPlayableSound(
    StereoSoundParameterSpaceControlPoint *inControlPoint,
    double inSoundLengthInSeconds,
    int inSamplesPerSecond )
    : mControlPoint( inControlPoint ),
      mSoundLengthInSeconds( inSoundLengthInSeconds ),
      mSamplesPerSecond( inSamplesPerSecond ),
      mSoundLengthInSamples(
          (unsigned long)( inSoundLengthInSeconds * inSamplesPerSecond ) ),
      mCurrentSoundPositionInSamples( 0 ) {

    }



OnePointPlayableSound::~OnePointPlayableSound() {
    delete mControlPoint;
    }



SoundSamples *OnePointPlayableSound::getMoreSamples(
    unsigned long inNumSamples ) {

    unsigned long numSamples = inNumSamples;
    
    if( mCurrentSoundPositionInSamples + numSamples >
        mSoundLengthInSamples ) {

        numSamples = mSoundLengthInSamples - mCurrentSoundPositionInSamples;
        }
    
    SoundSamples *resultSamples =
        mControlPoint->getSoundSamples( mCurrentSoundPositionInSamples,
                                        numSamples,
                                        mSamplesPerSecond,
                                        mSoundLengthInSeconds );

    mCurrentSoundPositionInSamples += numSamples;
    
    return resultSamples;
    }



PlayableSound *OnePointPlayableSound::copy() {
    return new OnePointPlayableSound(
        (StereoSoundParameterSpaceControlPoint *)(
            mControlPoint->copy() ),
        mSoundLengthInSeconds,
        mSamplesPerSecond );

    }


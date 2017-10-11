/*
 * Modification History
 *
 * 2004-August-9   Jason Rohrer
 * Created.
 */



#ifndef ONE_POINT_PLAYABLE_SOUND_INCLUDED
#define ONE_POINT_PLAYABLE_SOUND_INCLUDED



#include "PlayableSound.h"
#include "StereoSoundParameterSpaceControlPoint.h"



/**
 * Implementation of PlayableSound that plays one sound control
 * point through the duration of the sound.
 *
 * @author Jason Rohrer
 */
class OnePointPlayableSound : public PlayableSound {

        

    public:


        
        /**
         * Constructs a playable sound.
         *
         * @param inControlPoint the sound control point.
         *   Will be destroyed when this class is destroyed.
         * @param inSoundLengthInSeconds the length of the sound in seconds.
         * @param inSamplesPerSecond the sample rate.
         */
        OnePointPlayableSound(
            StereoSoundParameterSpaceControlPoint *inControlPoint,
            double inSoundLengthInSeconds,
            int inSamplesPerSecond );


        ~OnePointPlayableSound();


        
        // implements the PlayableSound interface
        virtual SoundSamples *getMoreSamples( unsigned long inNumSamples );
        virtual PlayableSound *copy();
        

    protected:

        StereoSoundParameterSpaceControlPoint *mControlPoint;
        
        double mSoundLengthInSeconds;
        int mSamplesPerSecond;
        unsigned long mSoundLengthInSamples;

        
        unsigned long mCurrentSoundPositionInSamples;
        
    };



#endif

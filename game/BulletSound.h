/*
 * Modification History
 *
 * 2004-August-15   Jason Rohrer
 * Created.
 */



#ifndef BULLET_SOUND_INCLUDED
#define BULLET_SOUND_INCLUDED



#include "ParameterizedStereoSound.h"
#include "PlayableSound.h"



/**
 * A bullet sound that can be controled with 2 parameters.
 *
 * @author Jason Rohrer.
 */
class BulletSound {


    public:


        
        /**
         * Constructs a bullet sound by reading values from a text file
         * stream.
         *
         * @param inFILE the open file to read from.
         *   Must be closed by caller.
         * @param outError pointer to where error flag should be returned.
         *   Destination will be set to true if reading the bullet
         *   from inFILE fails.
         */
        BulletSound( FILE *inFILE, char *outError );


        
        virtual ~BulletSound();


        
        /**
         * Get a playable sound objects from this sound template.
         *
         * @param inCloseRangeParameter a parameter in the range [0,1] to
         *   control the shape/power of the bullet at close range.
         * @param inFarRangeParameter a parameter in the range [0,1] to
         *   control the shape/power of the bullet at far range.
         * @param inSamplesPerSecond the current sample rate.
         *
         * @return a playable sound.
         *   Must be destroyed by caller.
         */
        PlayableSound *getPlayableSound(
            double inCloseRangeParameter,
            double inFarRangeParameter,
            unsigned long inSamplesPerSecond );


        
    protected:

        ParameterizedStereoSound *mCloseRangeSpace;
        ParameterizedStereoSound *mFarRangeSpace;

        
    };



#endif

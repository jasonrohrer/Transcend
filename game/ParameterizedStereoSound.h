/*
 * Modification History
 *
 * 2004-August-9   Jason Rohrer
 * Created.
 */



#ifndef PARAMETERIZED_STEREO_SOUND_INCLUDED
#define PARAMETERIZED_STEREO_SOUND_INCLUDED



#include "minorGems/math/geometry/Vector3D.h"
#include "minorGems/graphics/Color.h"
#include "minorGems/util/SimpleVector.h"

#include "ParameterizedSpace.h"
#include "PlayableSound.h"
#include "StereoSoundParameterSpaceControlPoint.h"



/**
 * A 1-D space of sound control points.
 *
 * @author Jason Rohrer.
 */
class ParameterizedStereoSound : public ParameterizedSpace {


    public:


        
        /**
         * Constructs a sound by reading values from a text file
         * stream.
         *
         * @param inFILE the open file to read from.
         *   Must be closed by caller.
         * @param outError pointer to where error flag should be returned.
         *   Destination will be set to true if reading the sound
         *   from inFILE fails.
         */
        ParameterizedStereoSound( FILE *inFILE, char *outError );



        /**
         * Gets a playable sound from this sound space.
         *
         * @param inParameter the parameter in the range [0,1] to map
         *   into the sound space.
         * @param inSamplesPerSecond the current sample rate.
         *
         * @return a playable sound
         *   Can return NULL if this space was not properly initialized.
         *   Must be destroyed by caller.
         */
        PlayableSound *getPlayableSound( double inParameter,
                                         unsigned long inSamplesPerSecond );

        

        /**
         * Gets a blended sound control point from this sound space.
         *
         * @param inParameter the parameter in the range [0,1] to map
         *   into the sound space.
         *
         * @return the blended control point.
         *   Can return NULL if this space was not properly initialized.
         *   Must be destroyed by caller.
         */
        StereoSoundParameterSpaceControlPoint *getBlendedControlPoint(
            double inParameter );



        /**
         * Gets the length of the sound represented by this space.
         *
         * @return the sound length in seconds.
         */
        double getSoundLengthInSeconds();

        
        
    protected:

        double mSoundLengthInSeconds;
        
        // inherit other protected members from ParameterizedSpace
        
        
    };



#endif

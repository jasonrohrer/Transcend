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



#ifndef STEREO_SOUND_PARAMETER_SPACE_CONTROL_POINT_INCLUDED
#define STEREO_SOUND_PARAMETER_SPACE_CONTROL_POINT_INCLUDED


#include "ParameterSpaceControlPoint.h"
#include "SoundParameterSpaceControlPoint.h"
#include "SoundSamples.h"
#include "PlayableSound.h"



#include <stdio.h>



/**
 * A control point in a 1-D parameterized stereo sound space.
 *
 * @author Jason Rohrer.
 */
class StereoSoundParameterSpaceControlPoint :
    public ParameterSpaceControlPoint {


    public:


        
        /**
         * Constructs a control point.
         *
         * @param inLeftPoint the left control point.
         *   Will be destroyed when this class is destroyed.
         * @param inRightPoint the right control point.
         *   Will be destroyed when this class is destroyed.
         */
        StereoSoundParameterSpaceControlPoint(
            SoundParameterSpaceControlPoint *inLeftPoint,
            SoundParameterSpaceControlPoint *inRightPoint );
        
        

        /**
         * Constructs a control point by reading values from a text file
         * stream.
         *
         * @param inFILE the open file to read from.
         *   Must be closed by caller.
         * @param outError pointer to where error flag should be returned.
         *   Destination will be set to true if reading a control point
         *   from inFILE fails.
         */
        StereoSoundParameterSpaceControlPoint( FILE *inFILE, char *outError );
        

        
        virtual ~StereoSoundParameterSpaceControlPoint();


        
        // implements the ParameterSpaceControlPoint interface
        ParameterSpaceControlPoint *copy();

        ParameterSpaceControlPoint *createLinearBlend(
            ParameterSpaceControlPoint *inOtherPoint,
            double inWeightOfOtherPoint );


        
        /**
         * Gets a block of samples from this control point.
         *
         * @param inStartSample the index of the first sample to get.
         * @param inSampleCount the number of samples to get.
         * @param inSamplesPerSecond the current sample rate.
         * @param inSoundLengthInSeconds the total length of the sound being
         *   played.
         *
         * @return the samples.
         *   Must be destroyed by caller.
         */
        SoundSamples *getSoundSamples( unsigned long inStartSample,
                                       unsigned long inSampleCount,
                                       unsigned long inSamplesPerSecond,
                                       double inSoundLengthInSeconds );

        

        /**
         * Gets a playable sound from this control point.
         *
         * @param inSamplesPerSecond the sample rate.
         * @param inSoundLengthInSeconds the length of the sound.
         *
         * @return a playable sound.
         *   Must be destroyed by caller.
         */
        PlayableSound *getPlayableSound( unsigned long inSamplesPerSecond,
                                         double inSoundLengthInSeconds );

        
        
        // these are public so that other Stereo Sound points can access
        // them when performing a blend.
        SoundParameterSpaceControlPoint *mLeftPoint;
        SoundParameterSpaceControlPoint *mRightPoint;

        
    };



#endif


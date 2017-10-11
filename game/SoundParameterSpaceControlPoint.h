/*
 * Modification History
 *
 * 2004-August-6   Jason Rohrer
 * Created.
 *
 * 2004-August-9   Jason Rohrer
 * Changed to use sine components.
 *
 * 2004-August-12   Jason Rohrer
 * Added support for getting blocks of samples.
 *
 * 2004-August-19   Jason Rohrer
 * Fixed bug in walking through wavetable.
 */



#ifndef SOUND_PARAMETER_SPACE_CONTROL_POINT_INCLUDED
#define SOUND_PARAMETER_SPACE_CONTROL_POINT_INCLUDED


#include "ParameterSpaceControlPoint.h"


#include <stdio.h>



/**
 * A control point in a 1-D parameterized sound space.
 *
 * @author Jason Rohrer.
 */
class SoundParameterSpaceControlPoint : public ParameterSpaceControlPoint {


    public:


        
        /**
         * Constructs a control point.
         *
         * Note that the waveform is limited to the amplitude range [-1,1]
         * and will clip if the sine components sum to produce amplitudes
         * ouside of this range.
         *
         * Start/end freqency and loudness are used to create a linear
         * sweep of these values as the sound plays.
         *
         * @param inNumWaveComponents the number of wave components.
         * @param inWaveComponentFreqencies the frequency of each sine
         *   component.  The frequency is in cycles per waveform (in other
         *   words, how many times this sine component cycles during
         *   the length of the waveform).
         *   This array will be destroyed when this class is destroyed.
         * @param inWaveComponentAmplitudes the peak amplitude for
         *   each sine component.
         *   Each value must be in the range [0,1].
         *   This array will be destroyed when this class is destroyed.
         * @param inStartFrequency the number of times the waveform should
         *   play per second at the start of the sound.
         * @param inEndFrequency the number of times the waveform should
         *   play per second at by the end of the sound.
         * @param inStartLoudness the loudness of the start of the sound
         *   in the range [0,1].
         * @param inEndLoudness the loudness of the end of the sound
         *   in the range [0,1].
         */
        SoundParameterSpaceControlPoint(
            int inNumWaveComponents,
            double *inWaveComponentFreqencies,
            double *inWaveComponentAmplitudes,
            double inStartFrequency,
            double inEndFrequency,
            double inStartLoudness,
            double inEndLoudness );
        
        

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
        SoundParameterSpaceControlPoint( FILE *inFILE, char *outError );
        

        
        virtual ~SoundParameterSpaceControlPoint();


        
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
        float *getSoundSamples( unsigned long inStartSample,
                                unsigned long inSampleCount,
                                unsigned long inSamplesPerSecond,
                                double inSoundLengthInSeconds );
        
        
        
        // these are public so that other Sound points can access
        // them when performing a blend.
        int mNumWaveComponents;
        double *mWaveComponentFrequencies;
        double *mWaveComponentAmplitudes;
        double mStartFrequency;
        double mEndFrequency;
        double mStartLoudness;
        double mEndLoudness; 


        
    protected:

        

        /**
         * Copies an array of doubles, producing a newly constructed array.
         *
         * @param inArray the array to copy.
         *   Must be destroyed by caller.
         * @param inLength the length of inArray.
         *
         * @return the copied array.
         *   Must be destroyed by caller.
         */
        double *copyDoubleArray( double *inArray, int inLength );



        // used when generating sound samples
        double mCurrentWavePoint;
        

    };



#endif


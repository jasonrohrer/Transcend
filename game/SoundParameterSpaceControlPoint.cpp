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
 *
 * 2004-September-3   Jason Rohrer
 * Added brief fade in/out at beginning/end of sound to avoid clicks.
 */



#include "SoundParameterSpaceControlPoint.h"


#include "minorGems/math/geometry/Vector3D.h"



#include <math.h>



SoundParameterSpaceControlPoint::SoundParameterSpaceControlPoint(
    int inNumWaveComponents,
    double *inWaveComponentFrequencies,
    double *inWaveComponentAmplitudes,
    double inStartFrequency,
    double inEndFrequency,
    double inStartLoudness,
    double inEndLoudness )
    : mNumWaveComponents( inNumWaveComponents ),
      mWaveComponentFrequencies( inWaveComponentFrequencies ),
      mWaveComponentAmplitudes( inWaveComponentAmplitudes ),
      mStartFrequency( inStartFrequency ),
      mEndFrequency( inEndFrequency ),
      mStartLoudness( inStartLoudness ),
      mEndLoudness( inEndLoudness ),
      mCurrentWavePoint( 0 ) {

    }



SoundParameterSpaceControlPoint::SoundParameterSpaceControlPoint(
    FILE *inFILE, char *outError ) {

    int totalNumRead = 0;
    
    mNumWaveComponents = 0;
    totalNumRead += fscanf( inFILE, "%d", &mNumWaveComponents );

    // how many values should we successfully read (cheap error checking)
    int totalToRead =
        1 +                 // read the number of wave points
        mNumWaveComponents * 2  + // for each point, read x and y
        + 4;                // read Start/End frequency and loudness
        
    
    mWaveComponentFrequencies = new double[ mNumWaveComponents ];
    mWaveComponentAmplitudes = new double[ mNumWaveComponents ];

    // read frequency and amplitude for each component
    for( int i=0; i<mNumWaveComponents; i++ ) {
        double frequency = 0;
        double amplitude = 0;

        totalNumRead += fscanf( inFILE, "%lf", &frequency );
        totalNumRead += fscanf( inFILE, "%lf", &amplitude );

        mWaveComponentFrequencies[i] = frequency;
        mWaveComponentAmplitudes[i] = amplitude;
        }

    mStartFrequency = 0;
    mEndFrequency = 0;
    mStartLoudness = 1;
    mEndLoudness = 1;
    
    totalNumRead += fscanf( inFILE, "%lf", &mStartFrequency );
    totalNumRead += fscanf( inFILE, "%lf", &mEndFrequency );
    totalNumRead += fscanf( inFILE, "%lf", &mStartLoudness );
    totalNumRead += fscanf( inFILE, "%lf", &mEndLoudness );


    if( totalNumRead != totalToRead ) {
        printf( "Expecting to read %d values, but read %d\n",
                totalToRead, totalNumRead );
        *outError = true;
        }
    else {
        *outError = false;
        }
    }
        

        
SoundParameterSpaceControlPoint::~SoundParameterSpaceControlPoint() {
    delete [] mWaveComponentFrequencies;
    delete [] mWaveComponentAmplitudes;
    }



ParameterSpaceControlPoint *SoundParameterSpaceControlPoint::copy() {

    return new SoundParameterSpaceControlPoint(
        mNumWaveComponents,
        copyDoubleArray( mWaveComponentFrequencies, mNumWaveComponents ),
        copyDoubleArray( mWaveComponentAmplitudes, mNumWaveComponents ),
        mStartFrequency,
        mEndFrequency,
        mStartLoudness,    
        mEndLoudness );    
    }



ParameterSpaceControlPoint *
    SoundParameterSpaceControlPoint::createLinearBlend(
        ParameterSpaceControlPoint *inOtherPoint,
        double inWeightOfOtherPoint ) {

    // cast
    SoundParameterSpaceControlPoint *otherPoint =
        (SoundParameterSpaceControlPoint *)inOtherPoint;
    
        
    double weightOfThisPoint = 1 - inWeightOfOtherPoint;

    int otherNumWaveComponents = otherPoint->mNumWaveComponents;
    double *otherWaveComponentFrequencies =
        otherPoint->mWaveComponentFrequencies;
    double *otherWaveComponentAmplitudes =
        otherPoint->mWaveComponentAmplitudes;
    

    // pack our waveform control points into vector3D arrays so
    // they can be processed by the blend function

    Vector3D **thisWaveComponents = new Vector3D*[ mNumWaveComponents ];
    Vector3D **otherWaveComponents =
        new Vector3D*[ otherPoint->mNumWaveComponents ];

    int i;
    for( i=0; i<mNumWaveComponents; i++ ) {
        thisWaveComponents[i] = new Vector3D( mWaveComponentFrequencies[i],
                                          mWaveComponentAmplitudes[i],
                                          0 );
        }

    for( i=0; i<otherNumWaveComponents; i++ ) {
        otherWaveComponents[i] =
            new Vector3D( otherWaveComponentFrequencies[i],
                          otherWaveComponentAmplitudes[i],
                          0 );
        }


    int resultNumWaveComponents;
    
    Vector3D **resultWaveComponents =
        blendVertexArrays(
            thisWaveComponents,
            mNumWaveComponents,
            weightOfThisPoint,
            otherWaveComponents,
            otherNumWaveComponents,
            &resultNumWaveComponents );

    
    for( i=0; i<mNumWaveComponents; i++ ) {
        delete thisWaveComponents[i];
        }
    delete [] thisWaveComponents;

    for( i=0; i<otherNumWaveComponents; i++ ) {
        delete otherWaveComponents[i];
        }
    delete [] otherWaveComponents;

    
    
    // unpack result
    double *resultWaveComponentFrequencies =
        new double[ resultNumWaveComponents ];
    double *resultWaveComponentAmplitudes =
        new double[ resultNumWaveComponents ];

    for( i=0; i<resultNumWaveComponents; i++ ) {
        resultWaveComponentFrequencies[i] = resultWaveComponents[i]->mX;
        resultWaveComponentAmplitudes[i] = resultWaveComponents[i]->mY;
        

        delete resultWaveComponents[i];
        }
    delete [] resultWaveComponents;



    return new SoundParameterSpaceControlPoint(
        resultNumWaveComponents,
        resultWaveComponentFrequencies,
        resultWaveComponentAmplitudes,
        weightOfThisPoint * mStartFrequency +
            inWeightOfOtherPoint * otherPoint->mStartFrequency,
        weightOfThisPoint * mEndFrequency +
            inWeightOfOtherPoint * otherPoint->mEndFrequency,
        weightOfThisPoint * mStartLoudness +
            inWeightOfOtherPoint * otherPoint->mStartLoudness,
        weightOfThisPoint * mEndLoudness +
            inWeightOfOtherPoint * otherPoint->mEndLoudness );
    }



float *SoundParameterSpaceControlPoint::getSoundSamples(
    unsigned long inStartSample,
    unsigned long inSampleCount,
    unsigned long inSamplesPerSecond,
    double inSoundLengthInSeconds ) {

    if( inStartSample == 0 ) {
        // reset our wave pointer
        mCurrentWavePoint = 0;
        }

    double sampleDeltaInSeconds = 1.0 / inSamplesPerSecond;

    
    float *samples = new float[ inSampleCount ];


    unsigned long soundLengthInSamples =
        (unsigned long)( inSoundLengthInSeconds * inSamplesPerSecond );


    // try to fade in and out for 100 samples to avoid a click
    // at the start and end of the note
    unsigned long numFadeInSamples = 100;
    if( numFadeInSamples > soundLengthInSamples ) {
        numFadeInSamples = soundLengthInSamples / 2;
        }
    
    unsigned long numFadeOutSamples = 100;
    if( numFadeOutSamples > soundLengthInSamples ) {
        numFadeOutSamples = soundLengthInSamples / 2;
        }
    
    
    for( unsigned long i=0; i<inSampleCount; i++ ) {
        
        unsigned long currentSample = i + inStartSample;

        double samplePointInSeconds =
            (double)currentSample / (double)inSamplesPerSecond;

        double soundProgress = samplePointInSeconds / inSoundLengthInSeconds;
    

        double currentFrequency =
            soundProgress * mEndFrequency +
            ( 1 - soundProgress ) * mStartFrequency;
        double currentLoudness =
            soundProgress * mEndLoudness +
            ( 1 - soundProgress ) * mStartLoudness;
        
        // compute the time point in our wave, in the range [0,1]

        mCurrentWavePoint += sampleDeltaInSeconds * currentFrequency;


        // check if we are in the sample region that needs to be faded
        double fadeInFactor = 1;
        double fadeOutFactor = 1;

        if( currentSample < numFadeInSamples ) {
            fadeInFactor = (double)currentSample /
                (double)( numFadeInSamples - 1 );
            }
        if( currentSample >= soundLengthInSamples - numFadeOutSamples ) {
            fadeOutFactor =
                (double)( soundLengthInSamples - currentSample - 1 ) /
                (double)( numFadeOutSamples - 1 );
            }

        double fadeFactor = fadeInFactor * fadeOutFactor;
        
        
        // add up the components at this time point

        
        // sine function cycles once every 2*pi
        // we need to adjust the sine function to cycle according to
        // our wave's freqency
        double adjustedTime = mCurrentWavePoint * ( 2 * M_PI );

    

        double componentSum = 0;
    
        for( int j=0; j<mNumWaveComponents; j++ ) {
            componentSum += mWaveComponentAmplitudes[j] *
                sin( mWaveComponentFrequencies[j] * adjustedTime );
            }
        
        samples[i] = (float)( currentLoudness * fadeFactor * componentSum );
        }

    return samples;
    }
      


double *SoundParameterSpaceControlPoint::copyDoubleArray(
    double *inArray, int inLength ) {

    double *result = new double[ inLength ];

    memcpy( (void *)result, (void *)inArray, inLength * sizeof( double ) );

    return result;
    }


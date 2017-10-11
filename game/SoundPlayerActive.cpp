/*
 * Modification History
 *
 * 2004-July-17   Jason Rohrer
 * Created.
 *
 * 2004-July-21   Jason Rohrer
 * Switched to a passive (callback-based) player.  Old player saved here.
 */



#include "SoundPlayerActive.h"


#include <stdio.h>



SoundPlayer::SoundPlayer()
    : mRealtimeSounds( new SimpleVector<SoundSamples *>() ) {

    PaError error = OpenAudioStream( &mAudioStream, 44100, paFloat32,
                                     ( PABLIO_WRITE | PABLIO_MONO ) );
    if( error == paNoError ) {
        mAudioInitialized = true;
        }
    else {
        mAudioInitialized = false;
        
        fprintf( stderr,
                 "An error occured while setting up the sound stream\n" );
        fprintf( stderr, "Error number: %d\n", error );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( error ) );
        
        }
    }



SoundPlayer::~SoundPlayer() {
    if( mAudioInitialized ) {
        PaError error = CloseAudioStream( mAudioStream );

        if( error != paNoError ) {
            fprintf(
                stderr,
                "An error occured while shutting down the sound stream\n" );
            fprintf( stderr, "Error number: %d\n", error);
            fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( error) );
            
            }
        }

    int numSounds = mRealtimeSounds->size();

    for( int i=0; i<numSounds; i++ ) {
        delete [] *( mRealtimeSounds->getElement( i ) );
        }
    delete mRealtimeSounds;
    }



void SoundPlayer::playSoundNow( SoundSamples *inSamples ) {
    mRealtimeSounds->push_back( new SoundSamples( inSamples ) );
    }



unsigned long SoundPlayer::getNumMusicSamplesNeeded() {
    return GetAudioStreamWriteable( mAudioStream );
    }


        
void SoundPlayer::addMoreMusic( SoundSamples *inSamples ) {

    SoundSamples *mixingBuffer = new SoundSamples( inSamples );

    unsigned long bufferLength = mixingBuffer->mSampleCount;

    
    
    // add each pending realtime sound to the buffer

    int i = 0;
    
    // we may be removing sounds from the buffer as we use them up
    // i is adjusted inside the while loop
    while( i<mRealtimeSounds->size() ) {
        
        SoundSamples *realtimeSound = *( mRealtimeSounds->getElement( i ) );

        unsigned long soundLength = realtimeSound->mSampleCount;

        
        // limit length of this mix based on whichever is shorter, the sound
        // or the mixing buffer
        unsigned long mixLength = bufferLength;
        if( mixLength > soundLength ) {
            mixLength = soundLength;
            }
        
        for( unsigned long j=0; j<mixLength; j++ ) {
            mixingBuffer->mLeftChannel[j] += realtimeSound->mLeftChannel[j];
            mixingBuffer->mRightChannel[j] += realtimeSound->mRightChannel[j];
            }
        
        if( mixLength == soundLength ) {
            // we have used up all samples of this sound
            delete realtimeSound;
            mRealtimeSounds->deleteElement( i );

            // don't increment i, since the next element drops into the current
            // index
            }
        else {
            
            // trim off the samples that we have used
            realtimeSound->trim( mixLength );
            
            // increment i to move on to the next sound
            i++;
            }
        }

    /*
    float **samples = new float*[ bufferLength ];

    unsigned long j;
    for( j=0; j<bufferLength; j++ ) {

        samples[j] = new float[2];

        samples[j][0] = mixingBuffer->mLeftChannel[j];
        samples[j][1] = mixingBuffer->mRightChannel[j];
        }

    WriteAudioStream( mAudioStream, samples, bufferLength );


    for( j=0; j<bufferLength; j++ ) {
        delete [] samples[j];
        }
    delete [] samples;
    */

    float *samples = new float[ bufferLength ];

    unsigned long j;
    for( j=0; j<bufferLength; j++ ) {

        samples[j] = mixingBuffer->mLeftChannel[j];
        }

    WriteAudioStream( mAudioStream, samples, bufferLength );

    delete [] samples;


    
    delete mixingBuffer;        
    }

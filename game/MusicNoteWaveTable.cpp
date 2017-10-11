/*
 * Modification History
 *
 * 2004-August-22   Jason Rohrer
 * Created.
 *
 * 2004-August-31   Jason Rohrer
 * Added brief fade-in at note start to reduce clicks.
 */



#include "MusicNoteWaveTable.h"
#include "LevelDirectoryManager.h"

#include "minorGems/util/SimpleVector.h"

#include <stdio.h>
#include <math.h>



MusicNote::MusicNote( int inFrequencyIndex, int inLengthIndex,
                      char inReversed )
    : mFrequencyIndex( inFrequencyIndex ),
      mLengthIndex( inLengthIndex ),
      mReversed( inReversed ) {

    
    }



MusicNote *MusicNote::copy() {
    return new MusicNote( mFrequencyIndex, mLengthIndex, mReversed );
    }



MusicNoteWaveTable::MusicNoteWaveTable( unsigned long inSamplesPerSecond ){


    // read frequencies and lengths from files
    

    SimpleVector<double> *frequencyVector = new SimpleVector<double>();
    SimpleVector<double> *lengthVector = new SimpleVector<double>();
    
    FILE *musicNotePitchesFILE =
        LevelDirectoryManager::getStdStream( "musicNotePitches", true );
    FILE *musicNoteLengthsFILE =
        LevelDirectoryManager::getStdStream( "musicNoteLengths", true );
    

    if( musicNotePitchesFILE != NULL ) {

        double readValue;
        int numRead = 1;

        while( numRead == 1 ) {
            numRead = fscanf( musicNotePitchesFILE, "%lf", &readValue );
            
            if( numRead == 1 ) {
                frequencyVector->push_back( readValue );
                }
            }

        fclose( musicNotePitchesFILE );
        }
    else {
        // default to one pitch
        frequencyVector->push_back( 400.00 );
        }

    

    if( musicNoteLengthsFILE != NULL ) {

        double readValue;
        int numRead = 1;

        while( numRead == 1 ) {
            numRead = fscanf( musicNoteLengthsFILE, "%lf", &readValue );
            
            if( numRead == 1 ) {
                lengthVector->push_back( readValue );
                }
            }

        fclose( musicNoteLengthsFILE );
        }
    else {
        // default to one pitch
        lengthVector->push_back( 400.00 );
        }


    mFrequencyCount = frequencyVector->size();
    mLengthCount = lengthVector->size();

    double *frequencies = frequencyVector->getElementArray();
    mLengthsInSeconds = lengthVector->getElementArray();

    delete frequencyVector;
    delete lengthVector;
    
    
    
    
    mSampleTable = new float**[ mFrequencyCount ];
    mSampleCounts = new unsigned long[ mLengthCount ];

    for( int F=0; F<mFrequencyCount; F++ ) {

        mSampleTable[F] = new float*[ mLengthCount ];

        
        for( int L=0; L<mLengthCount; L++ ) {
            
            // construct a sample table for this freqency/length pair
            unsigned long lengthInSamples =
                (unsigned long)( mLengthsInSeconds[L] * inSamplesPerSecond );

            mSampleTable[F][L] = new float[ lengthInSamples ];


            // setting this inside a double-loop will set the same
            // value repeatedly with the same value, but this makes the code
            // cleaner (other options:  a separate loop to set this value, or
            //  an if statement to ensure that it is set only once)
            mSampleCounts[L] = lengthInSamples;

            

            // populate the sample table with a linearly decaying sine wave
            double frequencyInCyclesPerSecond = frequencies[F];


            double frequencyInCyclesPerSample =
                frequencyInCyclesPerSecond / inSamplesPerSecond;

            // sine function cycles every 2*pi
            // adjust so that it cycles according to our desired frequency
            double adjustedFrequency =
                frequencyInCyclesPerSample * ( 2 * M_PI );

            // try to fade in for 100 samples to avoid a click
            // at the start of the note
            unsigned long numFadeInSamples = 100;
            if( numFadeInSamples > lengthInSamples ) {
                numFadeInSamples = lengthInSamples / 2;
                }
            
            for( unsigned long i=0; i<lengthInSamples; i++ ) {

                // decay loudness linearly
                double loudness =
                    (double)( lengthInSamples - i - 1 ) /
                    (double)( lengthInSamples - 1 );

                // fade in for the first 100 samples to avoid
                // a click
                double fadeInFactor = 1;

                if( i < numFadeInSamples ) {

                    fadeInFactor =
                        (double)( i ) / (double)( numFadeInSamples - 1 );
                    }
                
                mSampleTable[F][L][i] =
                    fadeInFactor * loudness * sin( i * adjustedFrequency );
                }
            }
        }

    
    delete [] frequencies;
    }



MusicNoteWaveTable::~MusicNoteWaveTable(){

    for( int F=0; F<mFrequencyCount; F++ ) {
        for( int L=0; L<mLengthCount; L++ ) {

            delete [] mSampleTable[F][L];

            }
        delete [] mSampleTable[F];
        }
    
    delete [] mSampleTable;
    delete [] mSampleCounts;
    delete [] mLengthsInSeconds;
    }



int MusicNoteWaveTable::getFrequencyCount(){
    return mFrequencyCount;
    }



int MusicNoteWaveTable::getLengthCount(){
    return mLengthCount;
    }



double MusicNoteWaveTable::getLengthInSeconds( int inLengthIndex ) {
    return mLengthsInSeconds[ inLengthIndex ];
    }



float *MusicNoteWaveTable::mapParametersToSamples(
    int inFrequencyIndex,
    int inLengthIndex,
    unsigned long *outNumSamples ){

    *outNumSamples = mSampleCounts[inLengthIndex];

    return mSampleTable[inFrequencyIndex][inLengthIndex];
    }



float *MusicNoteWaveTable::mapNoteToSamples(
    MusicNote *inNote,
    unsigned long *outNumSamples ){


    return mapParametersToSamples( inNote->mFrequencyIndex,
                                   inNote->mLengthIndex,
                                   outNumSamples );
    }







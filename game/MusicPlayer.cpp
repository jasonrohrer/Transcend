/*
 * Modification History
 *
 * 2004-August-22   Jason Rohrer
 * Created.
 *
 * 2004-August-23   Jason Rohrer
 * Fixed bug in note offsets.
 *
 * 2004-August-24   Jason Rohrer
 * Added missing support for reversed notes and stereo split.
 * Changed to use constant power panning.
 */



#include "MusicPlayer.h"
#include "LevelDirectoryManager.h"



MusicPlayer::MusicPlayer( unsigned long inSamplesPerSecond,
                          SculptureManager *inSculptureManager,
                          MusicNoteWaveTable *inWaveTable,
                          double inWorldWidth,
                          double inWorldHeight,
                          double inGridSpaceWidth  )
    : mSculptureManager( inSculptureManager ),
      mWaveTable( inWaveTable ),
      mActiveNotes( new SimpleVector<SoundSamples*>() ),
      mNotePositions( new SimpleVector<unsigned long>() ),
      mSampleRate( inSamplesPerSecond ),
      mWorldWidth( inWorldWidth ),
      mWorldHeight( inWorldHeight ),
      mGridSpaceWidth( inGridSpaceWidth ) {


    // start at far left
    mCurrentPartGridPosition = -mWorldWidth / 2; 

    
    char error = false;
    
    mPartLengthInSeconds =
        LevelDirectoryManager::readDoubleFileContents(
            "musicPartLength", &error, true );

    if( error ) {
        // default to 10 second parts
        mPartLengthInSeconds = 10.0;
        }

    
    
    }



MusicPlayer::~MusicPlayer() {
    int numActiveNotes = mActiveNotes->size();

    for( int i=0; i<numActiveNotes; i++ ) {
        delete *( mActiveNotes->getElement( i ) );
        }

    delete mActiveNotes;
    delete mNotePositions;
    }



SoundSamples *MusicPlayer::getMoreMusic( unsigned long inNumSamples ) {

    double halfWorldWidth = mWorldWidth / 2;
    

    double bufferLengthInSeconds = (double)inNumSamples / (double)mSampleRate;
    double bufferLengthInWorldUnits =
        ( bufferLengthInSeconds / mPartLengthInSeconds ) * mGridSpaceWidth;
    

    
    int numPieces;
    
    Vector3D **positions =
        mSculptureManager->getPiecePositions( &numPieces );

    MusicPart **musicParts =
        mSculptureManager->getPieceMusicParts( &numPieces );

    if( numPieces == 0 ) {
        // no pieces in sculpture... nothing to play

        delete [] positions;
        delete [] musicParts;

        // return silent samples
        return new SoundSamples( inNumSamples );        
        }

    
    // find right-most and left-most piece positions (bounds of song)
    
    double rightMostPiecePosition = -halfWorldWidth;
    double leftMostPiecePosition = halfWorldWidth;
    
    int i;
    for( i=0; i<numPieces; i++ ) {
        double x = positions[i]->mX;
        
        if( x < leftMostPiecePosition ) {
            leftMostPiecePosition = x;
            }
        if( x > rightMostPiecePosition ) {
            rightMostPiecePosition = x;
            }
        }

    
    if( mCurrentPartGridPosition < leftMostPiecePosition ) {
        // jump past empty space at far left
        mCurrentPartGridPosition = leftMostPiecePosition;
        }

    
    if( mCurrentPartGridPosition > rightMostPiecePosition + mGridSpaceWidth ) {
        // beyond music part of right-most sculpture piece

        // wrap around to the left-most in-sculpture piece
        mCurrentPartGridPosition = leftMostPiecePosition;
        }


    // find the pieces that play during this buffer
    for( i=0; i<numPieces; i++ ) {
        double x = positions[i]->mX;
        double y = positions[i]->mY;
        // if piece either starts during this buffer or started in a previous
        // buffer but is still playing during this buffer, then play notes
        // from it
        
        char playPiece = false;
        double offsetBeforePiece = 0;
        double offsetIntoPiece = 0;
        
        if( x >= mCurrentPartGridPosition &&
            x <= mCurrentPartGridPosition + bufferLengthInWorldUnits ) {
           
            playPiece = true;
            offsetBeforePiece = x - mCurrentPartGridPosition;

            // convert from world units into seconds
            offsetBeforePiece =
                ( offsetBeforePiece / mGridSpaceWidth ) * mPartLengthInSeconds;
            }

        
        if ( x < mCurrentPartGridPosition &&
             x + mGridSpaceWidth >= mCurrentPartGridPosition ) {

            playPiece = true;
            offsetIntoPiece = mCurrentPartGridPosition - x;

            // convert from world units into seconds
            offsetIntoPiece =
                ( offsetIntoPiece / mGridSpaceWidth ) * mPartLengthInSeconds;
            }
        

        if( playPiece ) {

            MusicPart *part = musicParts[i];

            MusicNote **notes;
            double *noteStartOffsets;
                    
            
            int numNotes = part->getNotesStartingInInterval( 
                offsetIntoPiece, 
                bufferLengthInSeconds,
                &notes,
                &noteStartOffsets );
                    
            
            // render each note and add it to our list of active notes
            for( int j=0; j<numNotes; j++ ) {

                double totalNoteOffset = offsetBeforePiece +
                    noteStartOffsets[j];

                unsigned long numSilentSamples =
                    (unsigned long)( totalNoteOffset * mSampleRate );
                
                unsigned long numNoteSamples;
                float *noteSamples =
                    mWaveTable->mapNoteToSamples( notes[j],
                                                  &numNoteSamples );

                
                unsigned long totalSamples = numSilentSamples + numNoteSamples;

                
                SoundSamples *samplesObject = new SoundSamples( totalSamples );

                float *leftChannel = samplesObject->mLeftChannel;
                float *rightChannel = samplesObject->mRightChannel;


                // compute stereo panning position

                /*
                 * Notes by Phil Burk (creator of portaudio):
                 *
                 * If you want to keep the power constant, then (L^2 + R^2)
                 * should be constant.  One way to do that is to use sine and
                 * cosine curves for left and right because
                 * (sin^2 + cos^2) = 1.
                 *
                 * pan = 0.0 to  PI/2
                 * LeftGain(pan) = cos(pan)
                 * RightGain(pan) = sin(pan)
                 */

                // pan smoothly between
                // -( mWorldHeight / 4 ) and +( mWorldHeight / 4 )
                // beyond this range, have constant right or left

                double panPosition;
                
                if( y >= -( mWorldHeight / 4 ) && y <= ( mWorldHeight / 4 ) ) {

                    // convert y position into a pan position in the range
                    // 0 to pi/2
                    
                    panPosition = y / ( mWorldHeight / 4 );

                    panPosition = ( panPosition + 1 ) / 2;
                    panPosition *= M_PI / 2;
                    }
                else if( y < -( mWorldHeight / 4 ) ) {
                    // hard left
                    panPosition = 0;
                    }
                else {
                    // hard right
                    panPosition = M_PI / 2;
                    }

                float leftGain = (float)( cos( panPosition ) );
                float rightGain = (float)( sin( panPosition ) );


                // samplesObject starts out with all 0 samples
                // just fill in the note samples beyond the starting silent
                // region
                char reversed = notes[j]->mReversed;
                
                for( unsigned long k=0; k<numNoteSamples; k++ ) {
                    unsigned long sampleIndex;
                    if( reversed ) {
                        sampleIndex =
                            ( numNoteSamples - k - 1 ) + numSilentSamples;
                        }
                    else {
                        sampleIndex = k + numSilentSamples;
                        }
                    
                    leftChannel[ sampleIndex ] =
                        leftGain * noteSamples[k];
                    rightChannel[ sampleIndex ] =
                        rightGain * noteSamples[k];
                    }

                mActiveNotes->push_back( samplesObject );
                mNotePositions->push_back( 0 );
                
                delete notes[j];
                }

            delete [] notes;
            delete [] noteStartOffsets;
            }


        delete positions[i];
        }

    delete [] positions;
    delete [] musicParts;


    // now we have added all notes that start playing sometime during
    // this buffer


    // next mix the samples from active notes that play during this buffer

    SoundSamples *returnSamples = new SoundSamples( inNumSamples );
    float *returnLeftChannel = returnSamples->mLeftChannel;
    float *returnRightChannel = returnSamples->mRightChannel;
    
    for( i=0; i<mActiveNotes->size(); i++ ) {

        SoundSamples *noteSamples = *( mActiveNotes->getElement( i ) );
        unsigned long notePosition = *( mNotePositions->getElement( i ) );

        unsigned long numNoteSamples = noteSamples->mSampleCount;
        
        unsigned long numSamplesToPlay = inNumSamples;

        char noteFinished = false;
        
        if( numSamplesToPlay + notePosition > numNoteSamples ) {
            // buffer goes beyond the end of this note

            numSamplesToPlay = numNoteSamples - notePosition;
            
            noteFinished = true;
            }
        
        float *noteLeftChannel = noteSamples->mLeftChannel;
        float *noteRightChannel = noteSamples->mRightChannel;
                
        for( unsigned long j=0; j<numSamplesToPlay; j++ ) {
            unsigned long noteIndex = j + notePosition;
            
            returnLeftChannel[j] += noteLeftChannel[ noteIndex ];
            returnRightChannel[j] += noteRightChannel[ noteIndex ];

            }

        notePosition += numSamplesToPlay;

        *( mNotePositions->getElement( i ) ) = notePosition;

        
        if( noteFinished ) {
            delete noteSamples;
            mActiveNotes->deleteElement( i );
            mNotePositions->deleteElement( i );

            // back up in for loop to reflect this note dropping out
            i--;
            }
        }
    
    

    // advance the grid position
    mCurrentPartGridPosition += bufferLengthInWorldUnits;

    
    return returnSamples;
    }



double MusicPlayer::getCurrentPartGridPosition() {
    return mCurrentPartGridPosition;
    }







/*
 * Modification History
 *
 * 2004-August-22   Jason Rohrer
 * Created.
 *
 * 2004-August-26   Jason Rohrer
 * Added parameter to control character of part.
 */



#include "MusicPart.h"
#include "LevelDirectoryManager.h"

#include <math.h>



MusicPart::MusicPart( MusicNoteWaveTable *inWaveTable,
                      RandomSource *inRandSource,
                      double inParameter )
    : mWaveTable( inWaveTable ),
      mNotes( new SimpleVector<MusicNote *>() ) {

    int frequencyCount = inWaveTable->getFrequencyCount();
    int lengthCount = inWaveTable->getLengthCount();

    char error= false;
    
    double musicChanceOfReversedNote =
        LevelDirectoryManager::readDoubleFileContents(
            "musicChanceOfReversedNote", &error, true );

    if( error ) {
        // default to all notes playing forward
        musicChanceOfReversedNote = 0.0;
        }

    error = false;
    
    double musicPartLength =
        LevelDirectoryManager::readDoubleFileContents(
            "musicPartLength", &error, true );

    if( error ) {
        // default to 10 second parts
        musicPartLength = 10.0;
        }

    // populate our note vector with randomized notes, all of the same length

    // decide note length using our parameter
    int lengthIndex = (int)( rint( inParameter * ( lengthCount - 1 ) ) );

    double totalLength = 0;

    while( totalLength < musicPartLength ) {

        // add another note

        int frequencyIndex =
            inRandSource->getRandomBoundedInt( 0, frequencyCount - 1 );        

        char noteReversed;
        
        // flip a weighted coin to determine if this note should be played
        // in reverse
        if( inRandSource->getRandomDouble() < musicChanceOfReversedNote ) {
            noteReversed = true;
            }
        else {            
            noteReversed = false;
            }

        mNotes->push_back(
            new MusicNote( frequencyIndex, lengthIndex, noteReversed ) );

        // add this note's length to our total
        totalLength += inWaveTable->getLengthInSeconds( lengthIndex );
        }


    // note lengths sum to a total length that may be beyond the limit

    if( totalLength > musicPartLength ) {
        // drop the last note

        int lastNoteIndex = mNotes->size() - 1;
        
        delete *( mNotes->getElement( lastNoteIndex ) );
        mNotes->deleteElement( lastNoteIndex );


        // could do something more intelligent here...
        // like drop the note that results in a total length that
        // is closest to the musicPartLength        
        }
    

    }



MusicPart::~MusicPart() {

    int numNotes = mNotes->size();

    for( int i=0; i<numNotes; i++ ) {
        delete *( mNotes->getElement( i ) );
        }
    delete mNotes;
    }



double MusicPart::getPartLengthInSeconds() {

    return mPartLengthInSeconds;
    }



int MusicPart::getNotesStartingInInterval( 
    double inStartTimeInSeconds, 
    double inLengthInSeconds,
    MusicNote ***outNotes,
    double **outNoteStartOffsetsInSeconds ) {


    SimpleVector<MusicNote*> *returnNotes = new SimpleVector<MusicNote*>();
    SimpleVector<double> *returnStartOffsets = new SimpleVector<double>();
    

    double endTimeInSeconds = inStartTimeInSeconds + inLengthInSeconds;

    // walk through notes looking for those that start in the interval
    int numNotes = mNotes->size();
    double currentNoteStartTime = 0;
    
    for( int i=0;
         i<numNotes && currentNoteStartTime <= endTimeInSeconds;
         i++ ) {


        MusicNote *note = *( mNotes->getElement( i ) );
        
        double noteLength =
            mWaveTable->getLengthInSeconds( note->mLengthIndex );
        
        if( currentNoteStartTime >= inStartTimeInSeconds ) {
            // add the note

            returnNotes->push_back( note->copy() );

            returnStartOffsets->push_back(
                currentNoteStartTime - inStartTimeInSeconds );
            }
        // else skip the note


        currentNoteStartTime += noteLength;
        }
    
    
    int numNotesReturned = returnNotes->size();
    
    *outNotes = returnNotes->getElementArray();
    *outNoteStartOffsetsInSeconds = returnStartOffsets->getElementArray();

    
    delete returnNotes;
    delete returnStartOffsets;
    
    return numNotesReturned;
    }







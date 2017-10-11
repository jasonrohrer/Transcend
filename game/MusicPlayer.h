/*
 * Modification History
 *
 * 2004-August-22   Jason Rohrer
 * Created.
 */



#ifndef MUSIC_PLAYER_INCLUDED
#define MUSIC_PLAYER_INCLUDED



#include "SoundSamples.h"
#include "SculptureManager.h"
#include "MusicNoteWaveTable.h"

#include "minorGems/util/SimpleVector.h"



/**
 * Class that plays music notes
 *
 * @author Jason Rohrer
 */
class MusicPlayer {


    public:


        
        /**
         * Constructs a player.
         * Reads configuration using the LevelDirectoryManager.
         *
         * @param inSamplesPerSecond the sample rate.
         * @param inSculptureManager the sculpture manager to
         *   get music notes from.
         *   Must be destroyed by caller after this class is destroyed.
         * @param inWaveTable the wave table to use when rendering notes.
         *   Must be destroyed by caller after this class is destroyed.
         * @param inWorldWidth the width of the world.
         * @param inWorldWidth the height of the world.
         * @param inGridSpaceWidth the width of each grid space in the world.
         */
        MusicPlayer( unsigned long inSamplesPerSecond,
                     SculptureManager *inSculptureManager,
                     MusicNoteWaveTable *inWaveTable,
                     double inWorldWidth,
                     double inWorldHeight,
                     double inGridSpaceWidth );


        
        ~MusicPlayer();


        
        /**
         * Gets more samples of music from this player.
         *
         * @param inNumSamples the number of samples to get.
         *
         * @return a buffer of samples.  Must be destroyed by caller.
         */
        SoundSamples *getMoreMusic( unsigned long inNumSamples );

        

        /**
         * Gets the grid position in the world that is currently being
         * played.
         *
         * @return the grid position in world units.
         */
        double getCurrentPartGridPosition();

        
        
    protected:

        SculptureManager *mSculptureManager;
        MusicNoteWaveTable *mWaveTable;
        
        SimpleVector<SoundSamples*> *mActiveNotes;
        SimpleVector<unsigned long> *mNotePositions;
        

        
        double mPartLengthInSeconds;

        double mCurrentPartGridPosition;

        double mSampleRate;


        double mWorldWidth;
        double mWorldHeight;
        double mGridSpaceWidth;
        
    };



#endif






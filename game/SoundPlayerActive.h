/*
 * Modification History
 *
 * 2004-July-17   Jason Rohrer
 * Created.
 *
 * 2004-July-21   Jason Rohrer
 * Switched to a passive (callback-based) player.  Old player saved here.
 */



#ifndef SOUND_PLAYER_INCLUDED
#define SOUND_PLAYER_INCLUDED


#include "SoundSamples.h"

#include "Transcend/portaudio/pa_common/portaudio.h"
#include "Transcend/portaudio/pablio/pablio.h"


#include "minorGems/util/SimpleVector.h"



/**
 * Class that plays both running background music and realtime sounds.
 *
 * @author Jason Rohrer
 */
class SoundPlayer {


        
    public:


        
        /**
         * Constructs a sound player.
         */
        SoundPlayer();

        ~SoundPlayer();

        

        /**
         * Mixes a sound to the speakers as quickly as possible.
         *
         * This call does not adjust the volume level of the samples
         * before mixing them with other realtime sounds or the background
         * music.
         *
         * @param inSamples the samples to play.
         *   Must be destroyed by caller.
         */
        void playSoundNow( SoundSamples *inSamples );


        
        /**
         * Gets the number of music samples that could be added to
         * this player without blocking.
         *
         * @return the number of samples.
         */
        unsigned long getNumMusicSamplesNeeded();

        
        
        /**
         * Add the next section of music to be played.
         *
         * This call drives the sound player to send audio to the speakers.
         *
         * @param inSamples the samples to play.
         *   Must be destroyed by caller.
         */
        void addMoreMusic( SoundSamples *inSamples );

        
        
    protected:

        char mAudioInitialized;
        
        
        PABLIO_Stream *mAudioStream;

        // realtime sounds that should be mixed into the next to-speaker call
        SimpleVector<SoundSamples *> *mRealtimeSounds;

        
        
    };



#endif

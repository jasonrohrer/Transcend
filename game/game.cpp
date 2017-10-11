/*
 * Modification History
 *
 * 2004-June-10   Jason Rohrer
 * Created.
 *
 * 2004-June-11   Jason Rohrer
 * Added zoom that adjusts with speed.  Disabled depth testing.
 *
 * 2004-June-12   Jason Rohrer
 * Changed to test control points.
 *
 * 2004-June-14   Jason Rohrer
 * Added call to glutInit to follow GLUT spec (particularly on Mac OS X).
 *
 * 2004-June-15   Jason Rohrer
 * Changed so that ship movement/rotation rates don't vary with framerate.
 *
 * 2004-June-18   Jason Rohrer
 * Disabled wrapping to increase framerate.
 * Made accellerations framerate-independent.
 *
 * 2004-June-20   Jason Rohrer
 * Added support for enemy/explosion scaling.
 *
 * 2004-June-21   Jason Rohrer
 * Added support for ship being impacted by enemy bullets.
 * Added umbilical cord.  Changed order of grid drawing.
 * Removed old code and random object code.
 * Fixed a memory leak.
 * Added support for changing working directory on Mac platform.
 *
 * 2004-June-29   Jason Rohrer
 * Added a pause function.
 *
 * 2004-July-3   Jason Rohrer
 * Zoomed out farther.  Improved ship bullet physics.
 * Added ship rotation accelleration.
 *
 * 2004-July-5   Jason Rohrer
 * Fixed memory leaks.
 *
 * 2004-August-6   Jason Rohrer
 * Added enemy shot sounds.
 *
 * 2004-August-9   Jason Rohrer
 * Added test of parameterized sound space.
 *
 * 2004-August-17   Jason Rohrer
 * Fixed a bug that caused ship to bounce back and forth when jarred to center.
 *
 * 2004-August-19   Jason Rohrer
 * Fixed bug in way sculpture piece moves with ship near boundaries.
 *
 * 2004-August-22   Jason Rohrer
 * Added support for music.
 *
 * 2004-August-22   Jason Rohrer
 * Added support for music.
 *
 * 2004-August-23   Jason Rohrer
 * Added current music position cursor.
 *
 * 2004-August-24   Jason Rohrer
 * Added color to music cursor.
 * Inverted music cursor.
 * Fixed a memory leak.
 *
 * 2004-August-25   Jason Rohrer
 * Changed so that sculpture piece parameter controls an aspect of MusicPart.
 * Parameterized grid and background colors.
 * Added support for loading levels and fading between them.
 *
 * 2004-August-26   Jason Rohrer
 * Fixed bug in setting mac working directory.
 *
 * 2004-August-30   Jason Rohrer
 * Added bullet scaling parameters.
 * Fixed a bug when boss bullets hit ship while we are carrying a piece.
 * Added code to set music loudness based on number of sculpture pieces.
 *
 * 2004-August-31   Jason Rohrer
 * Added support for specifying level number on command line.
 * Improved draw order.
 * Added code for reading reverb config and umbilical colors from file.
 * Fixed memory leaks.
 * Improved pause behavior.
 *
 * 2004-September-3   Jason Rohrer
 * Improved grid coloring.
 *
 * 2004-October-13   Jason Rohrer
 * Added zoom in/out during fade in/out.
 * Added code to eliminate large position jumps during slow frames.
 * Parameterized grid size per level.
 * Added strafe keys, and switched pick-up/drop key.
 * Added limit for number of ship bullets on screen. 
 *
 * 2004-October-14   Jason Rohrer
 * Got portals working.
 * Fixed key release bug that caused stuck keys.
 * Fixed zoom effects and added a zoom-in effect at level end.
 * Switched back to zoom-out effect at level end.
 * Removed an error print message.
 * Added a cheat for judges to skip to next level.
 *
 * 2005-August-21   Jason Rohrer
 * Fixed zoom behavior for strafing.
 *
 * 2005-August-22   Jason Rohrer
 * Doubled pick-up radius to make piece pick up easier.
 * Added magnet mode to smooth piece pick-up and drop.
 * Started work on boss damage graphics.
 *
 * 2005-August-28   Jason Rohrer
 * Fixed time error message.
 *
 * 2005-August-29   Jason Rohrer
 * Disabled the skip-level cheat.
 */


#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <unistd.h>
#include <time.h>


#include "minorGems/graphics/openGL/ScreenGL.h"
#include "minorGems/graphics/openGL/SceneHandlerGL.h"

#include "minorGems/util/random/StdRandomSource.h"
#include "minorGems/system/Time.h"
#include "minorGems/system/Thread.h"
#include "minorGems/io/file/File.h"


#include "DrawableObject.h"
#include "ObjectParameterSpaceControlPoint.h"
#include "LevelDirectoryManager.h"
#include "ParameterizedObject.h"
#include "ShipBullet.h"
#include "BulletSound.h"
#include "ShipBulletManager.h"
#include "EnemyManager.h"
#include "SculptureManager.h"
#include "BossManager.h"
#include "PortalManager.h"
#include "SoundPlayer.h"
#include "ReverbSoundFilter.h"
#include "ParameterizedStereoSound.h"
#include "MusicPart.h"
#include "MusicNoteWaveTable.h"
#include "MusicPlayer.h"


class GameSceneHandler :
    public SceneHandlerGL, public KeyboardHandlerGL,
    public RedrawListenerGL { 
	
	public:

        /**
         * Constructs a sceen handler.
         *
         * @param inStartingLevel the level to start on.
         *   Defaults to 0.
         */
        GameSceneHandler( int inStartingLevel = 1 );

        virtual ~GameSceneHandler();
        

        ScreenGL *mScreen;

        
        
		// implements the SceneHandlerGL interface
		virtual void drawScene();

        // implements the KeyboardHandlerGL interface
		virtual void keyPressed( unsigned char inKey, int inX, int inY );
		virtual void specialKeyPressed( int inKey, int inX, int inY );
		virtual void keyReleased( unsigned char inKey, int inX, int inY );
		virtual void specialKeyReleased( int inKey, int inX, int inY );

        // implements the RedrawListener interface
		virtual void fireRedraw();



        /**
         * Loads the next level.
         */
        void loadNextLevel();


        /**
         * Destroys the currently loaded level.
         */
        void destroyLevel();

        
        
    protected:

        int mLevelNumber;
        
        double mFadeLevel;
        double mFadeTime;
        
        // true if ship is in portal to move on to next level
        char mShipInPortal;
        
        double mMaxXPosition;
        double mMinXPosition;
        double mMaxYPosition;
        double mMinYPosition;

        double mGridSpacing;
        
        // the time that the last frame was drawn
        unsigned long mLastFrameSeconds;
        unsigned long mLastFrameMilliseconds;

        // our current frame rate
        unsigned long mFrameMillisecondDelta;



        
        // tracking our current state of movement
        double mForwardBackwardMoveRate;
        double mRightLeftMoveRate;
        double mMaxMoveRate;

        double mShipAccelleration;
        double mShipFriction;
        
        double mRotationRate;
        double mBaseRotationRate;
        double mMaxRotationRate;
        double mShipRotationAccelleration;
        
        double mShipScale;

        double mCurrentShipRadius;
        
        char mMovingUp;
        char mMovingDown;
        char mMovingLeft;
        char mMovingRight;

        char mZoomingIn;
        char mZoomingOut;

        char mRotatingClockwise;
        char mRotatingCounterClockwise;

        Vector3D *mCurrentShipVelocityVector;
        
        char mPaused;
        
        
        StdRandomSource *mRandSource;

        ParameterizedObject *mShipParameterSpace;
        
        ShipBulletManager *mShipBulletManager;
        double mShipBulletRange;
        double mShipBulletBaseVelocity;
        int mMaxNumShipBullets;
        
        ShipBulletManager *mEnemyBulletManager;
        double mEnemyBulletRange;
        double mEnemyBulletBaseVelocity;
        double mEnemyBulletsPerSecond;
        double mEnemyBulletShipJarPower;
        double mEnemyBulletSculptureJarPower;
        double mBossBulletShipJarPower;
        double mBossBulletSculptureJarPower;
        double mSculptureFriction;
        double mCurrentShipJarForce;

        ShipBulletManager *mBossBulletManager;
        ShipBulletManager *mBossDamageManager;
        
        EnemyManager *mEnemyManager;
        int mNumEnemies;
        
        SculptureManager *mSculptureManager;

        BossManager *mBossManager;
        PortalManager *mPortalManager;

        MusicNoteWaveTable *mWaveTable;
        MusicPlayer *mMusicPlayer;
        
        int mCurrentPieceCarried;
        double mPiecePickupRadius;

        double mMaxFrameRate;

        char mPrintFrameRate;
        unsigned long mNumFrames;
        unsigned long mFrameBatchSize;
        unsigned long mFrameBatchStartTimeSeconds;
        unsigned long mFrameBatchStartTimeMilliseconds;


        Color *mBackgroundColor;
        Color *mNearBossGridColor;
        Color *mFarBossGridColor;

        Color *mWeakUmbilicalColor;
        Color *mStrongUmbilicalColor;
        
        int mSampleRate;

        double mMusicLoudness;
        int mMaxSimultaneousSounds;
        SoundPlayer *mSoundPlayer;
        
        void addRandomEnemy();
        
	};



GameSceneHandler *sceneHandler;
ScreenGL *screen;

//double baseViewZ = -30;
double baseViewZ = -50;


// function that destroys object when exit is called.
// exit is the only way to stop the GLUT-based ScreenGL
void cleanUpAtExit() {
    printf( "exiting\n" );

    delete sceneHandler;
    delete screen;
    }




int main( int inNumArgs, char **inArgs ) {


    int startingLevel = 1;
    if( inNumArgs > 1 ) {
        int numRead = sscanf( inArgs[1], "%d", &startingLevel );

        if( numRead == 1 ) {
            if( startingLevel < 1 ) {
                startingLevel = 1;
                }
            }
        }
        
    
    sceneHandler = new GameSceneHandler( startingLevel );

    // must pass args to GLUT before constructing the screen
    glutInit( &inNumArgs, inArgs );

    screen =
        new ScreenGL( 300, 300, false, 
                      "Transcend", sceneHandler, NULL, sceneHandler );

    sceneHandler->mScreen = screen;

    screen->addRedrawListener( sceneHandler );
    
    Vector3D *move = new Vector3D( 0, 0, baseViewZ );
    screen->moveView( move );
    delete move;


    // do this mac check after constructing scene handler and screen,
    // since these cause various Mac frameworks to be loaded (which can
    // change the current working directory out from under us)
    #ifdef __mac__
        // make sure working directory is the same as the directory
        // that the app resides in
        // this is especially important on the mac platform, which
        // doesn't set a proper working directory for double-clicked
        // app bundles

        // arg 0 is the path to the app executable
        char *appDirectoryPath = stringDuplicate( inArgs[0] );
    
        char *appNamePointer = strstr( appDirectoryPath,
                                       "Transcend.app" );
        
        // terminate full app path to get parent directory
        appNamePointer[0] = '\0';

        chdir( appDirectoryPath );

        delete [] appDirectoryPath;
    #endif



    
    sceneHandler->loadNextLevel();

    
    // register cleanup function, since screen->start() will never return
    atexit( cleanUpAtExit );


    screen->start();

    
    return 0;
    }






GameSceneHandler::GameSceneHandler( int inStartingLevel )
    : mLevelNumber( inStartingLevel - 1 ),
      mFadeLevel( 0 ),   // start fully faded out,
      mFadeTime( 3 ),   // 3 seconds
      mShipInPortal( false ),
      mMaxXPosition( 100 ), mMinXPosition( -100 ),
      mMaxYPosition( 100 ), mMinYPosition( -100 ),
      mGridSpacing( 10 ),
      mFrameMillisecondDelta( 0 ),
      mForwardBackwardMoveRate( 0 ),
      mRightLeftMoveRate( 0 ),
      mRotationRate( 0 ),
      mMovingUp( false ), mMovingDown( false ), mMovingLeft( false ),
      mMovingRight( false ), mZoomingIn( false ), mZoomingOut( false ),
      mRotatingClockwise( false ), mRotatingCounterClockwise( false ),
      mPaused( false ),
      mRandSource( new StdRandomSource() ),
      mMaxFrameRate( 400 ),  // don't limit frame rate
      mPrintFrameRate( false ),
      mNumFrames( 0 ), mFrameBatchSize( 100 ),
      mFrameBatchStartTimeSeconds( time( NULL ) ),
      mFrameBatchStartTimeMilliseconds( 0 ),
      mMusicLoudness( 0.1 ),
      mMaxSimultaneousSounds( 2 ) {


    Time::getCurrentTime( &mLastFrameSeconds, &mLastFrameMilliseconds );
    
    mSampleRate = 11025;
    
    mSoundPlayer = new SoundPlayer( mSampleRate,
                                    mMaxSimultaneousSounds,
                                    NULL, mMusicLoudness );

    }



void GameSceneHandler::loadNextLevel() {

    
    mCurrentShipVelocityVector = new Vector3D( 0, 0, 0 );

    // reset ship position and angle
    Vector3D *viewPosition = new Vector3D( 0, 0, baseViewZ );
    mScreen->setViewPosition( viewPosition );
    delete viewPosition;

    // reset view orientation back to the zero vector
    Angle3D *viewOrientation = mScreen->getViewOrientation();
    Angle3D *viewRotation = new Angle3D( viewOrientation );
    viewRotation->scale( -1 );

    mScreen->rotateView( viewRotation );
    delete viewRotation;
    
    int i;


    
    mLevelNumber++;
    
    char *levelString = new char[4];
    
    // zero pad
    sprintf( levelString, "%03d", mLevelNumber );

    
    
    File *levelsDirectory = new File( NULL, "levels" );
    File *currentLevelDirectory = levelsDirectory->getChildFile( levelString );

    if( !( currentLevelDirectory->exists() ) ) {
        // we have run out of levels... back to level 1
        mLevelNumber = 1;

        delete currentLevelDirectory;

        currentLevelDirectory = levelsDirectory->getChildFile( "001" );
        }
        
    
    LevelDirectoryManager::setLevelDirectory( currentLevelDirectory->copy() );


    delete [] levelString;



    // read grid size for level
    char error = false;

    int xGridSize = LevelDirectoryManager::readIntFileContents( "gridSizeX",
                                                                &error,
                                                                true );
    if( error ) {
        xGridSize = 200;
        }
    error = false;

    int yGridSize = LevelDirectoryManager::readIntFileContents( "gridSizeY",
                                                                &error,
                                                                true );

    if( error ) {
        yGridSize = 200;
        }
    error = false;


    mMinXPosition = - xGridSize / 2;
    mMinYPosition = - yGridSize / 2;

    mMaxXPosition = xGridSize / 2;
    mMaxYPosition = yGridSize / 2;

    
    

    // clear existing filters
    mSoundPlayer->removeAllFilters();
    
    // load information about reverb filters
    FILE *reverbFILE = LevelDirectoryManager::getStdStream( "reverbFilters",
                                                            true );

    while( !error ) {
        double time;
        double loudness;

        error = true;
        int numRead = fscanf( reverbFILE, "%lf", &time );

        if( numRead == 1 ) {
            numRead = fscanf( reverbFILE, "%lf", &loudness );

            if( numRead == 1 ) {
                error = false;

                mSoundPlayer->addFilter(
                    new ReverbSoundFilter(
                        (unsigned long)( mSampleRate * time ), loudness ) );
                }
            }
        }
    


    
    FILE *backgroundColorFILE =
        LevelDirectoryManager::getStdStream( "backgroundColor", true );
    
    if( backgroundColorFILE != NULL ) {
        mBackgroundColor =
            ObjectParameterSpaceControlPoint::readColorFromFile(
                backgroundColorFILE );
        fclose( backgroundColorFILE );
        }
    else {
        mBackgroundColor = new Color( 0, 0, 0, 1 );
        }

    
    
    FILE *nearBossGridColorFILE =
        LevelDirectoryManager::getStdStream( "nearBossGridColor", true );
    
    if( nearBossGridColorFILE != NULL ) {
        mNearBossGridColor =
            ObjectParameterSpaceControlPoint::readColorFromFile(
                nearBossGridColorFILE );
        fclose( nearBossGridColorFILE );
        }
    else {
        mNearBossGridColor = new Color( 1, 0, 0, 0.5 );
        }

    FILE *farBossGridColorFILE =
        LevelDirectoryManager::getStdStream( "farBossGridColor", true );
    
    if( farBossGridColorFILE != NULL ) {
        mFarBossGridColor =
            ObjectParameterSpaceControlPoint::readColorFromFile(
                farBossGridColorFILE );
        fclose( farBossGridColorFILE );
        }
    else {
        mFarBossGridColor = new Color( 0, 0, 1, 0.5 );
        }



    FILE *weakUmbilicalColorFILE =
        LevelDirectoryManager::getStdStream( "weakUmbilicalColor", true );
    
    if( weakUmbilicalColorFILE != NULL ) {
        mWeakUmbilicalColor =
            ObjectParameterSpaceControlPoint::readColorFromFile(
                weakUmbilicalColorFILE );
        fclose( weakUmbilicalColorFILE );
        }
    else {
        mWeakUmbilicalColor = new Color( 1, 0, 0, 1 );
        }


    FILE *strongUmbilicalColorFILE =
        LevelDirectoryManager::getStdStream( "strongUmbilicalColor", true );
    
    if( strongUmbilicalColorFILE != NULL ) {
        mStrongUmbilicalColor =
            ObjectParameterSpaceControlPoint::readColorFromFile(
                strongUmbilicalColorFILE );
        fclose( strongUmbilicalColorFILE );
        }
    else {
        mStrongUmbilicalColor = new Color( 1, 1, 0, 1 );
        }



    
    mWaveTable = new MusicNoteWaveTable( mSampleRate );

    
    
    FILE *shipFILE = LevelDirectoryManager::getStdStream( "ship", true );
    
    mShipParameterSpace = new ParameterizedObject( shipFILE,
                                                   &error );
    if( error ) {
        printf( "Error reading control points from ship file\n" );
        }

    if( shipFILE != NULL ) {
        fclose( shipFILE );
        }

    
    FILE *shipBulletFILE = LevelDirectoryManager::getStdStream( "shipBullet",
                                                                true );
    
    error = false;
    ShipBullet *shipBulletTemplate = new ShipBullet( shipBulletFILE, &error );

    if( error ) {
        printf( "Error reading from shipBullet file\n" );
        }
    
    if( shipBulletFILE != NULL ) {
        fclose( shipBulletFILE );
        }


    error = false;
    double shipBulletScale =
        LevelDirectoryManager::readDoubleFileContents( "shipBulletScale",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        shipBulletScale = 1;
        }

    

    FILE *shipBulletSoundFILE =
        LevelDirectoryManager::getStdStream( "shipBulletSound", true );
    
    error = false;
    BulletSound *shipBulletSoundTemplate =
        new BulletSound( shipBulletSoundFILE, &error );

    if( error ) {
        printf( "Error reading from shipBulletSound file\n" );
        }
    
    if( shipBulletSoundFILE != NULL ) {
        fclose( shipBulletSoundFILE );
        }
    
    mShipBulletManager =
        new ShipBulletManager( shipBulletTemplate,
                               shipBulletScale,
                               mSoundPlayer,
                               shipBulletSoundTemplate,
                               mMaxXPosition - mMinXPosition,
                               mMaxYPosition - mMinYPosition );


    error = false;
    mShipBulletRange =
        LevelDirectoryManager::readDoubleFileContents( "shipBulletRange",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        mShipBulletRange = 30;
        }


    error = false;
    mShipBulletBaseVelocity =
        LevelDirectoryManager::readDoubleFileContents(
            "shipBulletBaseVelocity",
            &error,
            true );
    
    if( error ) {
        // default
        mShipBulletBaseVelocity = 20;
        }


    error = false;
    mMaxNumShipBullets =
        LevelDirectoryManager::readIntFileContents(
            "maxShipBulletsOnScreen",
            &error,
            true );
    
    if( error ) {
        // default
        mMaxNumShipBullets = 3;
        }
    

    FILE *enemyBulletFILE = LevelDirectoryManager::getStdStream( "enemyBullet",
                                                                 true );
    
    error = false;
    ShipBullet *enemyBulletTemplate =
        new ShipBullet( enemyBulletFILE, &error );

    if( error ) {
        printf( "Error reading from enemyBullet file\n" );
        }
    
    if( enemyBulletFILE != NULL ) {
        fclose( enemyBulletFILE );
        }


    error = false;
    double enemyBulletScale =
        LevelDirectoryManager::readDoubleFileContents( "enemyBulletScale",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        enemyBulletScale = 1;
        }
    

    FILE *enemyBulletSoundFILE =
        LevelDirectoryManager::getStdStream( "enemyBulletSound", true );
    
    error = false;
    BulletSound *enemyBulletSoundTemplate =
        new BulletSound( enemyBulletSoundFILE, &error );

    if( error ) {
        printf( "Error reading from enemyBulletSound file\n" );
        }
    
    if( enemyBulletSoundFILE != NULL ) {
        fclose( enemyBulletSoundFILE );
        }




    
    mEnemyBulletManager =
        new ShipBulletManager( enemyBulletTemplate,
                               enemyBulletScale,
                               mSoundPlayer,
                               enemyBulletSoundTemplate,
                               mMaxXPosition - mMinXPosition,
                               mMaxYPosition - mMinYPosition );


    error = false;
    mEnemyBulletShipJarPower =
        LevelDirectoryManager::readDoubleFileContents(
            "enemyBulletShipJarPower",
            &error,
            true );
    
    if( error ) {
        // default
        mEnemyBulletShipJarPower = .5;
        }

    error = false;
    mEnemyBulletSculptureJarPower =
        LevelDirectoryManager::readDoubleFileContents(
            "enemyBulletSculptureJarPower",
            &error,
            true );
    
    if( error ) {
        // default
        mEnemyBulletSculptureJarPower = .5;
        }


    
    error = false;
    mMaxMoveRate =
        LevelDirectoryManager::readDoubleFileContents( "shipMaxVelocity",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        mMaxMoveRate = 40;
        }


    error = false;
    mShipAccelleration =
        LevelDirectoryManager::readDoubleFileContents( "shipAccelleration",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        mShipAccelleration = 10;
        }


    
    error = false;
    mShipFriction =
        LevelDirectoryManager::readDoubleFileContents( "shipFriction",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        mShipFriction = 10;
        }

    
    
    error = false;
    mBaseRotationRate =
        LevelDirectoryManager::readDoubleFileContents( "shipBaseRotationRate",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        mBaseRotationRate = 1.6;
        }


    error = false;
    mMaxRotationRate =
        LevelDirectoryManager::readDoubleFileContents( "shipMaxRotationRate",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        mMaxRotationRate = 3.2;
        }


    error = false;
    mShipRotationAccelleration =
        LevelDirectoryManager::readDoubleFileContents(
            "shipRotationAccelleration",
            &error,
            true );
    if( error ) {
        // default
        mShipRotationAccelleration = 1;
        }


    error = false;
    mShipScale =
        LevelDirectoryManager::readDoubleFileContents( "shipScale",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        mShipScale = 1;
        }



    

    FILE *bossBulletFILE = LevelDirectoryManager::getStdStream( "bossBullet",
                                                                 true );
    
    error = false;
    ShipBullet *bossBulletTemplate =
        new ShipBullet( bossBulletFILE, &error );

    if( error ) {
        printf( "Error reading from bossBullet file\n" );
        }
    
    if( bossBulletFILE != NULL ) {
        fclose( bossBulletFILE );
        }

    error = false;
    double bossBulletScale =
        LevelDirectoryManager::readDoubleFileContents( "bossBulletScale",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        bossBulletScale = 1;
        }
    
    
    FILE *bossBulletSoundFILE =
        LevelDirectoryManager::getStdStream( "bossBulletSound", true );
    
    error = false;
    BulletSound *bossBulletSoundTemplate =
        new BulletSound( bossBulletSoundFILE, &error );

    if( error ) {
        printf( "Error reading from bossBulletSound file\n" );
        }
    
    if( bossBulletSoundFILE != NULL ) {
        fclose( bossBulletSoundFILE );
        }

    
    
    mBossBulletManager =
        new ShipBulletManager( bossBulletTemplate,
                               bossBulletScale,
                               mSoundPlayer,
                               bossBulletSoundTemplate,
                               mMaxXPosition - mMinXPosition,
                               mMaxYPosition - mMinYPosition );


    error = false;
    mBossBulletShipJarPower =
        LevelDirectoryManager::readDoubleFileContents(
            "bossBulletShipJarPower",
            &error,
            true );
    
    if( error ) {
        // default
        mBossBulletShipJarPower = .5;
        }

    error = false;
    mBossBulletSculptureJarPower =
        LevelDirectoryManager::readDoubleFileContents(
            "bossBulletSculptureJarPower",
            &error,
            true );
    
    if( error ) {
        // default
        mBossBulletSculptureJarPower = .5;
        }

    
    
    
    error = false;
    mSculptureFriction =
        LevelDirectoryManager::readDoubleFileContents( "sculptureFriction",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        mSculptureFriction = 1;
        }



    FILE *firstSculpturePieceFILE =
        LevelDirectoryManager::getStdStream( "firstSculpturePiece", true );
    
    error = false;
    ParameterizedObject *firstPieceSpace =
        new ParameterizedObject( firstSculpturePieceFILE,
                                 &error );
    if( error ) {
        printf(
            "Error reading control points from firstSculpturePiece file\n" );
        }

    if( firstSculpturePieceFILE != NULL ) {
        fclose( firstSculpturePieceFILE );
        }

    FILE *secondSculpturePieceFILE =
        LevelDirectoryManager::getStdStream( "secondSculpturePiece", true );
    
    error = false;
    ParameterizedObject *secondPieceSpace =
        new ParameterizedObject( secondSculpturePieceFILE,
                                 &error );
    if( error ) {
        printf(
            "Error reading control points from secondSculpturePiece file\n" );
        }

    if( secondSculpturePieceFILE != NULL ) {
        fclose( secondSculpturePieceFILE );
        }


    error = false;
    int numPieces =
        LevelDirectoryManager::readIntFileContents( "numberOfSculpturePieces",
                                                    &error,
                                                    true );
    if( error ) {
        // default
        numPieces = 10;
        }
    double *pieceParameters = new double[ numPieces ];
    Vector3D **piecePositions = new Vector3D*[ numPieces ];
    Angle3D **pieceRotations = new Angle3D*[ numPieces ];
    MusicPart **pieceMusicParts = new MusicPart*[ numPieces ];
    
    for( i=0; i<numPieces; i++ ) {
        pieceParameters[i] = mRandSource->getRandomDouble();
        
        double x, y;

        x = mRandSource->getRandomDouble();
        y = mRandSource->getRandomDouble();

        // round one param at random to push all pieces to edges of world
        int coin = mRandSource->getRandomBoundedInt( 0, 1 );
        if( coin == 0 ) {
            x = rint( x );
            }
        else {
            y = rint( y );
            }

        x = x * ( mMaxXPosition - mMinXPosition ) + mMinXPosition;
        y = y * ( mMaxYPosition - mMinYPosition ) + mMinYPosition;

        piecePositions[i] = new Vector3D( x, y, 0 );

        pieceRotations[i] = new Angle3D( 0, 0,
                                         mRandSource->getRandomDouble() *
                                         2 * M_PI );
        
        pieceMusicParts[i] =
            new MusicPart( mWaveTable, mRandSource, pieceParameters[i] );
        }


    error = false;
    double sculptureScale =
        LevelDirectoryManager::readDoubleFileContents( "sculptureScale",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        sculptureScale = 1;
        }


    error = false;
    double maxDistanceToBePartOfSculpture =
        LevelDirectoryManager::readDoubleFileContents(
            "maxSculptureSeparation",
            &error,
            true );
    
    if( error ) {
        // default
        maxDistanceToBePartOfSculpture = 10;
        }


    error = false;
    double sculptureAnimationTime =
        LevelDirectoryManager::readDoubleFileContents(
            "sculptureAnimationTime",
            &error,
            true );
    if( error ) {
        // default
        sculptureAnimationTime = 4;
        }


    
    
    
    FILE *sculpturePiecePowerupFILE =
        LevelDirectoryManager::getStdStream( "sculpturePiecePowerupSpace",
                                             true );
    
    error = false;
    mSculptureManager = new SculptureManager( firstPieceSpace,
                                              secondPieceSpace,
                                              sculptureScale,
                                              maxDistanceToBePartOfSculpture,
                                              sculptureAnimationTime,
                                              numPieces,
                                              pieceParameters,
                                              piecePositions,
                                              pieceRotations,
                                              pieceMusicParts,
                                              sculpturePiecePowerupFILE,
                                              &error,
                                              mEnemyBulletManager,
                                              mEnemyBulletSculptureJarPower,
                                              mBossBulletManager,
                                              mBossBulletSculptureJarPower,
                                              mSculptureFriction,
                                              mMaxXPosition - mMinXPosition,
                                              mMaxYPosition - mMinYPosition );

    if( error ) {
        printf( "Error reading from sculpturePiecePowerupSpace file\n" );
        }
    
    if( sculpturePiecePowerupFILE != NULL ) {
        fclose( sculpturePiecePowerupFILE );
        }

    mMusicPlayer = new MusicPlayer( mSampleRate,
                                    mSculptureManager,
                                    mWaveTable,
                                    mMaxXPosition - mMinXPosition,
                                    mMaxYPosition - mMinYPosition,
                                    mGridSpacing );

    mSoundPlayer->setMusicPlayer( mMusicPlayer );

    // avoid clipping
    mMusicLoudness = ( 1 - 0.1 * mMaxSimultaneousSounds ) / numPieces; 
    
    
    mCurrentPieceCarried = -1;

    error = false;
    mPiecePickupRadius =
        LevelDirectoryManager::readDoubleFileContents( "piecePickupRadius",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        mPiecePickupRadius = 10;
        }


    

    error = false;
    mEnemyBulletRange =
        LevelDirectoryManager::readDoubleFileContents( "enemyBulletRange",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        mEnemyBulletRange = 30;
        }


    error = false;
    mEnemyBulletBaseVelocity =
        LevelDirectoryManager::readDoubleFileContents(
            "enemyBulletBaseVelocity",
            &error,
            true );
    
    if( error ) {
        // default
        mEnemyBulletBaseVelocity = 20;
        }
    
    
    error = false;
    mEnemyBulletsPerSecond =
        LevelDirectoryManager::readDoubleFileContents(
            "enemyBulletsPerSecond",
            &error,
            true );
    
    if( error ) {
        // default
        mEnemyBulletsPerSecond = .5;
        }



    mCurrentShipJarForce = 0;


    FILE *enemyFILE = LevelDirectoryManager::getStdStream( "enemy",
                                                           true );

    error = false;
    Enemy *enemyTemplate = new Enemy( enemyFILE, &error );

    if( error ) {
        printf( "Error reading from enemy file\n" );
        }
    
    if( enemyFILE != NULL ) {
        fclose( enemyFILE );
        }

    error = false;
    double enemyScale =
        LevelDirectoryManager::readDoubleFileContents( "enemyScale",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        enemyScale = 1;
        }
    
    error = false;
    double enemyExplosionScale =
        LevelDirectoryManager::readDoubleFileContents( "enemyExplosionScale",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        enemyExplosionScale = 1;
        }



    error = false;
    double enemyVelocity =
        LevelDirectoryManager::readDoubleFileContents( "enemyVelocity",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        enemyVelocity = 3;
        }

    
    FILE *enemyExplosionSoundFILE =
        LevelDirectoryManager::getStdStream( "enemyExplosionSound", true );
    
    error = false;
    BulletSound *enemyExplosionSoundTemplate =
        new BulletSound( enemyExplosionSoundFILE, &error );

    if( error ) {
        printf( "Error reading from enemyExplosionSound file\n" );
        }
    
    if( enemyExplosionSoundFILE != NULL ) {
        fclose( enemyExplosionSoundFILE );
        }


    
    mEnemyManager =
        new EnemyManager( enemyTemplate,
                          enemyScale,
                          enemyExplosionScale,
                          enemyVelocity,
                          mSculptureManager,
                          mShipBulletManager,
                          mEnemyBulletManager,
                          mEnemyBulletRange,
                          mEnemyBulletBaseVelocity,
                          mEnemyBulletsPerSecond,
                          mSoundPlayer,
                          enemyExplosionSoundTemplate,
                          mMaxXPosition - mMinXPosition,
                          mMaxYPosition - mMinYPosition );
    

    error = false;
    mNumEnemies =
        LevelDirectoryManager::readIntFileContents( "numberOfEnemies",
                                                    &error,
                                                    true );
    if( error ) {
        // default
        mNumEnemies = 10;
        }



    
    
    FILE *bossFILE = LevelDirectoryManager::getStdStream( "boss",
                                                           true );

    error = false;
    Enemy *bossTemplate = new Enemy( bossFILE, &error );

    if( error ) {
        printf( "Error reading from boss file\n" );
        }

    if( bossFILE != NULL ) {
        fclose( bossFILE );
        }


    error = false;
    double bossScale =
        LevelDirectoryManager::readDoubleFileContents( "bossScale",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        bossScale = 3;
        }
    
    error = false;
    double bossExplosionScale =
        LevelDirectoryManager::readDoubleFileContents( "bossExplosionScale",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        bossExplosionScale = 3;
        }



    error = false;
    double bossMinVelocity =
        LevelDirectoryManager::readDoubleFileContents( "bossMinVelocity",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        bossMinVelocity = 3;
        }

    error = false;
    double bossMaxVelocity =
        LevelDirectoryManager::readDoubleFileContents( "bossMaxVelocity",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        bossMaxVelocity = 30;
        }


    error = false;
    double bossBulletRange =
        LevelDirectoryManager::readDoubleFileContents( "bossBulletRange",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        bossBulletRange = 30;
        }


    error = false;
    double bossBulletBaseVelocity =
        LevelDirectoryManager::readDoubleFileContents(
            "bossBulletBaseVelocity",
            &error,
            true );
    
    if( error ) {
        // default
        bossBulletBaseVelocity = 20;
        }
    
    
    error = false;
    double bossMinBulletsPerSecond =
        LevelDirectoryManager::readDoubleFileContents(
            "bossMinBulletsPerSecond",
            &error,
            true );
    
    if( error ) {
        // default
        bossMinBulletsPerSecond = .5;
        }


    error = false;
    double bossMaxBulletsPerSecond =
        LevelDirectoryManager::readDoubleFileContents(
            "bossMaxBulletsPerSecond",
            &error,
            true );
    
    if( error ) {
        // default
        bossMaxBulletsPerSecond = .5;
        }


    
    error = false;
    double bossTimeToGetAngry =
        LevelDirectoryManager::readDoubleFileContents(
            "bossTimeToGetAngry",
            &error,
            true );
    
    if( error ) {
        // default
        bossTimeToGetAngry = 5;
        }


    error = false;
    double bossMaxHealth =
        LevelDirectoryManager::readDoubleFileContents(
            "bossMaxHealth",
            &error,
            true );
    
    if( error ) {
        // default
        bossMaxHealth = 400;
        }

    error = false;
    double bossRecoveryRate =
        LevelDirectoryManager::readDoubleFileContents(
            "bossRecoveryRate",
            &error,
            true );
    
    if( error ) {
        // default
        bossRecoveryRate = 40;
        }

    error = false;
    double bossExplosionTime =
        LevelDirectoryManager::readDoubleFileContents(
            "bossExplosionTime",
            &error,
            true );
    
    if( error ) {
        // default
        bossExplosionTime = 2;
        }


    
    FILE *bossExplosionSoundFILE =
        LevelDirectoryManager::getStdStream( "bossExplosionSound", true );
    
    error = false;
    BulletSound *bossExplosionSoundTemplate =
        new BulletSound( bossExplosionSoundFILE, &error );

    if( error ) {
        printf( "Error reading from bossExplosionSound file\n" );
        }
    
    if( bossExplosionSoundFILE != NULL ) {
        fclose( bossExplosionSoundFILE );
        }


    // "stuff" that is spit out when a bullet hits the boss
    // re-use the ShipBullet code for it
    FILE *bossDamageFILE = LevelDirectoryManager::getStdStream( "bossDamage",
                                                                 true );
    
    error = false;
    ShipBullet *bossDamageTemplate =
        new ShipBullet( bossDamageFILE, &error );

    if( error ) {
        printf( "Error reading from bossDamage file\n" );
        }
    
    if( bossDamageFILE != NULL ) {
        fclose( bossDamageFILE );
        }


    error = false;
    double bossDamageScale =
        LevelDirectoryManager::readDoubleFileContents( "bossDamageScale",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        bossDamageScale = 1;
        }


    error = false;
    double bossDamageTime =
        LevelDirectoryManager::readDoubleFileContents(
            "bossDamageTime",
            &error,
            true );
    
    if( error ) {
        // default
        bossDamageTime = 1;
        }


    mBossDamageManager =
        new ShipBulletManager( bossDamageTemplate,
                               bossDamageScale,
                               // no sounds for damage
                               NULL,
                               NULL,
                               mMaxXPosition - mMinXPosition,
                               mMaxYPosition - mMinYPosition );
    
    
    mBossManager =
        new BossManager( bossTemplate,
                         bossScale,
                         bossExplosionScale,
                         bossExplosionTime,
                         mRandSource->getRandomDouble(),  // explosion shape
                         bossMinVelocity,
                         bossMaxVelocity,
                         mShipBulletManager,
                         mBossBulletManager,
                         mBossDamageManager,
                         bossBulletRange,
                         bossBulletBaseVelocity,
                         bossMinBulletsPerSecond,  //  (calm)
                         bossMaxBulletsPerSecond,  // (angry)
                         bossDamageTime,
                         bossTimeToGetAngry,
                         bossMaxHealth,
                         bossRecoveryRate,
                         new Vector3D( mMinXPosition + 20,
                                       0, 0 ),
                         mSoundPlayer,
                         bossExplosionSoundTemplate );



    
    FILE *portalFILE =
        LevelDirectoryManager::getStdStream( "portal", true );
    
    error = false;
    ParameterizedObject *portalTemplate =
        new ParameterizedObject( portalFILE, &error );

    if( portalFILE != NULL ) {
        fclose( portalFILE );
        }


    error = false;
    double portalScale =
        LevelDirectoryManager::readDoubleFileContents( "portalScale",
                                                       &error,
                                                       true );
    if( error ) {
        // default
        portalScale = 3;
        }

    error = false;
    double portalFadeTime =
        LevelDirectoryManager::readDoubleFileContents(
            "portalFadeTime",
            &error,
            true );
    
    if( error ) {
        // default
        portalFadeTime = 2;
        }


    mPortalManager = new PortalManager( portalTemplate, portalScale,
                                        portalFadeTime,
                                        mMaxXPosition - mMinXPosition );

    mShipInPortal = false;
    

    
    delete currentLevelDirectory;
    delete levelsDirectory;


    mCurrentShipRadius = 0;



    for( i=0; i<mNumEnemies; i++ ) {
        addRandomEnemy();
        }
    }



GameSceneHandler::~GameSceneHandler() {
    delete mRandSource;

    destroyLevel();

    delete mSoundPlayer;
    }


void GameSceneHandler::destroyLevel() {
    
    delete mShipParameterSpace;
    delete mShipBulletManager;
    delete mEnemyManager;
    delete mEnemyBulletManager;
    mSoundPlayer->setMusicPlayer( NULL );
    delete mMusicPlayer;
    delete mSculptureManager;
    delete mBossBulletManager;
    delete mBossDamageManager;
    delete mBossManager;
    delete mPortalManager;
    
    delete mWaveTable;
    
    delete mCurrentShipVelocityVector;

    delete mBackgroundColor;
    delete mNearBossGridColor;
    delete mFarBossGridColor;

    delete mWeakUmbilicalColor;
    delete mStrongUmbilicalColor;
    }



void GameSceneHandler::addRandomEnemy() {
    double x, y;
            
    x = mRandSource->getRandomDouble();
    y = mRandSource->getRandomDouble();

    // round one param at random to push all enemies to edges of world
    int coin = mRandSource->getRandomBoundedInt( 0, 1 );
    if( coin == 0 ) {
        x = rint( x );
        }
    else {
        y = rint( y );
        }

    // enemies start out outside of world
    double maxXEnemyPosition = mMaxXPosition + 50;
    double minXEnemyPosition = mMinXPosition - 50;
    double maxYEnemyPosition = mMaxYPosition + 50;
    double minYEnemyPosition = mMinYPosition - 50;
        
        
    x = x * ( maxXEnemyPosition - minXEnemyPosition ) + minXEnemyPosition;
    y = y * ( maxYEnemyPosition - minYEnemyPosition ) + minYEnemyPosition;
        

    double randRotationZ = mRandSource->getRandomDouble() * 2 * M_PI;
        
    Vector3D *position = new Vector3D( x, y, 0 );
    Angle3D *rotation = new Angle3D( 0, 0, randRotationZ );

    
    double explosionShape = mRandSource->getRandomDouble();

    // enemy shape determines bullet shape
    // thus, enemies that look the similar fire similar bullets
    double enemyShape = mRandSource->getRandomDouble();
    double bulletClose = enemyShape;
    double bulletFar = 1 - enemyShape;
    
    mEnemyManager->addEnemy( enemyShape,
                             explosionShape,
                             1,
                             bulletClose,
                             bulletFar,
                             position,
                             rotation );
    }



void GameSceneHandler::drawScene() {
    glClearColor( mBackgroundColor->r,
                  mBackgroundColor->g,
                  mBackgroundColor->b,
                  mBackgroundColor->a );
	
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );


    Vector3D *viewPosition = mScreen->getViewPosition();
    viewPosition->mZ = 0;

    // draw a large, semi-transparent square to make trails fade over time
    /*
    glBegin( GL_QUADS );
        glColor4f( 0,
                   0,
                   0, 0.75 / exp( mCurrentShipJarForce ) + .25  );
        glVertex2d( mMinXPosition, mMinYPosition );
        glVertex2d( mMinXPosition, mMaxYPosition );
        glVertex2d( mMaxXPosition, mMaxYPosition );
        glVertex2d( mMaxXPosition, mMinYPosition );
    glEnd();
    */
    
    // draw a grid

    // grid color based on boss position
    Vector3D *bossPostion = mBossManager->getBossPosition();
    
    glLineWidth( 2 );
    glBegin( GL_LINES );

        for( double x=mMinXPosition; x<=mMaxXPosition; x+=mGridSpacing ) {
            for( double y=mMinYPosition; y<=mMaxYPosition; y+=mGridSpacing ) {
                
                Vector3D *gridSpot = new Vector3D( x, y, 0 );

                double bossDistance = gridSpot->getDistance( bossPostion );

                double shipDistance = gridSpot->getDistance( viewPosition );

                delete gridSpot;
                
                double nearColorWeight = 1 -
                    bossDistance / ( mMaxXPosition - mMinXPosition );

                if( shipDistance < 20 ) {
                    // ship override's boss' effect on grid
                    double shipFactor = 1 - ( shipDistance / 19 );

                    nearColorWeight -= shipFactor;
                    }

                if( nearColorWeight < 0 ) {
                    nearColorWeight = 0;
                    }

                
                double farColorWeight = 1 - nearColorWeight;

                glColor4f( nearColorWeight * mNearBossGridColor->r +
                               farColorWeight * mFarBossGridColor->r,
                           nearColorWeight * mNearBossGridColor->g +
                               farColorWeight * mFarBossGridColor->g,
                           nearColorWeight * mNearBossGridColor->b +
                               farColorWeight * mFarBossGridColor->b,
                           nearColorWeight * mNearBossGridColor->a +
                               farColorWeight * mFarBossGridColor->a );
            
                glVertex2d( x, y );

                if( y != mMinYPosition &&
                    y != mMaxYPosition ) {

                    // draw start vertex for next segment
                    glVertex2d( x, y );
                    }                
                }
            
            }
        for( double y=mMinYPosition; y<=mMaxYPosition; y+=mGridSpacing ) {
            for( double x=mMinXPosition; x<=mMaxXPosition; x+=mGridSpacing ) {
                
                Vector3D *gridSpot = new Vector3D( x, y, 0 );

                double bossDistance = gridSpot->getDistance( bossPostion );
                
                double shipDistance = gridSpot->getDistance( viewPosition );
                
                delete gridSpot;
                
                double nearColorWeight = 1 -
                    bossDistance / ( mMaxXPosition - mMinXPosition );

                if( shipDistance < 20 ) {
                    // ship override's boss' effect on grid
                    double shipFactor = 1 - ( shipDistance / 19 );

                    nearColorWeight -= shipFactor;
                    }
                
                if( nearColorWeight < 0 ) {
                    nearColorWeight = 0;
                    }
                double farColorWeight = 1 - nearColorWeight;

                glColor4f( nearColorWeight * mNearBossGridColor->r +
                               farColorWeight * mFarBossGridColor->r,
                           nearColorWeight * mNearBossGridColor->g +
                               farColorWeight * mFarBossGridColor->g,
                           nearColorWeight * mNearBossGridColor->b +
                               farColorWeight * mFarBossGridColor->b,
                           nearColorWeight * mNearBossGridColor->a +
                               farColorWeight * mFarBossGridColor->a );
            
                glVertex2d( x, y );

                if( x != mMinXPosition &&
                    x != mMaxXPosition ) {

                    // draw start vertex for next segment
                    glVertex2d( x, y );
                    }                
                }
            }
    glEnd();

    delete bossPostion;
    

    unsigned long lastMillisecondDelta = mFrameMillisecondDelta;
    
    // how many milliseconds have passed since the last frame
    mFrameMillisecondDelta =
        Time::getMillisecondsSince( mLastFrameSeconds,
                                    mLastFrameMilliseconds );

    
    // lock down to 30 frames per second
    unsigned long minFrameTime = (unsigned long)( 1000 / mMaxFrameRate );
    if( mFrameMillisecondDelta < minFrameTime ) {
        unsigned long timeToSleep = minFrameTime - mFrameMillisecondDelta;
        Thread::staticSleep( timeToSleep );

        // get new frame second delta, including sleep time
        mFrameMillisecondDelta =
            Time::getMillisecondsSince( mLastFrameSeconds,
                                        mLastFrameMilliseconds );
        }

    // avoid huge position "jumps" if we have a very large delay during a frame
    // (possibly caused by something going on in the background)
    // This will favor a slight visual slow down, but this is better than
    // a disorienting jump

    // skip this check if we are just starting up
    if( lastMillisecondDelta != 0 ) {
        if( mFrameMillisecondDelta > 6 * lastMillisecondDelta ) {
            // limit:  this frame represents at most twice the jump of the last
            // frame
            // printf( "Limiting time jump (requested=%lu ms, last=%lu ms)\n",
            //        mFrameMillisecondDelta, lastMillisecondDelta );

            if( mFrameMillisecondDelta > 10000 ) {
                printf( "Time between frames more than 10 seconds:\n" );
                // way too big... investigate
                printf( "Last time = %lu s, %lu ms\n",
                        mLastFrameSeconds, mLastFrameMilliseconds );

                Time::getCurrentTime( &mLastFrameSeconds,
                                      &mLastFrameMilliseconds );
                printf( "current time = %lu s, %lu ms\n",
                        mLastFrameSeconds, mLastFrameMilliseconds );

                }
            
            mFrameMillisecondDelta = 2 * lastMillisecondDelta;
            
            }
        }
    
    double frameSecondsDelta = (double)mFrameMillisecondDelta / 1000.0;

    
    // record the time that this frame was drawn
    Time::getCurrentTime( &mLastFrameSeconds, &mLastFrameMilliseconds );


    

    if( !mPaused ) {
        // tell managers about the time delta
        mShipBulletManager->passTime( frameSecondsDelta );
        mEnemyBulletManager->passTime( frameSecondsDelta );
        mEnemyManager->passTime( frameSecondsDelta, viewPosition );
        mSculptureManager->passTime( frameSecondsDelta );
        mBossBulletManager->passTime( frameSecondsDelta );
        mBossDamageManager->passTime( frameSecondsDelta );
        mBossManager->passTime( frameSecondsDelta, viewPosition,
                                mCurrentShipVelocityVector );
        mPortalManager->passTime( frameSecondsDelta, viewPosition );

        // don't add enemies if boss is dead
        if( ! mBossManager->isBossDead() ) {
            // replenish enemy force
            while( mEnemyManager->getEnemyCount() < mNumEnemies ) {
                addRandomEnemy();
                }
            }
        }
    
    int i;



    SimpleVector<DrawableObject *> *shipBulletObjects =
        mShipBulletManager->getDrawableObjects();
    int numShipBulletObjects = shipBulletObjects->size();

    SimpleVector<DrawableObject *> *enemyBulletObjects =
        mEnemyBulletManager->getDrawableObjects();
    int numEnemyBulletObjects = enemyBulletObjects->size();

    SimpleVector<DrawableObject *> *enemyObjects =
        mEnemyManager->getDrawableObjects();
    int numEnemyObjects = enemyObjects->size();

    SimpleVector<DrawableObject *> *sculptureObjects =
        mSculptureManager->getDrawableObjects();
    int numSculptureObjects = sculptureObjects->size();


    SimpleVector<DrawableObject *> *bossBulletObjects =
        mBossBulletManager->getDrawableObjects();
    int numBossBulletObjects = bossBulletObjects->size();

    SimpleVector<DrawableObject *> *bossObjects =
        mBossManager->getDrawableObjects();
    int numBossObjects = bossObjects->size();

    // draw boss damage on top of boss
    SimpleVector<DrawableObject *> *bossDamageObjects =
        mBossDamageManager->getDrawableObjects();
    int numBossDamageObjects = bossDamageObjects->size();
    
    SimpleVector<DrawableObject *> *portalObjects =
        mPortalManager->getDrawableObjects();
    int numPortalObjects = portalObjects->size();



    // draw umbilical cord above grid but under everything else
    glLineWidth( 10 * mSculptureManager->getBulletPowerModifier() );
    glBegin( GL_LINES ); {
        
        Color *closeUmbilicalColor =
            Color::linearSum(
                mStrongUmbilicalColor,
                mWeakUmbilicalColor,
                mSculptureManager->getCloseRangeBulletParameter() );

        Color *farUmbilicalColor =
            Color::linearSum(
                mStrongUmbilicalColor,
                mWeakUmbilicalColor,
                mSculptureManager->getFarRangeBulletParameter() );

        glColor4f( closeUmbilicalColor->r,
                   closeUmbilicalColor->g,
                   closeUmbilicalColor->b,
                   0 );
        glVertex2d( viewPosition->mX, viewPosition->mY );


        glColor4f( farUmbilicalColor->r,
                   farUmbilicalColor->g,
                   farUmbilicalColor->b,
                   1 );
        glVertex2d( 0, 0 );
        
        delete closeUmbilicalColor;
        delete farUmbilicalColor;
        } glEnd();



    Vector3D *offsetVector = new Vector3D( 0, 0, 0 );

    // draw the bullets and enemies with no extra rotation
    Angle3D *zeroAngle = new Angle3D( 0, 0, 0 );

    // bottom layer is sculpture
    for( i=0; i<numSculptureObjects; i++ ) {
        DrawableObject *component =
            *( sculptureObjects->getElement( i ) );
        component->draw( 1, zeroAngle, offsetVector );
        delete component;
        }
    delete sculptureObjects;



    // too confusing... don't draw for now
    /*
    // draw recenter dot on top of sculpture
    glPointSize( 15 );
    glBegin( GL_POINTS ); {

        Vector3D *recenterPosition =
            mSculptureManager->getPiecePositionNeededToRecenter();

        glColor4f( mWeakUmbilicalColor->r,
                   mWeakUmbilicalColor->g,
                   mWeakUmbilicalColor->b,
                   1 );

        // don't draw if sculpture is centered
        if( recenterPosition->mX != 0 ||
            recenterPosition->mY != 0 ) {
            glVertex2d( recenterPosition->mX, recenterPosition->mY );
            }
        delete recenterPosition;
        } glEnd();
    */
    
    // draw music cursor on top of sculpture but under other stuff
    glLineWidth( 2 );
    glBegin( GL_LINES ); {
        if( mSculptureManager->getNumPiecesInSculpture() > 0 ) {
            // music is playing

            // draw the music cursor
            double musicCursorPosition =
                mMusicPlayer->getCurrentPartGridPosition();

            // move it back by half a grid space
            musicCursorPosition -= mGridSpacing / 2;
                
            glColor4f( 0, 1, 0, 0 );
            glVertex2d( musicCursorPosition, mMinYPosition );
            glColor4f( 0, 1, 0, 0.75 );
            glVertex2d( musicCursorPosition, mMinYPosition / 2 );

            glVertex2d( musicCursorPosition, mMinYPosition / 2 );
            glColor4f( 1, 1, 0, 0.75 );
            glVertex2d( musicCursorPosition, 0 );
            
            glVertex2d( musicCursorPosition, 0 );
            glColor4f( 1, 0, 0, 0.75 );
            glVertex2d( musicCursorPosition, mMaxYPosition / 2 );

            glVertex2d( musicCursorPosition, mMaxYPosition / 2 );
            glColor4f( 1, 0, 0, 0 );
            glVertex2d( musicCursorPosition, mMaxYPosition );
            }

        } glEnd();

    
    // next layer is bullets

    for( i=0; i<numEnemyBulletObjects; i++ ) {
        DrawableObject *component =
            *( enemyBulletObjects->getElement( i ) );
        component->draw( 1, zeroAngle, offsetVector );
        delete component;
        }
    delete enemyBulletObjects;
    
    for( i=0; i<numBossBulletObjects; i++ ) {
        DrawableObject *component =
            *( bossBulletObjects->getElement( i ) );
        component->draw( 1, zeroAngle, offsetVector );
        delete component;
        }
    delete bossBulletObjects;


    for( i=0; i<numShipBulletObjects; i++ ) {
        DrawableObject *component =
            *( shipBulletObjects->getElement( i ) );
        component->draw( 1, zeroAngle, offsetVector );
        delete component;
        }
    delete shipBulletObjects;

    
    // then enemies
    for( i=0; i<numEnemyObjects; i++ ) {
        DrawableObject *component =
            *( enemyObjects->getElement( i ) );
        component->draw( 1, zeroAngle, offsetVector );
        delete component;
        }
    delete enemyObjects;
    
    for( i=0; i<numBossObjects; i++ ) {
        DrawableObject *component =
            *( bossObjects->getElement( i ) );
        component->draw( 1, zeroAngle, offsetVector );
        delete component;
        }
    delete bossObjects;

    // then boss damage
    for( i=0; i<numBossDamageObjects; i++ ) {
        DrawableObject *component =
            *( bossDamageObjects->getElement( i ) );
        component->draw( 1, zeroAngle, offsetVector );
        delete component;
        }
    delete bossDamageObjects;

    // finally portal
    for( i=0; i<numPortalObjects; i++ ) {
        DrawableObject *component =
            *( portalObjects->getElement( i ) );
        component->draw( 1, zeroAngle, offsetVector );
        delete component;
        }
    delete portalObjects;


                
    delete zeroAngle;
    
    delete offsetVector;





    

    // map the speed into the range [0,1], with full-speed backward at 0,
    // stationary at 0.5, and full-speed forward at 1
    double shipParameter =
        ( ( mForwardBackwardMoveRate / mMaxMoveRate ) / 2 ) + 0.5;
    
    Angle3D *viewOrientation = mScreen->getViewOrientation();

    double rotationRate;
    SimpleVector<DrawableObject*> *shipObjects =
        mShipParameterSpace->getDrawableObjects( shipParameter,
                                                 &rotationRate );

    int numComponentObjects = shipObjects->size();

    // compute the half radius of this ship as we draw it
    double minRadius = DBL_MAX;
    double maxRadius = 0;

    // draw ship components
    for( int j=0; j<numComponentObjects; j++ ) {
        
        DrawableObject *component =
            *( shipObjects->getElement( j ) );
        
        component->draw( mShipScale, viewOrientation, viewPosition );

        component->scale( mShipScale );
        component->rotate( viewOrientation );
        component->move( viewPosition );
        
        double componentMinRadius =
            component->getBorderMinDistance( viewPosition );
        double componentMaxRadius =
            component->getBorderMaxDistance( viewPosition );

        if( componentMinRadius < minRadius ) {
            minRadius = componentMinRadius;
            }
        if( componentMaxRadius > maxRadius ) {
            maxRadius = componentMaxRadius;
            }
        
        delete component;
        }

    mCurrentShipRadius = ( minRadius + maxRadius ) / 2;
    
    delete shipObjects;


    delete viewPosition;


    // draw an overlay rectangle if we are fading out
    if( mFadeLevel < 1 ) {

        glBegin( GL_QUADS );
            glColor4f( 0, 0, 0, 1 - mFadeLevel );

            glVertex2d( 20 * mMinXPosition, 20 * mMinYPosition );
            glVertex2d( 20 * mMinXPosition, 20 * mMaxYPosition );
            glVertex2d( 20 * mMaxXPosition, 20 * mMaxYPosition );
            glVertex2d( 20 * mMaxXPosition, 20 * mMinYPosition );

        glEnd();
        }


    
    mNumFrames ++;

    if( mPrintFrameRate ) {
        
        if( mNumFrames % mFrameBatchSize == 0 ) {
            // finished a batch
            
            unsigned long timeDelta =
                Time::getMillisecondsSince( mFrameBatchStartTimeSeconds,
                                            mFrameBatchStartTimeMilliseconds );

            double frameRate =
                1000 * (double)mFrameBatchSize / (double)timeDelta;
            
            printf( "Frame rate = %f frames/second\n", frameRate );

            mFrameBatchStartTimeSeconds = mLastFrameSeconds;
            mFrameBatchStartTimeMilliseconds = mLastFrameMilliseconds;
            }
        }
        
    }



void GameSceneHandler::keyPressed(
	unsigned char inKey, int inX, int inY ) {

    if( mPaused ) {
        // ignore all keys except for un-pause and quit
        if( inKey == 'p' || inKey == 'P' ) {
            mPaused = !mPaused;
            }
        else if( inKey == 'q' || inKey == 'Q' ) {
            // quit
            ::exit( 0 );
            }
        
        return;
        }

    if( mShipInPortal ) {
        // ignore all other keys
        return;
        }
                
    
    if( inKey == ' ' ) {

        // make sure there are not already too many bullets
        if( mShipBulletManager->getBulletCount() < mMaxNumShipBullets ) {

            // fire a bullet

            Vector3D *viewPosition = mScreen->getViewPosition();
            viewPosition->mZ = 0;
            
            Angle3D *viewAngle = new Angle3D( mScreen->getViewOrientation() );

            Vector3D *velocityVector =
                new Vector3D( 0, mShipBulletBaseVelocity, 0 );
            velocityVector->rotate( viewAngle );

            velocityVector->add( mCurrentShipVelocityVector );
            
            double bulletMoveRate = velocityVector->getLength();
        
            mShipBulletManager->addBullet(
                mSculptureManager->getCloseRangeBulletParameter(),
                mSculptureManager->getFarRangeBulletParameter(),
                mSculptureManager->getBulletPowerModifier(),
                ( mShipBulletRange /
                  mShipBulletBaseVelocity ) *
                bulletMoveRate,
                viewPosition,
                viewAngle,
                velocityVector );
            }
        }

    if( inKey == 'd' || inKey == 'D' ) {
		if( mCurrentPieceCarried != -1 ) {
            // drop the piece and align it to the grid
            Vector3D *droppedPiecePosition = mScreen->getViewPosition();
            double pieceX = droppedPiecePosition->mX;
            double pieceY = droppedPiecePosition->mY;

            // round to closest multiples of 10 (grid points)
            pieceX = pieceX / 10;
            pieceY = pieceY / 10;
            pieceX = 10 * rint( pieceX );
            pieceY = 10 * rint( pieceY );

            droppedPiecePosition->mX = pieceX;
            droppedPiecePosition->mY = pieceY;
            droppedPiecePosition->mZ = 0;

            // enable smooth move to closest grid point
            mSculptureManager->turnMagnetModeOn( mCurrentPieceCarried );
            
            mSculptureManager->setPiecePosition( mCurrentPieceCarried,
                                                 droppedPiecePosition );

            delete droppedPiecePosition;
            
            mCurrentPieceCarried = -1;
            }
        else {
            // try picking up a piece;
            Vector3D *viewPosition = mScreen->getViewPosition();
            viewPosition->mZ = 0;

            // double ship radius to make piece pick-up easier
            mCurrentPieceCarried =
                mSculptureManager->getSculpturePieceInCircle(
                    viewPosition,
                    2 * mCurrentShipRadius );
            
            delete viewPosition;

            if( mCurrentPieceCarried != -1 ) {
                // enable smooth piece move as we pick up the piece
                mSculptureManager->turnMagnetModeOn( mCurrentPieceCarried );
                }
            }
		}

    else if( inKey == 's' || inKey == 'S' ) {
		mMovingLeft = true;
		}
    else if( inKey == 'f' || inKey == 'F' ) {
		mMovingRight = true;
		}

    
    else if( inKey == 'p' || inKey == 'P' ) {
        mPaused = !mPaused;
        }
    else if( inKey == 'q' || inKey == 'Q' ) {
		// quit
		::exit( 0 );
		}		
	}



void GameSceneHandler::keyReleased(
	unsigned char inKey, int inX, int inY ) {

    // never ignore key releases
    
    if( inKey == 's' || inKey == 'S' ) {
		mMovingLeft = false;
		}
    else if( inKey == 'f' || inKey == 'F' ) {
		mMovingRight = false;
		}
    /*
    else if( inKey == '8' ) {
        // cheat... skip to next level

        // jump to full fade-out to show proper fade-in when loading
        // next level
        mFadeLevel = 0;
        destroyLevel();
        
        loadNextLevel();
        }
    */
	}



void GameSceneHandler::specialKeyPressed(
	int inKey, int inX, int inY ) {

    if( mPaused || mShipInPortal ) {
        // ignore keys
        return;
        }
    
    if( inKey == GLUT_KEY_UP ) {
		mMovingUp = true;
		}

    else if( inKey == GLUT_KEY_DOWN ) {
		mMovingDown = true;
		}
    
    else if( inKey == GLUT_KEY_LEFT ) {
		mRotatingCounterClockwise = true;
		}
    else if( inKey == GLUT_KEY_RIGHT ) {
		mRotatingClockwise = true;
		}
    
	}



void GameSceneHandler::specialKeyReleased(
	int inKey, int inX, int inY ) {

    // never ignore key releases
    
    if( inKey == GLUT_KEY_UP ) {
		mMovingUp = false;
		}

    else if( inKey == GLUT_KEY_DOWN ) {
		mMovingDown = false;
		}
    
    else if( inKey == GLUT_KEY_LEFT ) {
		mRotatingCounterClockwise = false;
		}
    else if( inKey == GLUT_KEY_RIGHT ) {
		mRotatingClockwise = false;
		}
	}


void GameSceneHandler::fireRedraw() {

    if( mPaused ) {
        // ignore redraw event
        return;
        }
    
    double frameSecondsDelta = (double)mFrameMillisecondDelta / 1000.0;

        
    Vector3D *moveVector = new Vector3D( 0, 0, 0 );
    
    if( mMovingUp ) {
        mForwardBackwardMoveRate += mShipAccelleration * frameSecondsDelta;
        if( mForwardBackwardMoveRate > mMaxMoveRate ) {
            mForwardBackwardMoveRate = mMaxMoveRate;
            }
        
        }
    if( mMovingDown ) {
        mForwardBackwardMoveRate -= mShipAccelleration * frameSecondsDelta;
        if( mForwardBackwardMoveRate < -mMaxMoveRate ) {
            mForwardBackwardMoveRate = -mMaxMoveRate;
            }
        }


    moveVector->mY += mForwardBackwardMoveRate;


    if( mMovingRight ) {
        mRightLeftMoveRate += mShipAccelleration * frameSecondsDelta;
        if( mRightLeftMoveRate > mMaxMoveRate ) {
            mRightLeftMoveRate = mMaxMoveRate;
            }
        
        }
    if( mMovingLeft ) {
        mRightLeftMoveRate -= mShipAccelleration * frameSecondsDelta;
        if( mRightLeftMoveRate < -mMaxMoveRate ) {
            mRightLeftMoveRate = -mMaxMoveRate;
            }
        }

    // x axis flipped on screen
    moveVector->mX -= mRightLeftMoveRate;

    // friction to slow ship down
    if( !mMovingUp && mForwardBackwardMoveRate > 0 ) {
        mForwardBackwardMoveRate -= mShipFriction * frameSecondsDelta;

        if( mForwardBackwardMoveRate < 0 ) {
            mForwardBackwardMoveRate = 0;
            }
        }

    if( !mMovingDown && mForwardBackwardMoveRate < 0 ) {
        mForwardBackwardMoveRate += mShipFriction * frameSecondsDelta;

        if( mForwardBackwardMoveRate > 0 ) {
            mForwardBackwardMoveRate = 0;
            }
        }

    if( !mMovingRight && mRightLeftMoveRate > 0 ) {
        mRightLeftMoveRate -= mShipFriction * frameSecondsDelta;

        if( mRightLeftMoveRate < 0 ) {
            mRightLeftMoveRate = 0;
            }
        }

    if( !mMovingLeft && mRightLeftMoveRate < 0 ) {
        mRightLeftMoveRate += mShipFriction * frameSecondsDelta;

        if( mRightLeftMoveRate > 0 ) {
            mRightLeftMoveRate = 0;
            }
        }
    
    
    Vector3D *viewPosition = mScreen->getViewPosition();

    // zoom out at high speeds
    double netMotionRate =
        sqrt( mForwardBackwardMoveRate * mForwardBackwardMoveRate +
              mRightLeftMoveRate * mRightLeftMoveRate );
    
    viewPosition->mZ =
        baseViewZ
        - 40 * netMotionRate / mMaxMoveRate;
        
    // also zoom from way out when we are fading in
    // and zoom to way out when we are fading out
    viewPosition->mZ -= 1000 * ( 1 - mFadeLevel ); 
    
    mScreen->setViewPosition( viewPosition );

    delete viewPosition;


    // we rotate at a fixed number of angle units per second

    if( mRotatingClockwise ) {
        if( mRotationRate <= 0 ) {
            // jump right to base rate
            mRotationRate = mBaseRotationRate;
            }
        // add accelleration
        mRotationRate += mShipRotationAccelleration * frameSecondsDelta;
        if( mRotationRate > mMaxRotationRate ) {
            mRotationRate = mMaxRotationRate;
            }
        
        }
    else if( mRotatingCounterClockwise ) {
        if( mRotationRate >= 0 ) {
            // jump right to base rate
            mRotationRate = -mBaseRotationRate;
            }
        // add accelleration
        mRotationRate -= mShipRotationAccelleration * frameSecondsDelta;
        if( mRotationRate < -mMaxRotationRate ) {
            mRotationRate = -mMaxRotationRate;
            }
        }
    else {
        // unlike motion, we stop rotating instantly when key is released
        mRotationRate = 0;
        }

    Angle3D *rotationDelta =
        new Angle3D( 0, 0, mRotationRate * frameSecondsDelta );

    if( !mShipInPortal ) {
        mScreen->rotateView( rotationDelta );
        }
    
    delete rotationDelta;
    
    Angle3D *rotation = new Angle3D( mScreen->getViewOrientation() );


    moveVector->rotate( rotation );
    

    // moveVector in units per second
    // save it as our current velocity vector
    delete mCurrentShipVelocityVector;
    mCurrentShipVelocityVector = new Vector3D( moveVector );
    
    // must be scaled by how many seconds are in our frame
    moveVector->scale( frameSecondsDelta );

    if( !mShipInPortal ) {
        mScreen->moveView( moveVector );
        }
    
    delete rotation;
    delete moveVector;    

    
    // wrap around if we go out of bounds
    
    Vector3D *jumpToWrapVector = new Vector3D( 0, 0, 0 );
    
    Vector3D *currentPosition = mScreen->getViewPosition();

    if( currentPosition->mX > mMaxXPosition ) {
        //jumpToWrapVector->mX = -100;
        jumpToWrapVector->mX = mMaxXPosition - currentPosition->mX;
        mCurrentShipVelocityVector->mX = 0;
        }
    if( currentPosition->mX < mMinXPosition ) {
        //jumpToWrapVector->mX = 100;
        jumpToWrapVector->mX = mMinXPosition - currentPosition->mX;
        mCurrentShipVelocityVector->mX = 0;
        }
    if( currentPosition->mY > mMaxYPosition ) {
        //jumpToWrapVector->mY = -100;
        jumpToWrapVector->mY = mMaxYPosition - currentPosition->mY;
        mCurrentShipVelocityVector->mY = 0;
        }
    if( currentPosition->mY < mMinYPosition ) {
        //jumpToWrapVector->mY = 100;
        jumpToWrapVector->mY = mMinYPosition - currentPosition->mY;
        mCurrentShipVelocityVector->mY = 0;
        }

    if( !mShipInPortal ) {
        mScreen->moveView( jumpToWrapVector );
        }
    delete jumpToWrapVector;

    delete currentPosition;
    currentPosition = mScreen->getViewPosition();
    currentPosition->mZ = 0;
    
    // test if we have been hit by an enemy bullet    
    
    double enemyBulletPower =
        mEnemyBulletManager->getBulletPowerInCircle( currentPosition,
                                                     mCurrentShipRadius );
    double bossBulletPower =
        mBossBulletManager->getBulletPowerInCircle( currentPosition,
                                                    mCurrentShipRadius );

    // scale bullet powers by time delta to make it framerate independent
    double jarForceIncrease =
        frameSecondsDelta * enemyBulletPower * mEnemyBulletShipJarPower +
        frameSecondsDelta * bossBulletPower * mBossBulletShipJarPower;

        
    // increased by being hit by bullets
    mCurrentShipJarForce += jarForceIncrease;
        
    // decayed by friction
    mCurrentShipJarForce -= mShipAccelleration * frameSecondsDelta;

    // never negative
    if( mCurrentShipJarForce < 0 ) {
        mCurrentShipJarForce = 0;
        }

    
    // jar ship toward center
    Vector3D *jarVector = new Vector3D( 0, 0, 0 );
    jarVector->subtract( currentPosition );

    // don't jar if already close to center
    double distanceFromCenter = jarVector->getLength();

    if( distanceFromCenter > 1 ) {
        
        jarVector->normalize();

        // compute jar velocity vector
        jarVector->scale( mCurrentShipJarForce );

        //mCurrentShipVelocityVector->add( jarVector );

        // scale by time delta to get actual distance jarred during this frame
        jarVector->scale( frameSecondsDelta );

        if( jarVector->getLength() > distanceFromCenter ) {
            // this jar vector will move us past the center

            // ignore it and cancel the jar force
            mCurrentShipJarForce = 0;

            // move the ship to the center
            Vector3D *moveToCenterVector = new Vector3D( 0, 0, 0 );
            moveToCenterVector->subtract( currentPosition );
            
            if( !mShipInPortal ) {
                mScreen->moveView( moveToCenterVector );
                }
            delete moveToCenterVector;
            }
        else {
            if( !mShipInPortal ) {
                mScreen->moveView( jarVector );
                }
            }
        }
    else {
        // close to center, so jar force immediately decays
        mCurrentShipJarForce = 0;
        }

    delete jarVector;

    
    delete currentPosition;
    currentPosition = mScreen->getViewPosition();
    currentPosition->mZ = 0;

    
    // move the piece we are carrying
    if( mCurrentPieceCarried != -1 ) {
        if( mSculptureManager->isPieceJarred( mCurrentPieceCarried ) ||
            mCurrentShipJarForce > 0 ) {
            // drop piece if it is being jarred or if we are jarred
            mCurrentPieceCarried = -1;
            }
        else {
            mSculptureManager->setPiecePosition( mCurrentPieceCarried,
                                                 currentPosition );
            }
        }


    if( mBossManager->isBossDead() &&
        ! mPortalManager->isPortalVisible() ) {
        // show portal where boss died
        mPortalManager->showPortal( mBossManager->getBossPosition() );

        // destroy all enemies
        mEnemyManager->explodeAllEnemies();
        }
    else if( mBossManager->isBossDead() &&
             mPortalManager->isPortalVisible() &&
             mPortalManager->isShipInPortal( currentPosition ) ) {

        mShipInPortal = true;

        // handle fade out
        
        if( mFadeLevel > 0 ) {
            // fade out more
            double fadeDelta = frameSecondsDelta / mFadeTime;

            mFadeLevel -= fadeDelta;
            if( mFadeLevel < 0 ) {
                mFadeLevel = 0;
                }
            
            // fade music loudness too to avoid clicks when we stop
            // this level's music
            mSoundPlayer->setMusicLoudness( mMusicLoudness * mFadeLevel );
            }


        if( mFadeLevel == 0 ) {
            // switch to next level
            destroyLevel();
            
            
            loadNextLevel();
            }

        }

    // handle fade in
    if( ! mBossManager->isBossDead() && mFadeLevel < 1 ) {
        // fade in more
        double fadeDelta = frameSecondsDelta / mFadeTime;

        mFadeLevel += fadeDelta;
        if( mFadeLevel > 1 ) {
            mFadeLevel = 1;
            }

        // fade music loudness back in
        mSoundPlayer->setMusicLoudness( mMusicLoudness * mFadeLevel );
        }



    
    delete currentPosition;
    }



/*
 * Modification History
 *
 * 2004-June-21   Jason Rohrer
 * Created.
 *
 * 2004-August-6   Jason Rohrer
 * Fixed consistency bugs in test for in/out of sculpture status.
 *
 * 2004-August-22   Jason Rohrer
 * Added music parts.
 *
 * 2004-August-23   Jason Rohrer
 * Added function for getting number of pieces in sculpture.
 *
 * 2004-August-29   Jason Rohrer
 * Changed so that only pieces in sculpture are animated.
 *
 * 2004-August-31   Jason Rohrer
 * Added function for getting piece position needed to recenter.
 *
 * 2005-August-22   Jason Rohrer
 * Changed so that isPieceJarred returns true only if jar force is increasing.
 * Added magnet mode to smooth piece pick-up and drop.
 */



#ifndef SCULPTURE_MANAGER_INCLUDED
#define SCULPTURE_MANAGER_INCLUDED


#include "ParameterizedObject.h"
#include "ShipBulletManager.h"
#include "MusicPart.h"


#include "minorGems/util/SimpleVector.h"
#include "minorGems/math/geometry/Vector3D.h"
#include "minorGems/math/geometry/Angle3D.h"



/**
 * A class that manages and draws all sculpture pieces in the environment.
 *
 * @author Jason Rohrer.
 */
class SculptureManager {


    public:


        
        /**
         * Constructs a manager.
         *
         * @param inFirstSculptureTemplate the class to use to draw the first
         *   anmimation frame of all sculpture pieces.
         *   Will be destroyed by this class.
         * @param inSecondSculptureTemplate the class to use to draw the second
         *   anmimation frame of all sculpture pieces.
         *   Will be destroyed by this class.
         * @param inSculptureScale the multiplier to scale the template by.
         * @param inMaxDistanceToBeInSculpture how far a piece can be from
         *   other sculpture pieces to be counted as "in" the sculpture.
         * @param inAnimationLoopTime the time in seconds of a full
         *   sculpture animation loop.
         * @param inNumSculpturePieces the number of sculpture pieces.
         * @param inPieceParameters array of shape parameters, one for each
         *   piece.
         *   Will be destroyed by this class.
         * @param inPieceStartingPositions array of starting positions,
         *   one for each piece.
         *   Will be destroyed by this class.
         * @param inPieceStartingRotations array of starting rotations,
         *   one for each piece.
         *   Will be destroyed by this class.
         * @param inPieceMusicParts array of music parts,
         *   one for each piece.
         *   Will be destroyed by this class.
         * @param inSculpturePowerUpSpaceFILE the file to read the sculpture
         *   piece power-up mapping from (maps [0,1] parameters into the
         *   2 parameter ship bullet space).
         *   Must be closed by caller.
         * @param outError pointer to where error flag should be returned.
         *   Destination will be set to true if reading the space
         *   from inSculpturePowerUpSpaceFILE fails.
         * @param inEnemyBulletManager the manager for enemy bullets.
         *   Must be destroyed by caller after this class is destroyed.
         * @param inEnemyBulletJarPower the power of enemy bullets.
         * @param inBossBulletManager the manager for boss bullets.
         *   Must be destroyed by caller after this class is destroyed.
         * @param inBossBulletJarPower the power of boss bullets.
         * @param inFriction the force of friction on jarred pieces.
         * @param inWorldWidth the width of the world (for wrap-around).
         * @param inWorldHeight the height of the world (for wrap-around).
         */
        SculptureManager( ParameterizedObject *inFirstSculptureTemplate,
                          ParameterizedObject *inSecondSculptureTemplate,
                          double inSculptureScale,
                          double inMaxDistanceToBeInSculpture,
                          double inAnimationLoopTime,
                          int inNumSculpturePieces,
                          double *inPieceShapeParameters,
                          Vector3D **inPieceStartingPositions,
                          Angle3D **inPieceStartingRotations,
                          MusicPart **inPieceMusicParts,
                          FILE *inSculpturePowerUpSpaceFILE,
                          char *outError,
                          ShipBulletManager *inEnemyBulletManager,
                          double inEnemyBulletJarPower,
                          ShipBulletManager *inBossBulletManager,
                          double inBossBulletJarPower,
                          double inFriction,
                          double inWorldWidth,
                          double inWorldHeight );


        
        virtual ~SculptureManager();



        /**
         * Gets the current parameter for close-range ship bullets.
         *
         * @return the close-range parameter, in range [0,1].
         */
        double getCloseRangeBulletParameter();

        
        
        /**
         * Gets the current parameter for far-range ship bullets.
         *
         * @return the far-range parameter, in range [0,1].
         */
        double getFarRangeBulletParameter();

        

        /**
         * Gets the current ship bullet power modifier.
         *
         * @return the power modifier, in range [0,1].
         */
        double getBulletPowerModifier();
        
        

        /**
         * Tell this manager that time has passed.
         *
         * @param inTimeDeltaInSeconds the amount of time that has passed.
         */
        void passTime( double inTimeDeltaInSeconds );


        
        /**
         * Gets the sculpture piece that is in a circle.  If more
         * than one piece is in the circle, the closest piece to the center
         * is returned. Ties are decided in an arbitrary but deterministic
         * fashion.
         *
         * Considers all pieces, even those not in the sculpture.
         *
         * @param inCenter the center of the circle.
         *   Must be destroyed by caller.
         * @param inRadius the radius of the circle.
         *
         * @return a non-negative piece handle, or -1 if no piece is in the
         *   circle.
         */
        int getSculpturePieceInCircle( Vector3D *inCenter,
                                       double inRadius );

        

        /**
         * Gets the piece of the sculpture that is closest to a given
         * point.
         *
         * Only considers pieces that are in the sculpture.
         *
         * @param inPosition the point to measure distances from.
         *   Must be destroyed by caller.
         *
         * @return the position of the closest sculpture piece,
         *   or NULL if the sculpture has no pieces in it.
         *   Must be destroyed by caller.
         */
        Vector3D *getPositionOfClosestSculpturePiece( Vector3D *inPosition );


        
        /**
         * Gets the number of pieces that are currently connected
         * together in the sculpture.
         *
         * @return the number of pieces.
         */
        int getNumPiecesInSculpture();

        
        
        /**
         * Gets the positions for all pieces in the sculpture.
         *
         * @param outNumPieces pointer to where the number of pieces
         *   should be returned.
         *
         * @return an array of vector positions, one for each piece.
         *   Vectors and array must be destroyed by caller.
         */
        Vector3D **getPiecePositions( int *outNumPieces );


        
        /**
         * Gets the music parts for all pieces in the sculpture.
         *
         * As long as sculpture piece positions have not been
         * modified with setPiecePosition between calls,
         * the pieces described by getPiecePositions and getPieceMusicParts
         * are in the same order.
         * In other words, the piece at position with index i has its
         * music part at index i.
         *
         * @param outNumPieces pointer to where the number of pieces
         *   should be returned.
         *
         * @return an array of music parts, one for each piece.
         *   Array must be destroyed by caller.
         *   Music parts SHOULD NOT be modified or destroyed by caller.
         */
        MusicPart **getPieceMusicParts( int *outNumPieces );


        
        /**
         * Gets the center of mass of the sculpture.
         *
         * If the sculpture is empty, the zero vector is returned.
         *
         * @return the center of the sculpture.
         *   Must be destroyed by caller.
         */
        Vector3D *getSculptureCenter();

        

        /**
         * Gets the position that piece would need to be placed at in
         * order to recenter the sculpture.
         *
         * If the sculpture is already centered, the zero vector
         * will be returned (since the only way to keep the sculpture
         * centered when adding a piece would be to add that piece to the
         * center).
         *
         * @return the piece position that would recenter the scupture.
         *   Must be destroyed by caller.
         */
        Vector3D *getPiecePositionNeededToRecenter();

        
        
        /**
         * Puts a piece into magnet mode.
         * In magnet mode, a piece is drawn smoothly toward the positions set
         * with setPiecePosition until the piece finally hits the set position.
         * Once the position is hit, the magnet mode turns off.
         *
         * @param inPieceHandle the handle of the piece to turn magnet mode
         *   on for.
         */
        void turnMagnetModeOn( int inPieceHandle );


        
        /**
         * Sets the position of a piece.
         *
         * @param inPieceHandle the handle for the piece to move.
         * @param inNewPosition the new position of the piece.
         *   Must be destroyed by caller.
         */
        void setPiecePosition( int inPieceHandle, Vector3D *inNewPosition );



        /**
         * Check if a piece is currently being jarred by enemy fire.
         * 
         * @param inPieceHandle the handle for the piece to check.
         *
         * @return true if the piece is being jarred or false if it is not.
         *   Returns true only if the jar force is increasing.
         *   Piece may be still moving from the force of a previous bullet
         *   when isPieceJarred returns false.
         */
        char isPieceJarred( int inPieceHandle );

        
        
        /**
         * Gets drawable objects for all sculptures in their current
         * positions/states.
         *
         * @return all sculptures as a collection of drawable objects.
         *   Vector and objects must be destroyed by caller.
         */
        SimpleVector<DrawableObject *> *getDrawableObjects();


        
    protected:

        ParameterizedObject *mFirstSculpturePieceTemplate;
        ParameterizedObject *mSecondSculpturePieceTemplate;
        double mSculptureScale;
        double mMaxDistanceToBePartOfSculpture;

        double mAnimationLoopTime;

        double mCurrentAnimationPosition;
        double mCurrentAnimationDirection;
        
        int mNumSculpturePieces;
        double *mSculpturePieceParameters;

        // current position for each piece
        Vector3D **mCurrentPiecePositions;

        // flags indicating which pieces are in magnet mode
        // in magnet mode, pieces move smoothly toward their target
        // once they hit their target, magnet mode turns off and they
        // stick to their set position
        char *mPieceMagnetModes;
        // position each piece is trying to move toward
        Vector3D **mCurrentPieceTargetPositions;
        // velocity at which each piece is moving toward its target
        double *mCurrentTowardTargetVelocities;
        
        Angle3D **mCurrentPieceRotations;

        MusicPart **mPieceMusicParts;
        
        double *mCurrentPieceRotationRates;
        double *mCurrentPieceRadii;
        double *mCurrentJarForces;
        char *mJarForcesIncreasing;
        
        int mNumPiecesInSculpture;
        char *mInSculptureFlags;
        char *mDelayAnimationStartFlags;
        char *mDelayAnimationStopFlags;
        
        ShipBulletManager *mEnemyBulletManager;
        double mEnemyBulletJarPower;
        ShipBulletManager *mBossBulletManager;
        double mBossBulletJarPower;

        double mFriction;
        
        double mMaxXPosition;
        double mMinXPosition;
        double mMaxYPosition;
        double mMinYPosition;
        

        int mNumParameterMapAnchors;
        
        double *mParameterMapAnchors;

        double *mParameterMapCloseRangeValues;
        double *mParameterMapFarRangeValues;

        

        /**
         * Maps a parameter into the power up space.
         *
         * @param inParameter the parameter to map.  In range [0,1].
         * @param outCloseRangePowerUp pointer to where the close-range
         *   power up should be returned.
         * @param outFarRangePowerUp pointer to where the far-range
         *   power up should be returned.
         */
        void mapParameters( double inParameter,
                            double *outCloseRangePowerUp,
                            double *outFarRangePowerUp );

        

        /**
         * Updates all flags indicating whether pieces are in or out of
         * the sculpture.
         *
         * Should be called when a piece changes position.
         */
        void updateInOutStatusOfAllPieces();


        
    };



#endif

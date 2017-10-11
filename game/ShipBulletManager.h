/*
 * Modification History
 *
 * 2004-June-16   Jason Rohrer
 * Created.
 *
 * 2004-August-15   Jason Rohrer
 * Added sound support.
 *
 * 2004-August-30   Jason Rohrer
 * Added bullet scaling parameters.
 *
 * 2004-October-13   Jason Rohrer
 * Added function for getting bullet count.
 *
 * 2005-August-22   Jason Rohrer
 * Started work on boss damage graphics.
 *
 * 2005-August-23   Jason Rohrer
 * Finished boss damage graphics.
 */



#ifndef SHIP_BULLET_MANAGER_INCLUDED
#define SHIP_BULLET_MANAGER_INCLUDED



#include "ShipBullet.h"
#include "SoundPlayer.h"
#include "BulletSound.h"

#include "minorGems/util/SimpleVector.h"
#include "minorGems/math/geometry/Vector3D.h"
#include "minorGems/math/geometry/Angle3D.h"

#include <stdio.h>



/**
 * A class that manages and draws all ship bullets in the environment.
 *
 * @author Jason Rohrer.
 */
class ShipBulletManager {


    public:


        
        /**
         * Constructs a manager.
         *
         * @param inBulletTemplate the class to use to draw all bullets.
         *   Will be destroyed by this class.
         * @param inBulletScale the scale factor for bullet
         *   graphics.
         * @param inPlayer the player to direct bullet sounds to, or NULL
         *   to not play sounds.
         *   Must be destroyed by caller after this class is destroyed.
         * @param inBulletSoundTemplate the class to use to generate bullet
         *   Sounds, or NULL to not play sounds.
         *   Will be destroyed by this class.
         * @param inWorldWidth the width of the world (for wrap-around).
         * @param inWorldHeight the height of the world (for wrap-around).
         */
        ShipBulletManager( ShipBullet *inBulletTemplate,
                           double inBulletScale,
                           SoundPlayer *inPlayer,
                           BulletSound *inBulletSoundTemplate,
                           double inWorldWidth,
                           double inWorldHeight );


        
        virtual ~ShipBulletManager();

        

        /**
         * Adds a bullet to this manager.
         *
         * @param inCloseRangeParameter the parameter in [0,1] that controls
         *   the shape and power of the bullet at close range.
         * @param inFarRangeParameter the parameter in [0,1] that controls
         *   the shape and power of the bullet at far range.
         * @param inPowerModifier a value in [0,1] that further modifies
         *   (multiplicatively) the power of the bullet (visually represented
         *   by bullet transparency.
         * @param inRangeInScreenUnits the length of this bullet's range
         *   in screen units.
         *   This will be interpreted as bullet life time in seconds if the
         *   bullet velocity vector (inVelocityInScreenUnitsPerSecond) is 0.
         * @param inStartingPosition the starting position of this bullet.
         *   Will be destroyed by this class.
         * @param inStartingRotation the starting rotation angle. 
         *   Will be destroyed by this class.
         * @param inVelocityInScreenUnitsPerSecond the velocity vector for
         *   this bullet.
         *   Will be destroyed by this class.
         */
        void addBullet( double inCloseRangeParameter,
                        double inFarRangeParameter,
                        double inPowerModifier,
                        double inRangeInScreenUnits,
                        Vector3D *inStartingPosition,
                        Angle3D *inStartingRotation,
                        Vector3D *inVelocityInScreenUnitsPerSecond );


        
        /**
         * Gets the number of active bullets.
         *
         * @return the number of bullets currently being handled by this
         *   manager.
         */
        int getBulletCount();
        

        
        /**
         * Gets the sum of bullet powers in a circular region of the world.
         * Wraps around if circle extends off the edge of the world.
         * 
         * @param inCircleCenter the center of the circle.
         *   Must be destroyed by caller.
         * @param inCircleRadius the radius of the circle.
         */
        double getBulletPowerInCircle( Vector3D *inCircleCenter,
                                       double inCircleRadius );

        

        /**
         * Gets the positions of all bullets in a circular region of the world.
         * Wraps around if circle extends off the edge of the world.
         * 
         * @param inCircleCenter the center of the circle.
         *   Must be destroyed by caller.
         * @param inCircleRadius the radius of the circle.
         * @param outNumBullets pointer to where bullet count should be
         *   returned.
         *
         * @return an array of vector positions.
         *   Array and vectors must be destroyed by caller.
         */
        Vector3D **getBulletPositionsInCircle( Vector3D *inCircleCenter,
                                               double inCircleRadius,
                                               int *outNumBullets );


        
        /**
         * Tell this manager that time has passed.
         *
         * @param inTimeDeltaInSeconds the amount of time that has passed.
         */
        void passTime( double inTimeDeltaInSeconds );

        
        
        /**
         * Gets drawable objects for all bullets in their current
         * positions/states.
         *
         * @return all bullets as a collection of drawable objects.
         *   Vector and objects must be destroyed by caller.
         */
        SimpleVector<DrawableObject *> *getDrawableObjects();


        
    protected:

        ShipBullet *mBulletTemplate;
        double mBulletScale;
        
        SoundPlayer *mSoundPlayer;
        BulletSound *mBulletSoundTemplate;
        
        double mMaxXPosition;
        double mMinXPosition;
        double mMaxYPosition;
        double mMinYPosition;
        
        
        SimpleVector<double> *mCloseRangeParameters;
        SimpleVector<double> *mFarRangeParameters;
        SimpleVector<double> *mPowerModifiers;
        SimpleVector<double> *mCurrentPowers;
        
        // how long each bullet range is in screen units
        SimpleVector<double> *mRangesInScreenUnits;

        // how far each bullet is along in its range, in [0,1]
        SimpleVector<double> *mRangeFractions;

        // how much time has passed during the life of this bullet
        SimpleVector<double> *mTimePassed;
        
        // since velocities are constant, we can always
        // compute bullet position from the starting position, the time,
        // and the velocity vector
        SimpleVector<Vector3D *> *mStartingPositions;
        
        SimpleVector<Vector3D *> *mVelocitiesInScreenUnitsPerSecond;

        SimpleVector<Vector3D *> *mCurrentPositions;


        // rotation rates can vary across a bullet's lifespan, so we must
        // keep a current angle for each bullet and adjust the angle
        // for each time delta
        SimpleVector<Angle3D *> *mCurrentRotations;


        SimpleVector<double> *mCurrentRotationRates;

        SimpleVector<char> *mSholdBeDestroyedFlags;

        
    };



#endif

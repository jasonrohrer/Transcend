/*
 * Modification History
 *
 * 2004-June-15   Jason Rohrer
 * Created.
 */



#ifndef SHIP_BULLET_INCLUDED
#define SHIP_BULLET_INCLUDED



#include "ParameterizedObject.h"



/**
 * A bullet that can be controled with 2 parameters.
 *
 * @author Jason Rohrer.
 */
class ShipBullet {


    public:


        
        /**
         * Constructs a bullet by reading values from a text file
         * stream.
         *
         * @param inFILE the open file to read from.
         *   Must be closed by caller.
         * @param outError pointer to where error flag should be returned.
         *   Destination will be set to true if reading the bullet
         *   from inFILE fails.
         */
        ShipBullet( FILE *inFILE, char *outError );


        
        virtual ~ShipBullet();


        
        /**
         * Gets drawable objects for this bullet.
         *
         * @param inCloseRangeParameter a parameter in the range [0,1] to
         *   control the shape/power of the bullet at close range.
         * @param inFarRangeParameter a parameter in the range [0,1] to
         *   control the shape/power of the bullet at far range.
         * @param inPositionInRange the position of the bullet in its
         *   range (in the range [0,1], with 0 being close and 1 being far).
         * @param outPower pointer to where the bullet's power (in the range
         *   [0,1]) should be returned.
         * @param outRotationRate pointer to where the bullet rotation
         *   rate should be returned.
         *
         * @return this bullet as a collection of drawable objects.
         *   Vector and objects must be destroyed by caller.
         */
        SimpleVector<DrawableObject *> *getDrawableObjects(
            double inCloseRangeParameter,
            double inFarRangeParameter,
            double inPositionInRange,
            double *outPower,
            double *outRotationRate );


        
    protected:

        ParameterizedObject *mCloseRangeObject;
        ParameterizedObject *mFarRangeObject;

        
    };



#endif

/*
 * Modification History
 *
 * 2004-October-13   Jason Rohrer
 * Created.
 */



#ifndef PORTAL_MANAGER_INCLUDED
#define PORTAL_MANAGER_INCLUDED



#include "ParameterizedObject.h"

#include "minorGems/math/geometry/Vector3D.h"
#include "minorGems/math/geometry/Angle3D.h"



/**
 * A class that manages and draws all enemies in the environment.
 *
 * @author Jason Rohrer.
 */
class PortalManager {


    public:


        
        /**
         * Constructs a manager.
         *
         * @param inPortalTemplate the class to use to draw the portal.
         *   Will be destroyed by this class.
         * @param inPortalScale the value to multiply portal size by.
         * @param inPortalFadeInTimeSeconds how long it takes this portal
         *   to fade in when it is displayed.
         * @param inGridWidth the width of the grid in screen units.
         */
        PortalManager( ParameterizedObject *inPortalTemplate,
                       double inPortalScale,
                       double inPortalFadeInTimeSeconds,
                       double inGridWidth );


        
        virtual ~PortalManager();

        

        /**
         * Shows the portal at a particular location.
         *
         * @param inPosition the position of the portal.
         *   Will be destroyed by this class.
         */
        void showPortal( Vector3D *inPosition );



        /**
         * Gets whether this portal is visible.
         *
         * @return true if this portal has been shown.
         */
        char isPortalVisible();

        
        
        /**
         * Tell this manager that time has passed.
         *
         * @param inTimeDeltaInSeconds the amount of time that has passed.
         * @param inShipPosition the position of the ship (portal changes
         *   shape as ship moves closer).
         *   Must be destroyed by caller.
         */
        void passTime( double inTimeDeltaInSeconds,
                       Vector3D *inShipPosition );


        /**
         * Gets whether the ship is in the portal.
         *
         * @param inShipPosition the position of the ship.
         *   Must be destroyed by caller.
         *
         * @return true if the ship is in the portal, or false otherwise.
         */
        char isShipInPortal( Vector3D *inShipPosition );

        
        
        /**
         * Gets drawable objects for the portal in its current
         * positions/states.
         *
         * @return portal as a collection of drawable objects.
         *   Vector and objects must be destroyed by caller.
         */
        SimpleVector<DrawableObject *> *getDrawableObjects();


        
    protected:

        ParameterizedObject *mPortalTemplate;

        double mPortalScale;
        double mFadeInTimeSeconds;

        double mFadeFactor;

        double mGridWidth;
        
        // the current portal shape, based on distance of ship
        double mPortalShapeParameter;


        Vector3D *mCurrentPosition;

        double mCurrentRadius;


        // rotation rate can vary across portal's lifespan, so we must
        // keep a current angle for the portal and adjust the angle
        // for each time delta
        double mCurrentRotationRate;
        Angle3D *mCurrentRotation;

    };



#endif

/*
 * Modification History
 *
 * 2004-June-14   Jason Rohrer
 * Created.
 *
 * 2004-June-15   Jason Rohrer
 * Added a function for getting a blended control point.
 *
 * 2004-August-9   Jason Rohrer
 * Made a subclass of ParameterizedSpace.
 */



#ifndef PARAMETERIZED_OBJECT_INCLUDED
#define PARAMETERIZED_OBJECT_INCLUDED



#include "minorGems/math/geometry/Vector3D.h"
#include "minorGems/graphics/Color.h"
#include "minorGems/util/SimpleVector.h"

#include "ParameterizedSpace.h"
#include "DrawableObject.h"
#include "ObjectParameterSpaceControlPoint.h"



/**
 * A 1-D space of object control points.
 *
 * @author Jason Rohrer.
 */
class ParameterizedObject : public ParameterizedSpace {


    public:


        
        /**
         * Constructs an object by reading values from a text file
         * stream.
         *
         * @param inFILE the open file to read from.
         *   Must be closed by caller.
         * @param outError pointer to where error flag should be returned.
         *   Destination will be set to true if reading the object
         *   from inFILE fails.
         */
        ParameterizedObject( FILE *inFILE, char *outError );



        /**
         * Gets drawable objects from this object space.
         *
         * @param inParameter the parameter in the range [0,1] to map
         *   into the object space.
         * @param outRotationRate pointer to where the mapped rotation
         *   rate should be returned.
         *
         * @return this object as a collection of drawable objects.
         *   Can return NULL if this space was not properly initialized.
         *   Vector and objects must be destroyed by caller.
         */
        SimpleVector<DrawableObject *> *getDrawableObjects(
            double inParameter, double *outRotationRate );

        

        /**
         * Gets a blended object control point from this object space.
         *
         * @param inParameter the parameter in the range [0,1] to map
         *   into the object space.
         *
         * @return the blended control point.
         *   Can return NULL if this space was not properly initialized.
         *   Must be destroyed by caller.
         */
        ObjectParameterSpaceControlPoint *getBlendedControlPoint(
            double inParameter );


        
    protected:

        // inherit all protected members from ParameterizedSpace
        
        
    };



#endif

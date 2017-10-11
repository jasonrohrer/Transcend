/*
 * Modification History
 *
 * 2004-August-9   Jason Rohrer
 * Created.
 */



#ifndef PARAMETERIZED_SPACE_INCLUDED
#define PARAMETERIZED_SPACE_INCLUDED



#include "minorGems/util/SimpleVector.h"

#include "ParameterSpaceControlPoint.h"



/**
 * A 1-D space of control points.
 *
 * @author Jason Rohrer.
 */
class ParameterizedSpace {


    public:
        
        
        virtual ~ParameterizedSpace();

        

        /**
         * Gets a blended control point from this space space.
         *
         * @param inParameter the parameter in the range [0,1] to map
         *   into the space.
         *
         * @return the blended control point.
         *   Can return NULL if this space was not properly initialized.
         *   Must be destroyed by caller.
         */
        ParameterSpaceControlPoint *getBlendedControlPoint(
            double inParameter );


        
    protected:

        int mNumControlPoints;

        double *mControlPointParameterAnchors;

        ParameterSpaceControlPoint **mControlPoints;

        
        
    };



#endif

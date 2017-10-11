/*
 * Modification History
 *
 * 2004-August-9   Jason Rohrer
 * Created.
 */



#ifndef PARAMETER_SPACE_CONTROL_POINT_INCLUDED
#define PARAMETER_SPACE_CONTROL_POINT_INCLUDED



#include "minorGems/math/geometry/Vector3D.h"
#include "minorGems/util/SimpleVector.h"



/**
 * A control point in a 1-D parameterized object space.
 *
 * @author Jason Rohrer.
 */
class ParameterSpaceControlPoint {


    public:
        

        
        virtual ~ParameterSpaceControlPoint();


        
        /**
         * Makes a deep copy of this control point.
         *
         * @return a copy of this point.
         *   Must be destroyed by caller.
         */
        virtual ParameterSpaceControlPoint *copy() = 0;
        
        

        /**
         * Creates a new control point by blending this point with another
         * point.
         *
         * @param inOtherPoint the other control point.
         *   Must be destroyed by caller.
         * @param inWeightOfOtherPoint the weight of the other point,
         *   in the range [0,1].
         *
         * @return the new, blended point.
         *   Must be destroyed by caller.
         */
        virtual ParameterSpaceControlPoint *createLinearBlend(
            ParameterSpaceControlPoint *inOtherPoint,
            double inWeightOfOtherPoint ) = 0;


        
    protected:


        
        /**
         * Blends two vertex arrays.
         *
         * @param inFirstArray the first array of vertices.
         *   Vertices and array must be destroyed by caller.
         * @param inFirstArrayLength the number of vertices in the first array.
         * @param inWeightFirstArray the weight of the first array, in [0,1].
         * @param inSecondArray the first array of vertices.
         *   Vertices and array must be destroyed by caller.
         * @param inSecondArrayLength the number of vertices in the first array.
         * @param outResultLength pointer to where the length of the resulting
         *   array should be returned.
         *
         * @return the blended array.
         *   Vertices and array must be destroyed by caller.
         */
        static Vector3D **blendVertexArrays(
            Vector3D **inFirstArray,
            int inFirstArrayLength,
            double inWeightFirstArray,
            Vector3D **inSecondArray,
            int inSecondArrayLength,
            int *outResultLength ); 
        

        
    };



#endif

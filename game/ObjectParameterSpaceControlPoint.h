/*
 * Modification History
 *
 * 2004-June-12   Jason Rohrer
 * Created.
 *
 * 2004-June-15   Jason Rohrer
 * Added a copy function.
 *
 * 2004-June-18   Jason Rohrer
 * Changed to use triangles instead of polygons.
 *
 * 2004-August-9   Jason Rohrer
 * Made a subclass of ParameterSpaceControlPoint.
 *
 * 2004-August-25   Jason Rohrer
 * Made readColorFromFile static and public so that other classes can use it.
 * 
 * 2004-August-26   Jason Rohrer
 * Added scaling of rotated copies to support spiral shapes.
 * 
 * 2004-August-27   Jason Rohrer
 * Added function for writing out to file.
 * 
 * 2004-August-29   Jason Rohrer
 * Added a scale factor for the angle of rotated copies.
 */



#ifndef OBJECT_PARAMETER_SPACE_CONTROL_POINT_INCLUDED
#define OBJECT_PARAMETER_SPACE_CONTROL_POINT_INCLUDED



#include "minorGems/math/geometry/Vector3D.h"
#include "minorGems/graphics/Color.h"
#include "minorGems/util/SimpleVector.h"

#include "ParameterSpaceControlPoint.h"
#include "DrawableObject.h"



/**
 * A control point in a 1-D parameterized object space.
 *
 * @author Jason Rohrer.
 */
class ObjectParameterSpaceControlPoint : public ParameterSpaceControlPoint {


    public:


        
        /**
         * Constructs a control point.
         * 
         * @param inNumTriangleVertices the number of triangle vertices.
         *   Must be a multiple of 3.
         * @param inTriangleVertices the triangle vertices.
         *   This array and the vertices it contains will be
         *   destroyed when this class is destroyed.
         * @param inTriangleVertexFillColors a fill color for each
         *   triangle vertex.
         *   This array and the colors it contains will be destroyed when
         *   this class is destroyed.
         * @param inNumBorderVertices the number of vertices.
         *   Must be a multiple of 3.
         * @param inBorderVertices the border vertices.
         *   This array and the vertices it contains will be
         *   destroyed when this class is destroyed.
         * @param inBorderVertexColors a color for each
         *   border vertex.
         *   This array and the colors it contains will be destroyed when
         *   this class is destroyed.
         * @param inBorderWidth the width of the border, in pixels.
         * @param inNumRotatedCopies the number of rotated copies of the
         *   base vertices to draw (evenly spaced rotations).
         *   Non-integer values will cause angle between copies to
         *   be smaller with an extra copy "fading in" as the fractional
         *   part of inNumRotatedCopies approaches 1.
         * @param inRotatedCopyScaleFactor the factor to scale each successive
         *   rotated copy by.  Values less than one will cause rotated copies
         *   to get progressively smaller, while values larger than
         *   one will cause copies to get larger.
         * @param inRotatedCopyAngleScaleFactor the factor to scale the
         *   rotation angle by.  If inRotatedCopyAngleScaleFactor is 1, the
         *   copies will be evenly spaced around a circle.  For values less
         *   than 1, copies will be closer together and will not fill a circle.
         *   For values greater than 1, copies will be spread around a
         *   circle more than once and will overlap.
         * @param inRotationRate the rate in rotations per second.
         */
        ObjectParameterSpaceControlPoint(
            int inNumTriangleVertices, Vector3D **inTriangleVertices,
            Color **inTriangleVertexFillColors,
            int inNumBorderVertices, Vector3D **inBorderVertices,
            Color **inBorderVertextColors,
            double inBorderWidth,
            double inNumRotatedCopies,
            double inRotatedCopyScaleFactor,
            double inRotatedCopyAngleScaleFactor,
            double inRotationRate );
        
        

        /**
         * Constructs a control point by reading values from a text file
         * stream.
         *
         * @param inFILE the open file to read from.
         *   Must be closed by caller.
         * @param outError pointer to where error flag should be returned.
         *   Destination will be set to true if reading a control point
         *   from inFILE fails.
         */
        ObjectParameterSpaceControlPoint( FILE *inFILE, char *outError );
        

        
        virtual ~ObjectParameterSpaceControlPoint();


        
        /**
         * Writes this control point out to a text file stream.
         *
         * @param inFILE the open file to write to.
         *   Must be closed by caller.
         */
        void writeToFile( FILE *inFILE );
        
        
        
        // implements the ParameterSpaceControlPoint interface
        ParameterSpaceControlPoint *copy();

        ParameterSpaceControlPoint *createLinearBlend(
            ParameterSpaceControlPoint *inOtherPoint,
            double inWeightOfOtherPoint );



        /**
         * Gets drawable objects from this control point.
         *
         * @return this control point as a collection of drawable objects.
         *   Vector and objects must be destroyed by caller.
         */
        SimpleVector<DrawableObject *> *getDrawableObjects();

        

        /**
         * Gets the rotation rate of this point.
         *
         * @return the rotation rate in rotations per second.
         */
        double getRotationRate();


        
        // These internal members are public only to allow this control
        // point to be blended with other control points.
        
        // These members should not be accessed by other classes.

        int mNumTriangleVertices;
        Vector3D **mTriangleVertices;
        Color **mTriangleVertexFillColors;

        int mNumBorderVertices;
        Vector3D **mBorderVertices;
        Color **mBorderVertexColors;

        double mBorderWidth;

        double mNumRotatedCopies;
        double mRotatedCopyScaleFactor;
        double mRotatedCopyAngleScaleFactor;
        double mRotationRate;

        

        /**
         * Reads a color from a text file as either RGBA or as a named
         * color.
         *
         * @param inFILE the file stream to read from.
         *   Must be closed by caller.
         *
         * @return the read color, or NULL on an error.
         *   Must be destroyed by caller.
         */
        static Color *readColorFromFile( FILE *inFILE );



        /**
         * Writes a color to a text file as space-delimited RGBA.
         *
         * @param inFILE the file stream to read from.
         *   Must be closed by caller.
         * @param inColor the color to write.
         *   Must be destroyed by caller.
         */
        static void writeColorToFile( FILE *inFILE, Color *inColor );

        

    protected:

        
        
        /**
         * Makes a deep copy of an array of vertices.
         *
         * @param inArray the array to duplicate.
         *   Array and vertices must be destroyed by caller.
         * @param inNumVertices the number of vertices in the array.
         *
         * @return the new array.
         *   Array and vertices must be destroyed by caller.
         */
        Vector3D **duplicateVertextArray( Vector3D **inArray,
                                          int inNumVertices );


        /**
         * Makes a deep copy of an array of colors.
         *
         * @param inArray the array to duplicate.
         *   Array and colorsmust be destroyed by caller.
         * @param inNumColors the number of colors in the array.
         * @param inAlphaMultiplier a factor to multiply the alpha
         *   channel of each color by when making the copy.
         *   Defaults to 1 (no change to alphas in copied array).
         *   
         *
         * @return the new array.
         *   Array and colors must be destroyed by caller.
         */
        Color **duplicateColorArray( Color **inArray,
                                     int inNumColors,
                                     float inAlphaMultiplier = 1 );



        /**
         * Blends two color arrays.
         *
         * @param inFirstArray the first array of colors.
         *   Colors and array must be destroyed by caller.
         * @param inFirstArrayLength the number of colors in the first array.
         * @param inWeightFirstArray the weight of the first array, in [0,1].
         * @param inSecondArray the first array of colors.
         *   Colors and array must be destroyed by caller.
         * @param inSecondArrayLength the number of colors in the first array.
         * @param outResultLength pointer to where the length of the resulting
         *   array should be returned.
         *
         * @return the blended array.
         *   Colors and array must be destroyed by caller.
         */
        Color **blendColorArrays( Color **inFirstArray,
                                  int inFirstArrayLength,
                                  double inWeightFirstArray,
                                  Color **inSecondArray,
                                  int inSecondArrayLength,
                                  int *outResultLength );

        

        

        
    };



#endif

/*
 * Modification History
 *
 * 2004-June-11   Jason Rohrer
 * Created.
 *
 * 2004-June-16   Jason Rohrer
 * Added fuctions for modifying object position, rotation, and transparency.
 *
 * 2004-June-18   Jason Rohrer
 * Changed to use triangles instead of polygons.
 *
 * 2004-June-20   Jason Rohrer
 * Added fuction for modifying scale.
 * Added function for testing circle containment.
 *
 * 2004-June-21   Jason Rohrer
 * Added fuction for getting minimum border distance.
 */



#ifndef DRAWABLE_OBJECT_INCLUDED
#define DRAWABLE_OBJECT_INCLUDED



#include "minorGems/math/geometry/Vector3D.h"
#include "minorGems/math/geometry/Angle3D.h"
#include "minorGems/graphics/Color.h"



/**
 * A 2d object that can draw itself into the current OpenGL context.
 *
 * @author Jason Rohrer.
 */
class DrawableObject {


    public:


        
        /**
         * Constructs an object.
         *
         * @param inNumTriangleVertices the number of triangle vertices.
         *   Must be a multiple of 3.
         * @param inTriangleVertices the triangle vertices.
         *   Only the x and y components have an effect.  This array and
         *   the vertices it contains will be destroyed when this class is
         *   destroyed.
         * @param inTriangleVertexFillColors a fill color for each triangle
         *   vertex.
         *   This array and the colors it contains will be destroyed when
         *   this class is destroyed.
         * @param inNumBorderVertices the number of border vertices.
         * @param inBorderVertices the borer vertices.
         *   Only the x and y components have an effect.  This array and
         *   the vertices it contains will be destroyed when this class is
         *   destroyed.
         * @param inVertextBorderColors a border color for each vertex.
         *   This array and the colors it contains will be destroyed when
         *   this class is destroyed.
         * @param inBorderWidth the width of the border, in pixels.
         */
        DrawableObject( int inNumTriangleVertices,
                        Vector3D **inTriangleVertices,
                        Color **inTriangleVertexFillColors,
                        int inNumBorderVertices,
                        Vector3D **inBorderVertices,
                        Color **inBorderVertextColors,
                        float inBorderWidth );

        

        virtual ~DrawableObject();


        
        /**
         * Rotates this object.
         *
         * @param inRotation the angle to rotate it by.
         *   Must be destroyed by caller.
         */
        void rotate( Angle3D *inRotation );


        
        /**
         * Moves this object.
         *
         * @param inPosition the vector to move it by.
         *   Must be destroyed by caller.
         */
        void move( Vector3D *inPosition );



        /**
         * Scales this object.
         *
         * @param inScale the multiplier to scale it by.
         */
        void scale( double inScale );


        
        /**
         * Fades the colors of this object (makes them more transparent).
         *
         * @param inAlphaScale the value, in the range [0,1] to multiply
         *   alpha values by.
         */
        void fade( double inAlphaScale );


        
        /**
         * Tests whether any of this object's border vertices are in a circle.
         *
         * @param inCenter the center of the circle.
         *   Must be destroyed by caller.
         * @param inRadius the radius of the circle.
         */
        char isBorderInCircle( Vector3D *inCenter, double inRadius );


        
        /**
         * Gets the maximum distance of the border vertices from a given
         * point.
         *
         * @param inPoint the point to get the distance from.
         *   Must be destroyed by caller.
         */
        double getBorderMaxDistance( Vector3D *inPoint );



        /**
         * Gets the minimum distance of the border vertices from a given
         * point.
         *
         * @param inPoint the point to get the distance from.
         *   Must be destroyed by caller.
         */
        double getBorderMinDistance( Vector3D *inPoint );

        
        
        /**
         * Draws this object into the current OpenGL context.
         *
         * @param inScale the scale factor.
         * @param inRotation the rotation of the object.
         *   Must be destroyed by caller.
         * @param inPosition the position of the object.
         *   Must be destroyed by caller.
         */
        void draw( double inScale, Angle3D *inRotation, Vector3D *inPosition );

        
        
    protected:
        int mNumTriangleVertices;
        Vector3D **mTriangleVertices;
        Color **mTriangleVertexFillColors;

        int mNumBorderVertices;
        Vector3D **mBorderVertices;
        Color **mBorderVertexColors;

        float mBorderWidth;

        
        
    };



#endif

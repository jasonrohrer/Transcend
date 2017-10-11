/*
 * Modification History
 *
 * 2004-June-14   Jason Rohrer
 * Created.
 *
 * 2004-June-15   Jason Rohrer
 * Added a function for getting a blended control point.
 *
 * 2004-June-22   Jason Rohrer
 * Fixed algorithmic errors in linear blend function.
 */



#include "ParameterizedObject.h"



ParameterizedObject::ParameterizedObject( FILE *inFILE, char *outError ) {

    SimpleVector<ParameterSpaceControlPoint *> *controlPoints =
        new SimpleVector<ParameterSpaceControlPoint*>();
    SimpleVector<double> *controlPointParameterAnchors =
        new SimpleVector<double>();

    // keep reading parameter anchors and control points until we
    // can read no more
    
    char readError = false;

    while( !readError ) {
        
        // read the parameter space anchor
        double anchor = 0;
        int numRead = fscanf( inFILE, "%lf", &anchor );

        if( numRead != 1 ) {
            readError = true;
            }
        else {

            // read the control point
            ObjectParameterSpaceControlPoint *point =
                new ObjectParameterSpaceControlPoint( inFILE,
                                                      &readError );

            if( !readError ) {
                controlPointParameterAnchors->push_back( anchor );
                controlPoints->push_back( point );
                }
            else {
                delete point;
                }
            }
        }

    mNumControlPoints = controlPoints->size();
    mControlPoints = controlPoints->getElementArray();
    mControlPointParameterAnchors =
        controlPointParameterAnchors->getElementArray();

    delete controlPoints;
    delete controlPointParameterAnchors;

    if( mNumControlPoints >= 2 ) {
        *outError = false;
        }
    else {
        // we didn't read enough control points
        *outError = true;
        }
    }



SimpleVector<DrawableObject *> *ParameterizedObject::getDrawableObjects(
    double inParameter, double *outRotationRate ) {

    // blend the two points, using the distance to weight them
    ObjectParameterSpaceControlPoint *blendedPoint =
        getBlendedControlPoint( inParameter );
    
    if( blendedPoint != NULL ) {
        SimpleVector<DrawableObject*> *drawableObjects =
            blendedPoint->getDrawableObjects();
        
        *outRotationRate = blendedPoint->getRotationRate();
        
        delete blendedPoint;
        
        return drawableObjects;
        }
    else {
        printf( "Error:  no control points in object space.\n" );

        *outRotationRate = 0;

        return NULL;
        }

    }



ObjectParameterSpaceControlPoint *ParameterizedObject::getBlendedControlPoint(
    double inParameter ) {

    // cast result of super-class function call and return it
    return (ObjectParameterSpaceControlPoint*)
        ParameterizedSpace::getBlendedControlPoint( inParameter );

    }

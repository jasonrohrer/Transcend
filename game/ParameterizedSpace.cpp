/*
 * Modification History
 *
 * 2004-August-9   Jason Rohrer
 * Created.
 */



#include "ParameterizedSpace.h"


#include <math.h>



ParameterizedSpace::~ParameterizedSpace() {

    for( int i=0; i<mNumControlPoints; i++ ) {
        delete mControlPoints[i];
        }

    delete [] mControlPoints;
    delete [] mControlPointParameterAnchors;
    }



ParameterSpaceControlPoint *ParameterizedSpace::getBlendedControlPoint(
    double inParameter ) {


       
    if( mNumControlPoints >= 2 ) {
        // find the 2 surrounding control points

        // all distances will be no more than 1
        double distanceFromClosestLargerPoint = 2;
        double distanceFromClosestSmallerPoint = 3;
        
        int indexOfClosestLargerPoint = -1;
        int indexOfClosestSmallerPoint = -1;

        for( int i=0; i<mNumControlPoints; i++ ) {

            double distanceFromThisPoint =
                fabs( mControlPointParameterAnchors[i] - inParameter );  

            if( mControlPointParameterAnchors[i] >= inParameter ) {
                // a larger point
                if( distanceFromThisPoint < distanceFromClosestLargerPoint ) {
                    // closer than our closest largerpoint
                    
                    indexOfClosestLargerPoint = i;
                    distanceFromClosestLargerPoint = distanceFromThisPoint;
                    }
                }
            else {
                // a smaller point
                if( distanceFromThisPoint < distanceFromClosestSmallerPoint ) {
                    // closer than our closest smallerpoint

                    indexOfClosestSmallerPoint = i;
                    distanceFromClosestSmallerPoint = distanceFromThisPoint;
                    }
                }
            }
        if( indexOfClosestLargerPoint != -1  &&
            indexOfClosestSmallerPoint != -1 ) {
            // found two points
            
            // compute weights
            double distanceBetweenSurroundingPoints =
                fabs( mControlPointParameterAnchors[
                          indexOfClosestLargerPoint ] -
                      mControlPointParameterAnchors[
                          indexOfClosestSmallerPoint ] );

            double weightOnLargerPoint =
                distanceFromClosestSmallerPoint /
                distanceBetweenSurroundingPoints;


            // blend the two points, using the distance to weight them
            return 
                mControlPoints[ indexOfClosestSmallerPoint ]->
                    createLinearBlend(
                        mControlPoints[ indexOfClosestLargerPoint ],
                        weightOnLargerPoint );
            }
        else {
            // found only one point

            int indexOfPoint;
            
            if( indexOfClosestLargerPoint != -1 ) {
                indexOfPoint = indexOfClosestLargerPoint;
                }
            else if( indexOfClosestLargerPoint != -1 ) {
                indexOfPoint = indexOfClosestSmallerPoint;
                }
            else {
                printf( "Error:  found no closest space parameter map "
                        " anchor point.\n" );
                indexOfPoint = 0;
                }

            // return copy of this point
            return mControlPoints[ indexOfPoint ]->copy();
            }
        }
    else if( mNumControlPoints == 1 ) {
        // only one anchor point... return its parameters
        
        return mControlPoints[0]->copy();
        }
    else {
        printf( "Error:  no anchor points in sculpture parameter space.\n" );

        return NULL;
        }

    /*
    if( mNumControlPoints >= 2 ) {
        // find the 2 closest control points

        // all distances will be no more than 1
        double distanceFromClosestPoint = 2;
        double distanceFromNextClosestPoint = 3;
        
        int indexOfClosestPoint = -1;
        int indexOfNextClosestPoint = -1;

        for( int i=0; i<mNumControlPoints; i++ ) {

            double distanceFromThisPoint =
                fabs( mControlPointParameterAnchors[i] - inParameter );  
            
            if( distanceFromThisPoint < distanceFromClosestPoint ) {
                // closer than our closest point

                indexOfNextClosestPoint = indexOfClosestPoint;
                distanceFromNextClosestPoint = distanceFromClosestPoint;

                indexOfClosestPoint = i;
                distanceFromClosestPoint = distanceFromThisPoint;
                }
            else if( distanceFromThisPoint < distanceFromNextClosestPoint ) {
                // not as close as our closest point, but closer than
                // our next closest point

                indexOfNextClosestPoint = i;
                distanceFromNextClosestPoint = distanceFromThisPoint;
                }
            }

        // scale the distance from our parameter to the closest point
        // by the distance between the two closest points to get a weight
        // for the closest point
        double distanceBetweenClosestPoints =
            fabs( mControlPointParameterAnchors[ indexOfClosestPoint ] -
                  mControlPointParameterAnchors[ indexOfNextClosestPoint ] );

        double weightOnClosestPoint =
            1 - ( distanceFromClosestPoint / distanceBetweenClosestPoints );


        // blend the two points, using the distance to weight them
        return 
            mControlPoints[ indexOfNextClosestPoint ]->createLinearBlend(
                mControlPoints[ indexOfClosestPoint ],
                weightOnClosestPoint );
        
        }
    else if( mNumControlPoints == 1 ) {
        // only one control point... return a copy
        
        return mControlPoints[0]->copy();
        }
    else {
        printf( "Error:  no control points in space space.\n" );

        return NULL;
        }
    */
    }

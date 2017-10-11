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
 * 2004-August-12   Jason Rohrer
 * Optimized Color constructor.
 * Optimized blendColorArrays function.
 * 
 * 2004-August-26   Jason Rohrer
 * Added scaling of rotated copies to support spiral shapes.
 * 
 * 2004-August-27   Jason Rohrer
 * Added function for writing out to file.
 * 
 * 2004-August-29   Jason Rohrer
 * Added a scale factor for the angle of rotated copies.
 *
 * 2004-August-30   Jason Rohrer
 * Optimization:  avoid object blending whenever possible.
 */



#include "ObjectParameterSpaceControlPoint.h"

#include "NamedColorFactory.h"



#include <math.h>



ObjectParameterSpaceControlPoint::ObjectParameterSpaceControlPoint(
    int inNumTriangleVertices, Vector3D **inTriangleVertices,
    Color **inTriangleVertexFillColors,
    int inNumBorderVertices, Vector3D **inBorderVertices,
    Color **inBorderVertexColors,
    double inBorderWidth,
    double inNumRotatedCopies,
    double inRotatedCopyScaleFactor,
    double inRotatedCopyAngleScaleFactor,
    double inRotationRate )
    : mNumTriangleVertices( inNumTriangleVertices ),
      mTriangleVertices( inTriangleVertices ),
      mTriangleVertexFillColors( inTriangleVertexFillColors ),
      mNumBorderVertices( inNumBorderVertices ),
      mBorderVertices( inBorderVertices ),
      mBorderVertexColors( inBorderVertexColors ),
      mBorderWidth( inBorderWidth ),
      mNumRotatedCopies( inNumRotatedCopies ),
      mRotatedCopyScaleFactor( inRotatedCopyScaleFactor ),
      mRotatedCopyAngleScaleFactor( inRotatedCopyAngleScaleFactor ),
      mRotationRate( inRotationRate ) {

    }



ObjectParameterSpaceControlPoint::ObjectParameterSpaceControlPoint(
    FILE *inFILE, char *outError ) {

    int totalNumRead = 0;
    
    mNumTriangleVertices = 0;
    totalNumRead += fscanf( inFILE, "%d", &mNumTriangleVertices );

    // how many values should we successfully read (cheap error checking)
    int totalToRead =
        1 +                 // read the number of triangle vertices
        mNumTriangleVertices * ( 2 + 1); // each vertex, read x, y, and 1
                                         // color
        
    
    mTriangleVertices = new Vector3D*[ mNumTriangleVertices ];
    mTriangleVertexFillColors = new Color*[ mNumTriangleVertices ];

    // read coordinates and colors for each vertex
    for( int i=0; i<mNumTriangleVertices; i++ ) {
        double x, y;

        totalNumRead += fscanf( inFILE, "%lf", &x );
        totalNumRead += fscanf( inFILE, "%lf", &y );

        mTriangleVertices[i] = new Vector3D( x, y, 0 );

        mTriangleVertexFillColors[i] = readColorFromFile( inFILE );

        if( mTriangleVertexFillColors[i] != NULL ) {
            totalNumRead += 1;
            }
        else {
            // fill with dummy color
            mTriangleVertexFillColors[i] = new Color( 1, 1, 1, 1, false );
            }
        }


    mNumBorderVertices = 0;
    totalNumRead += fscanf( inFILE, "%d", &mNumBorderVertices );

    totalToRead +=
        1 +           // read the number of border vertices
        mNumBorderVertices * ( 2 + 1); // each border vertex, read x, y,
                                        // and 1 color

    mBorderVertices = new Vector3D*[ mNumBorderVertices ];
    mBorderVertexColors = new Color*[ mNumBorderVertices ];

    // read coordinates and colors for each vertex
    for( int i=0; i<mNumBorderVertices; i++ ) {
        double x, y;

        totalNumRead += fscanf( inFILE, "%lf", &x );
        totalNumRead += fscanf( inFILE, "%lf", &y );

        mBorderVertices[i] = new Vector3D( x, y, 0 );

        mBorderVertexColors[i] = readColorFromFile( inFILE );

        if( mBorderVertexColors[i] != NULL ) {
            totalNumRead += 1;
            }
        else {
            // fill with dummy color
            mBorderVertexColors[i] = new Color( 1, 1, 1, 1, false );
            }

        }


    

    
    totalToRead += 5;   // border width, rotated copies, scale factor, angle
                        // scale factor, and rotation rate 
        
    
    totalNumRead += fscanf( inFILE, "%lf", &mBorderWidth );
    totalNumRead += fscanf( inFILE, "%lf", &mNumRotatedCopies );
    totalNumRead += fscanf( inFILE, "%lf", &mRotatedCopyScaleFactor );
    totalNumRead += fscanf( inFILE, "%lf", &mRotatedCopyAngleScaleFactor );
    totalNumRead += fscanf( inFILE, "%lf", &mRotationRate );

    
    if( totalNumRead != totalToRead ) {
        printf( "Expecting to read %d values, but read %d\n",
                totalToRead, totalNumRead );
        *outError = true;
        }
    else {
        *outError = false;
        }
    }
        

        
ObjectParameterSpaceControlPoint::~ObjectParameterSpaceControlPoint() {

    int i;
    for( i=0; i<mNumTriangleVertices; i++ ) {
        delete mTriangleVertices[i];
        delete mTriangleVertexFillColors[i];
        }
    for( i=0; i<mNumBorderVertices; i++ ) {
        delete mBorderVertices[i];
        delete mBorderVertexColors[i];
        }
    
    delete [] mTriangleVertices;
    delete [] mTriangleVertexFillColors;
    delete [] mBorderVertices;
    delete [] mBorderVertexColors;
    }



void ObjectParameterSpaceControlPoint::writeToFile( FILE *inFILE ) {
    fprintf( inFILE, "%d\n\n", mNumTriangleVertices );


    // write coordinates and colors for each vertex
    for( int i=0; i<mNumTriangleVertices; i++ ) {
        
        fprintf( inFILE, "%f ", mTriangleVertices[i]->mX );
        fprintf( inFILE, "%f\n", mTriangleVertices[i]->mY );

        writeColorToFile( inFILE, mTriangleVertexFillColors[i] );
        }

    fprintf( inFILE, "\n" );

    fprintf( inFILE, "%d\n\n", mNumBorderVertices );

    // read coordinates and colors for each vertex
    for( int i=0; i<mNumBorderVertices; i++ ) {
        fprintf( inFILE, "%f ", mBorderVertices[i]->mX );
        fprintf( inFILE, "%f\n", mBorderVertices[i]->mY );

        writeColorToFile( inFILE, mBorderVertexColors[i] );
        }


    fprintf( inFILE, "\n" );
    
    fprintf( inFILE, "%f\n", mBorderWidth );
    fprintf( inFILE, "%f\n", mNumRotatedCopies );
    fprintf( inFILE, "%f\n", mRotatedCopyScaleFactor );
    fprintf( inFILE, "%f\n", mRotatedCopyAngleScaleFactor );
    fprintf( inFILE, "%f\n", mRotationRate );
    }



ParameterSpaceControlPoint *ObjectParameterSpaceControlPoint::copy() {

    return new ObjectParameterSpaceControlPoint (
        mNumTriangleVertices,
        duplicateVertextArray( mTriangleVertices, mNumTriangleVertices ),
        duplicateColorArray( mTriangleVertexFillColors, mNumTriangleVertices ),
        mNumBorderVertices,
        duplicateVertextArray( mBorderVertices, mNumBorderVertices ),
        duplicateColorArray( mBorderVertexColors, mNumBorderVertices ),
        mBorderWidth,
        mNumRotatedCopies, mRotatedCopyScaleFactor,
        mRotatedCopyAngleScaleFactor,
        mRotationRate );
    }



ParameterSpaceControlPoint *
ObjectParameterSpaceControlPoint::createLinearBlend(
    ParameterSpaceControlPoint *inOtherPoint,
    double inWeightOfOtherPoint ) {

    // cast
    ObjectParameterSpaceControlPoint *otherPoint =
        (ObjectParameterSpaceControlPoint*)inOtherPoint;

    // skip blending if we are at either end of our range
    if( inWeightOfOtherPoint == 0 ) {
        return this->copy();
        }
    else if( inWeightOfOtherPoint == 1 ) {
        return otherPoint->copy();
        }
    
    double weightOfThisPoint = 1 - inWeightOfOtherPoint;


    int numBlendTriangleVertices;
    Vector3D **blendTriangleVertices =
        blendVertexArrays( mTriangleVertices,
                           mNumTriangleVertices,
                           weightOfThisPoint,
                           otherPoint->mTriangleVertices,
                           otherPoint->mNumTriangleVertices,
                           &numBlendTriangleVertices );

    int numBlendTriangleVertexFillColors;
    Color **blendTriangleVertexFillColors =
        blendColorArrays( mTriangleVertexFillColors,
                          mNumTriangleVertices,
                          weightOfThisPoint,
                          otherPoint->mTriangleVertexFillColors,
                          otherPoint->mNumTriangleVertices,
                          &numBlendTriangleVertexFillColors );

    if( numBlendTriangleVertexFillColors != numBlendTriangleVertices ) {
        printf( "Error in ObjectParameterSpaceControlPoint:\n"
                "   Number of blended triangle colors does not match number\n"
                "   of blended vertices.\n" );
        }

    int numBlendBorderVertices;
    Vector3D **blendBorderVertices =
        blendVertexArrays( mBorderVertices,
                           mNumBorderVertices,
                           weightOfThisPoint,
                           otherPoint->mBorderVertices,
                           otherPoint->mNumBorderVertices,
                           &numBlendBorderVertices );

    int numBlendBorderVertexColors;
    Color **blendBorderVertexColors =
        blendColorArrays( mBorderVertexColors,
                          mNumBorderVertices,
                          weightOfThisPoint,
                          otherPoint->mBorderVertexColors,
                          otherPoint->mNumBorderVertices,
                          &numBlendBorderVertexColors );
    
    if( numBlendBorderVertexColors != numBlendBorderVertices ) {
        printf( "Error in ObjectParameterSpaceControlPoint:\n"
                "   Number of blended border colors does not match number of\n"
                "   blended vertices.\n" );
        }
    
    double blendBorderWidth =
        inWeightOfOtherPoint * otherPoint->mBorderWidth +
        weightOfThisPoint * mBorderWidth;
    
    double blendNumRotatedCopies =
        inWeightOfOtherPoint * otherPoint->mNumRotatedCopies +
        weightOfThisPoint * mNumRotatedCopies;

    double blendRotatedCopyScaleFactor =
        inWeightOfOtherPoint * otherPoint->mRotatedCopyScaleFactor +
        weightOfThisPoint * mRotatedCopyScaleFactor;

    double blendRotatedCopyAngleScaleFactor =
        inWeightOfOtherPoint * otherPoint->mRotatedCopyAngleScaleFactor +
        weightOfThisPoint * mRotatedCopyAngleScaleFactor;

    double blendRotationRate =
        inWeightOfOtherPoint * otherPoint->mRotationRate +
        weightOfThisPoint * mRotationRate;

    return new ObjectParameterSpaceControlPoint(
        numBlendTriangleVertices,
        blendTriangleVertices,
        blendTriangleVertexFillColors,
        numBlendBorderVertices,
        blendBorderVertices,
        blendBorderVertexColors,
        blendBorderWidth,
        blendNumRotatedCopies,
        blendRotatedCopyScaleFactor,
        blendRotatedCopyAngleScaleFactor,
        blendRotationRate );
    }



SimpleVector<DrawableObject *> *
ObjectParameterSpaceControlPoint::getDrawableObjects() {

    SimpleVector<DrawableObject *> *returnVector =
        new SimpleVector<DrawableObject *>();

    Angle3D *angleBetweenRotatedCopies =
        new Angle3D( 0, 0,
                     2 * M_PI / ( mNumRotatedCopies + 1 ) );

    angleBetweenRotatedCopies->scale( mRotatedCopyAngleScaleFactor );
    
    // start out with a copy of the vertices and rotate them,
    // bit by bit, to create each reflection
    Vector3D **workingTriangleVertices =
        duplicateVertextArray( mTriangleVertices,
                               mNumTriangleVertices );
    Vector3D **workingBorderVertices =
        duplicateVertextArray( mBorderVertices,
                               mNumBorderVertices );

    
    // if we have a non-integral number of reflections, draw one
    // extra reflection (it will overlap another of the reflections,
    // but this eliminates reflection "pop-in" when transitioning between two
    // control points that have a different number of reflections
    int numRotatedCopiesToDraw = (int)ceil( mNumRotatedCopies );

    char drawingExtraReflection = false;

    if( numRotatedCopiesToDraw - mNumRotatedCopies > 0 ) {
        drawingExtraReflection = true;
        }
    
    for( int s=0; s<=numRotatedCopiesToDraw; s++ ) {

        Color **reflectedFillColors;
        Color **reflectedBorderColors;

        if( drawingExtraReflection && s == numRotatedCopiesToDraw ) {
            // this is our extra reflection
            // alpha-fade it in based on the fractional part of our number
            // of reflections

            double fractionalPart =
                mNumRotatedCopies -
                floor( mNumRotatedCopies );

            reflectedFillColors =
                duplicateColorArray( mTriangleVertexFillColors,
                                     mNumTriangleVertices,
                                     (float)fractionalPart );
                                                       
            reflectedBorderColors =
                duplicateColorArray( mBorderVertexColors,
                                     mNumBorderVertices,
                                     (float)fractionalPart );
            }
        else {
            // not an extra reflection
            // do not alpha-fade it at all
            reflectedFillColors = duplicateColorArray(
                mTriangleVertexFillColors,
                mNumTriangleVertices );
                                                       
            reflectedBorderColors = duplicateColorArray(
                mBorderVertexColors,
                mNumBorderVertices );
            }
        
        DrawableObject *reflectedObject =
            new DrawableObject( mNumTriangleVertices,
                                duplicateVertextArray( workingTriangleVertices,
                                                       mNumTriangleVertices ),
                                reflectedFillColors,
                                mNumBorderVertices,
                                duplicateVertextArray( workingBorderVertices,
                                                       mNumBorderVertices ),
                                reflectedBorderColors,
                                mBorderWidth );

        returnVector->push_back( reflectedObject );

        // rotate the working vertices by one reflection angle
        int i;
        for( i=0; i<mNumTriangleVertices; i++ ) {
            workingTriangleVertices[i]->rotate( angleBetweenRotatedCopies );
            workingTriangleVertices[i]->scale( mRotatedCopyScaleFactor );
            }
        for( i=0; i<mNumBorderVertices; i++ ) {
            workingBorderVertices[i]->rotate( angleBetweenRotatedCopies );
            workingBorderVertices[i]->scale( mRotatedCopyScaleFactor );
            }

        }

    // delete the working vertices
    int i;
    for( i=0; i<mNumTriangleVertices; i++ ) {
        delete workingTriangleVertices[i];
        }
    delete [] workingTriangleVertices;

    for( i=0; i<mNumBorderVertices; i++ ) {
        delete workingBorderVertices[i];
        }
    delete [] workingBorderVertices;

    
    delete angleBetweenRotatedCopies;


    return returnVector;
    }



double ObjectParameterSpaceControlPoint::getRotationRate() {
    return mRotationRate;
    }



Vector3D **ObjectParameterSpaceControlPoint::duplicateVertextArray(
    Vector3D **inArray, int inNumTriangleVertices ) {

    Vector3D **returnArray = new Vector3D*[ inNumTriangleVertices ];
    
    for( int i=0; i<inNumTriangleVertices; i++ ) {
        returnArray[i] = new Vector3D( inArray[i] );
        }
    
    return returnArray;
    }



Color **ObjectParameterSpaceControlPoint::duplicateColorArray(
    Color **inArray, int inNumColors, float inAlphaMultiplier ) {

    Color **returnArray = new Color*[ inNumColors ];
    
    for( int i=0; i<inNumColors; i++ ) {
        returnArray[i] = inArray[i]->copy();

        returnArray[i]->a *= inAlphaMultiplier;
        }
    
    return returnArray;
    }



Color *ObjectParameterSpaceControlPoint::readColorFromFile( FILE *inFILE ) {
    int numRead;
    // try reading the red component to test if we have RGBA or a named color

    float r, g, b, a;
    Color *returnColor = NULL;
    
    numRead = fscanf( inFILE, "%f", &r );

    if( numRead == 1 ) {
        // color present as RGBA components
        numRead += fscanf( inFILE, "%f", &g );
        numRead += fscanf( inFILE, "%f", &b );
        numRead += fscanf( inFILE, "%f", &a );

        if( numRead == 4 ) {
            // read all 4
            returnColor = new Color( r, g, b, a, false );
            }
        }
    else {
        // color might be a color name
        char *colorName = new char[100];

        numRead = fscanf( inFILE, "%99s", colorName );

        if( numRead == 1 ) {
            returnColor = NamedColorFactory::getColor( colorName );
            }
        delete [] colorName;
        }

    return returnColor;
    }



void ObjectParameterSpaceControlPoint::writeColorToFile( FILE *inFILE,
                                                         Color *inColor ) {
    
    fprintf( inFILE, "%f ", inColor->r );
    fprintf( inFILE, "%f ", inColor->g );
    fprintf( inFILE, "%f ", inColor->b );
    fprintf( inFILE, "%f\n", inColor->a );
    }



Color **ObjectParameterSpaceControlPoint::blendColorArrays(
    Color **inFirstArray,
    int inFirstArrayLength,
    double inWeightFirstArray,
    Color **inSecondArray,
    int inSecondArrayLength,
    int *outResultLength ) {

    
    
    double weightOfSecondArray = 1 - inWeightFirstArray;

    // blend has the same number of elements as the larger control array
    int resultLength = inFirstArrayLength;

    if( inSecondArrayLength > resultLength ) {
        resultLength = inSecondArrayLength;
        }

    Color **blendColors = new Color*[ resultLength ];
    

    // map the larger array nto the smaller array to
    // blend
    int sizeLargerArray;
    int sizeSmallerArray;

    Color **arrayWithMoreColors;
    Color **arrayWithFewerColors;
    
    double weightOfLargerSet;
    double weightOfSmallerSet;
    

    if( inFirstArrayLength > inSecondArrayLength ) {
        sizeLargerArray = inFirstArrayLength;
        sizeSmallerArray = inSecondArrayLength;
        
        arrayWithMoreColors = inFirstArray;
        arrayWithFewerColors = inSecondArray;
        weightOfLargerSet = inWeightFirstArray;
        weightOfSmallerSet = weightOfSecondArray;

        
        }
    else {
        sizeLargerArray = inSecondArrayLength;
        sizeSmallerArray = inFirstArrayLength;

        arrayWithMoreColors = inSecondArray;
        arrayWithFewerColors = inFirstArray;
        weightOfLargerSet = weightOfSecondArray;
        weightOfSmallerSet = inWeightFirstArray;
        }

    // size of blend array is same as size of larger set

    // factor to map large array indices into the smaller array
    double mapFactor =
        (double)( sizeSmallerArray - 1 ) / (double)(sizeLargerArray - 1 );
    
    for( int i=0; i<resultLength; i++ ) {

        // find the index of our blend partner vertex in the smaller set
        int partnerIndex =
            (int)rint( i * mapFactor );
        
        Color *largerSetColor =
            arrayWithMoreColors[i];
        Color *smallerSetColor = arrayWithFewerColors[ partnerIndex ];

        blendColors[i] =
            Color::linearSum( largerSetColor,
                              smallerSetColor,
                              weightOfLargerSet );
        }


    *outResultLength = resultLength;

    return blendColors;
    }









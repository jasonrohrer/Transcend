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



#include <GL/gl.h>
#include <float.h>


#include "DrawableObject.h"



DrawableObject::DrawableObject( int inNumTriangleVertices,
                                Vector3D **inTriangleVertices,
                                Color **inTriangleVertexFillColors,
                                int inNumBorderVertices,
                                Vector3D **inBorderVertices,
                                Color **inBorderVertextColors,
                                float inBorderWidth )
    : mNumTriangleVertices( inNumTriangleVertices ),
      mTriangleVertices( inTriangleVertices ),
      mTriangleVertexFillColors( inTriangleVertexFillColors ),
      mNumBorderVertices( inNumBorderVertices ),
      mBorderVertices( inBorderVertices ),
      mBorderVertexColors( inBorderVertextColors ),
      mBorderWidth( inBorderWidth ) {

    }

        

DrawableObject::~DrawableObject() {

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



void DrawableObject::rotate( Angle3D *inRotation ) {
    int i;
    for( i=0; i<mNumTriangleVertices; i++ ) {
        mTriangleVertices[i]->rotate( inRotation );
        }
    for( i=0; i<mNumBorderVertices; i++ ) {
        mBorderVertices[i]->rotate( inRotation );
        }
    }



void DrawableObject::move( Vector3D *inPosition ) {
    int i;
    for( i=0; i<mNumTriangleVertices; i++ ) {
        mTriangleVertices[i]->add( inPosition );
        }
    for( i=0; i<mNumBorderVertices; i++ ) {
        mBorderVertices[i]->add( inPosition );
        }
    }



void DrawableObject::scale( double inScale ) {
    int i;
    for( i=0; i<mNumTriangleVertices; i++ ) {
        mTriangleVertices[i]->scale( inScale );
        }
    for( i=0; i<mNumBorderVertices; i++ ) {
        mBorderVertices[i]->scale( inScale );
        }
    }



void DrawableObject::fade( double inAlphaScale ) {
    int i;
    for( i=0; i<mNumTriangleVertices; i++ ) {
        mTriangleVertexFillColors[i]->a *= inAlphaScale;
        }
    for( i=0; i<mNumBorderVertices; i++ ) {
        mBorderVertexColors[i]->a *= inAlphaScale;
        }
    }



char DrawableObject::isBorderInCircle( Vector3D *inCenter,
                                       double inRadius ) {
    for( int i=0; i<mNumBorderVertices; i++ ) {
        if( mBorderVertices[i]->getDistance( inCenter ) <= inRadius ) {
            return true;
            }
        }
    
    // else none inside circle
    return false;
    }



double DrawableObject::getBorderMaxDistance( Vector3D *inPoint ) {
    double maxDistance = 0;
    for( int i=0; i<mNumBorderVertices; i++ ) {
        double distance = mBorderVertices[i]->getDistance( inPoint );
        
        if( distance > maxDistance ) {
            maxDistance = distance;
            }
        }
    
    return maxDistance;
    }



double DrawableObject::getBorderMinDistance( Vector3D *inPoint ) {
    double minDistance = DBL_MAX;
    for( int i=0; i<mNumBorderVertices; i++ ) {
        double distance = mBorderVertices[i]->getDistance( inPoint );
        
        if( distance < minDistance ) {
            minDistance = distance;
            }
        }
    
    return minDistance;
    }


        
void DrawableObject::draw( double inScale, Angle3D *inRotation,
                           Vector3D *inPosition ) {

    Vector3D **worldTriangleVertices = new Vector3D*[ mNumTriangleVertices ];
    Vector3D **worldBorderVertices = new Vector3D*[ mNumBorderVertices ];
    int i;
    
    // transform the vertices
    // (do this once here before drawing since we draw them twice,
    //  once for the filled polygon and once for the border)
    for( i=0; i<mNumTriangleVertices; i++ ) {
        worldTriangleVertices[i] = new Vector3D( mTriangleVertices[i] );
        worldTriangleVertices[i]->scale( inScale );
        worldTriangleVertices[i]->rotate( inRotation );
        worldTriangleVertices[i]->add( inPosition );
        }
    for( i=0; i<mNumBorderVertices; i++ ) {
        worldBorderVertices[i] = new Vector3D( mBorderVertices[i] );
        worldBorderVertices[i]->scale( inScale );
        worldBorderVertices[i]->rotate( inRotation );
        worldBorderVertices[i]->add( inPosition );
        }
    
    // draw the filled polygon
    glBegin( GL_TRIANGLES );
    
        for( i=0; i<mNumTriangleVertices; i++ ) {
            glColor4f( mTriangleVertexFillColors[i]->r,
                       mTriangleVertexFillColors[i]->g,
                       mTriangleVertexFillColors[i]->b,
                       mTriangleVertexFillColors[i]->a );

            glVertex2d( worldTriangleVertices[i]->mX,
                        worldTriangleVertices[i]->mY );
            }
    
    glEnd();


    // draw the border
    glLineWidth( mBorderWidth );
    
    glBegin( GL_LINE_LOOP );
    
        for( i=0; i<mNumBorderVertices; i++ ) {
            glColor4f( mBorderVertexColors[i]->r,
                       mBorderVertexColors[i]->g,
                       mBorderVertexColors[i]->b,
                       mBorderVertexColors[i]->a );

            glVertex2d( worldBorderVertices[i]->mX,
                        worldBorderVertices[i]->mY );
            }
    
    glEnd();


    for( i=0; i<mNumTriangleVertices; i++ ) {
        delete worldTriangleVertices[i];
        }
    delete [] worldTriangleVertices;

    for( i=0; i<mNumBorderVertices; i++ ) {
        delete worldBorderVertices[i];
        }
    delete [] worldBorderVertices;

    }

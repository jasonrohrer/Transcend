/*
 * Modification History
 *
 * 2004-August-25   Jason Rohrer
 * Created.
 * Copied from game/game.cpp
 * Added extra editing controls and parameterized grid/background color.
 *
 * 2004-August-27   Jason Rohrer
 * Added more editing features.
 * Added support for saving to file.
 * Added support for adding/removing triangles and border vertices.
 *
 * 2004-August-30   Jason Rohrer
 * Added safety features.
 *
 * 2004-October-13   Jason Rohrer
 * Added extra grid lines in fine movement mode.
 */


#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <unistd.h>
#include <time.h>


#include "minorGems/graphics/openGL/ScreenGL.h"
#include "minorGems/graphics/openGL/SceneHandlerGL.h"

#include "minorGems/util/random/StdRandomSource.h"
#include "minorGems/util/stringUtils.h"
#include "minorGems/system/Time.h"
#include "minorGems/system/Thread.h"
#include "minorGems/io/file/File.h"


#include "Transcend/game/DrawableObject.h"
#include "Transcend/game/ObjectParameterSpaceControlPoint.h"


class EditorSceneHandler :
    public SceneHandlerGL, public KeyboardHandlerGL,
    public RedrawListenerGL { 
	
	public:
        EditorSceneHandler();
        virtual ~EditorSceneHandler();
        

        ScreenGL *mScreen;

        
        
		// implements the SceneHandlerGL interface
		virtual void drawScene();

        // implements the KeyboardHandlerGL interface
		virtual void keyPressed( unsigned char inKey, int inX, int inY );
		virtual void specialKeyPressed( int inKey, int inX, int inY );
		virtual void keyReleased( unsigned char inKey, int inX, int inY );
		virtual void specialKeyReleased( int inKey, int inX, int inY );

        // implements the RedrawListener interface
		virtual void fireRedraw();
        
    protected:

        double mMaxXPosition;
        double mMinXPosition;
        double mMaxYPosition;
        double mMinYPosition;

        double mGridSpacing;

        Color *mBackgroundColor;
        Color *mGridColor;

        char mGridVisible;
        
        ObjectParameterSpaceControlPoint *mControlPoint;


        char mTriangleVertexEditMode;
        char mBorderVertexEditMode;
        char mWholeObjectMovementMode;
        
        int mCurrentTriangleVertexIndex;
        int mCurrentBorderVertexIndex;

        char mVertexGroupingLock;
        char mColorGroupingLock;

        Color *mClipboardColor;

        double mValueIncrement;
        double mMovementIncrement;
	};



EditorSceneHandler *sceneHandler;
ScreenGL *screen;
char *controlPointFileName = NULL;

double baseViewZ = -10;


// function that destroys object when exit is called.
// exit is the only way to stop the GLUT-based ScreenGL
void cleanUpAtExit() {
    printf( "exiting\n" );

    delete sceneHandler;
    delete screen;

    if( controlPointFileName != NULL ) {
        delete [] controlPointFileName;
        }
    }



int main( int inNumArgs, char **inArgs ) {

    #ifdef __mac__
        // make sure working directory is the same as the directory
        // that the app resides in
        // this is especially important on the mac platform, which
        // doesn't set a proper working directory for double-clicked
        // app bundles

        // app arguments provided as a wxApp member
        // arg 0 is the path to the app executable
        char *appDirectoryPath = stringDuplicate( inArgs[0] );
    
        char *appNamePointer = strstr( appDirectoryPath,
                                       "game1.app" );
        
        // terminate full app path to get parent directory
        appNamePointer[0] = '\0';
        
        chdir( appDirectoryPath );
        
        delete [] appDirectoryPath;
        
    #endif

    // take a file name from our args
    if( inNumArgs > 1 ) {
        controlPointFileName = stringDuplicate( inArgs[1] );
        }

        
    sceneHandler = new EditorSceneHandler();

    // must pass args to GLUT before constructing the screen
    glutInit( &inNumArgs, inArgs );

    screen =
        new ScreenGL( 300, 300, false, 
                      "editor", sceneHandler, NULL, sceneHandler );

    sceneHandler->mScreen = screen;

    screen->addRedrawListener( sceneHandler );
    
    Vector3D *move = new Vector3D( 0, 0, baseViewZ );
    screen->moveView( move );
    delete move;

    // register cleanup function, since screen->start() will never return
    atexit( cleanUpAtExit );

    
    screen->start();

    
    return 0;
    }






EditorSceneHandler::EditorSceneHandler()
    : mMaxXPosition( 100 ), mMinXPosition( -100 ),
      mMaxYPosition( 100 ), mMinYPosition( -100 ),
      mGridSpacing( 1 ),
      mGridVisible( true ),
      mTriangleVertexEditMode( false ), mBorderVertexEditMode( false ),
      mWholeObjectMovementMode( false ),
      mCurrentTriangleVertexIndex( 0 ), mCurrentBorderVertexIndex( 0 ),
      mVertexGroupingLock( false ),
      mColorGroupingLock( false ),
      mClipboardColor( new Color( 0, 0, 0, 1.0 ) ),
      mValueIncrement( 0.1 ),
      mMovementIncrement( 1 ) {


    FILE *controlPointFile = fopen( controlPointFileName, "r" );

    char error;
    mControlPoint = new ObjectParameterSpaceControlPoint( controlPointFile,
                                                          &error );

    if( controlPointFile != NULL ) {
        fclose( controlPointFile );
        }

    FILE *gridColorFILE = fopen( "gridColor", "r" );

    if( gridColorFILE != NULL ) {
        mGridColor = ObjectParameterSpaceControlPoint::readColorFromFile(
            gridColorFILE );
        fclose( gridColorFILE );
        }
    else {
        mGridColor = new Color( 0, 0, 0, 1 );
        }

    FILE *backgroundColorFILE = fopen( "backgroundColor", "r" );

    if( backgroundColorFILE != NULL ) {
        mBackgroundColor = ObjectParameterSpaceControlPoint::readColorFromFile(
            backgroundColorFILE );
        fclose( backgroundColorFILE );
        }
    else {
        mBackgroundColor = new Color( 0, 0, 0, 1 );
        }

    
    
    }



EditorSceneHandler::~EditorSceneHandler() {

    delete mControlPoint;

    delete mGridColor;
    delete mBackgroundColor;

    delete mClipboardColor;
    }





void EditorSceneHandler::drawScene() {
    glClearColor( mBackgroundColor->r,
                  mBackgroundColor->g,
                  mBackgroundColor->b,
                  mBackgroundColor->a );
	
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );


    if( mGridVisible ) {
        glLineWidth( 2 );
        glBegin( GL_LINES );
            glColor4f( mGridColor->r,
                       mGridColor->g,
                       mGridColor->b,
                       mGridColor->a );
            for( double x=mMinXPosition; x<=mMaxXPosition; x+=mGridSpacing ) {
                glVertex2d( x, mMinYPosition );
                glVertex2d( x, mMaxYPosition );
                }
            for( double y=mMinYPosition; y<=mMaxYPosition; y+=mGridSpacing ) {
                glVertex2d( mMinXPosition, y );
                glVertex2d( mMaxXPosition, y );
                }

            if( mMovementIncrement < mGridSpacing ) {
                // draw additional transparent lines to show move increments
                glLineWidth( 1 );
                glColor4f( mGridColor->r,
                       mGridColor->g,
                       mGridColor->b,
                       mGridColor->a / 2 );
            
                for( double x=mMinXPosition; x<=mMaxXPosition;
                     x+=mMovementIncrement ) {

                    glVertex2d( x, mMinYPosition );
                    glVertex2d( x, mMaxYPosition );
                    }
                for( double y=mMinYPosition; y<=mMaxYPosition;
                     y+=mMovementIncrement ) {
                    
                    glVertex2d( mMinXPosition, y );
                    glVertex2d( mMaxXPosition, y );
                    }
                }
            

            // draw extra lines through origin
            glLineWidth( 2 );
            glColor4f( 0,
                       1,
                       0, 1 );
            glVertex2d( 0, mMinYPosition );
            glVertex2d( 0, mMaxYPosition );
            glVertex2d( mMinXPosition, 0 );
            glVertex2d( mMaxYPosition, 0 );
        glEnd();

        // draw the clipboard color
        double x = 5 * mGridSpacing;
        double y = 5 * mGridSpacing;

        glBegin( GL_QUADS ); {
            glColor4f( mClipboardColor->r,
                       mClipboardColor->g,
                       mClipboardColor->b,
                       mClipboardColor->a );
            glVertex2d( x, y );
            glVertex2d( x, y + mGridSpacing );
            glVertex2d( x + mGridSpacing, y + mGridSpacing );
            glVertex2d( x + mGridSpacing, y );
            
            }; glEnd();
        }

    
    int i;



    SimpleVector<DrawableObject *> *pointObjects =
        mControlPoint->getDrawableObjects();
    int numPointObjects = pointObjects->size();

    
    int xOffset = 0;
    int yOffset = 0;
    Vector3D *offsetVector = new Vector3D( xOffset, yOffset, 0 );
    
    // draw the bullets and enemies with no extra rotation
    Angle3D *zeroAngle = new Angle3D( 0, 0, 0 );
    
    for( i=0; i<numPointObjects; i++ ) {
        DrawableObject *component =
            *( pointObjects->getElement( i ) );
        component->draw( 1, zeroAngle, offsetVector );
        }
    
    
    
    delete zeroAngle;
    
    delete offsetVector;

    // delete the bullet and enemy objects
    for( i=0; i<numPointObjects; i++ ) {
        delete *( pointObjects->getElement( i ) );
        }
    delete pointObjects;        

    glLineWidth( 2 );
    glBegin( GL_LINE_LOOP );
        Vector3D *point = NULL;

        double boxRadius = mGridSpacing / 2;
        
        if( mTriangleVertexEditMode ) {
            point = new Vector3D( mControlPoint->mTriangleVertices[
                mCurrentTriangleVertexIndex ] );
            glColor4f( 1, 0, 0, 0.75 );
            }
        else if( mBorderVertexEditMode ) {
            point = new Vector3D( mControlPoint->mBorderVertices[
                mCurrentBorderVertexIndex ] );
            glColor4f( .5, 0, 1, 0.75 );
            }
        else if( mWholeObjectMovementMode ) {
            // take average of entire object (center)
            point = new Vector3D( 0, 0, 0 );

            for( i=0; i<mControlPoint->mNumBorderVertices; i++ ) {
                point->add( mControlPoint->mBorderVertices[ i ] );
                }
            for( i=0; i<mControlPoint->mNumTriangleVertices; i++ ) {
                point->add( mControlPoint->mTriangleVertices[ i ] );
                }

            point->scale( 1.0 / ( mControlPoint->mNumTriangleVertices +
                                  mControlPoint->mNumBorderVertices ) );

            // bigger box
            boxRadius *= 5;
            glColor4f( 0, 1, 0, .75 );
            }
            
            
        if( point != NULL ) {
            
            glVertex2d( point->mX - boxRadius, point->mY - boxRadius );
            glVertex2d( point->mX + boxRadius, point->mY - boxRadius );
            glVertex2d( point->mX + boxRadius, point->mY + boxRadius );
            glVertex2d( point->mX - boxRadius, point->mY + boxRadius );

            delete point;
            }
    glEnd();
    }



void EditorSceneHandler::keyPressed(
	unsigned char inKey, int inX, int inY ) {

    if( inKey == ' ' ) {
        }
    else if( inKey == 't' || inKey == 'T' ) {
        // switch to triangle mode
        mTriangleVertexEditMode = ! mTriangleVertexEditMode;

        if( ! mTriangleVertexEditMode ) {
            mBorderVertexEditMode = true;
            printf( "Border mode\n" );
            }
        else {
            mBorderVertexEditMode = false;
            printf( "Triangle mode\n" );
            }
        mWholeObjectMovementMode = false;
        }
    else if( inKey == 'm' || inKey == 'M' ) {
        // exit from special modes
        mBorderVertexEditMode = false;
        mTriangleVertexEditMode = false;
        mWholeObjectMovementMode = true;
        printf( "Whole object movement mode\n" );
        }
    else if( inKey == 'e' || inKey == 'E' ) {
        // exit from special modes
        mBorderVertexEditMode = false;
        mTriangleVertexEditMode = false;
        mWholeObjectMovementMode = false;
        printf( "Movement mode\n" );
        }
    else if( inKey == 'v' || inKey == 'V' ) {
        // toggle grid
        mGridVisible = ! mGridVisible;
        }
    else if( inKey == 'l' || inKey == 'L' ) {
        // toggle lock
        mVertexGroupingLock = ! mVertexGroupingLock;

        printf( "Vertex grouping lock" );

        if( mVertexGroupingLock ) {
            printf( " on\n" );
            }
        else {
            printf( " off\n" );
            }
        }
    else if( inKey == 'k' || inKey == 'K' ) {
        // toggle lock
        mColorGroupingLock = ! mColorGroupingLock;

        printf( "Color grouping lock" );

        if( mColorGroupingLock ) {
            printf( " on\n" );
            }
        else {
            printf( " off\n" );
            }
        }
    else if( inKey == 'n' || inKey == 'N' ) {
        // cycle through vertices
        if( mBorderVertexEditMode ) {
            mCurrentBorderVertexIndex++;

            if( mCurrentBorderVertexIndex >=
                mControlPoint->mNumBorderVertices ) {

                mCurrentBorderVertexIndex = 0;
                }
            }
        else if( mTriangleVertexEditMode ) {
            mCurrentTriangleVertexIndex++;

            if( mCurrentTriangleVertexIndex >=
                mControlPoint->mNumTriangleVertices ) {

                mCurrentTriangleVertexIndex = 0;
                }
            }
        }
    else if( inKey == 'p' || inKey == 'P' ) {
        // cycle through vertices
        if( mBorderVertexEditMode ) {
            mCurrentBorderVertexIndex--;

            if( mCurrentBorderVertexIndex < 0 ) {

                mCurrentBorderVertexIndex =
                    mControlPoint->mNumBorderVertices - 1;
                }
            }
        else if( mTriangleVertexEditMode ) {
            mCurrentTriangleVertexIndex--;

            if( mCurrentTriangleVertexIndex < 0 ) {

                mCurrentTriangleVertexIndex =
                    mControlPoint->mNumTriangleVertices - 1;
                }
            }
        }
    
    else if( inKey == 'q' || inKey == 'Q' ) {
		// quit

        // save control point to be safe
        
        FILE *file = fopen( "rescue.txt", "w" );

        if( file != NULL ) {
            mControlPoint->writeToFile( file );
            fclose( file );

            printf( "Saved control point to file: rescue.txt\n" );
            }
        else {
            printf( "Failed to open rescue.txt for writing.\n" );
            }
        
		::exit( 0 );
		}


    if( mBorderVertexEditMode || mTriangleVertexEditMode ) {
        Color *color;
        Vector3D *position;
        if( mBorderVertexEditMode ) {
            color = mControlPoint->mBorderVertexColors[
                mCurrentBorderVertexIndex ];
            position = mControlPoint->mBorderVertices[
                mCurrentBorderVertexIndex ];
            }
        else {
            color = mControlPoint->mTriangleVertexFillColors[
                mCurrentTriangleVertexIndex ];
            position = mControlPoint->mTriangleVertices[
                mCurrentTriangleVertexIndex ];
            }

        Color *oldColor = color->copy();
        
        char colorChanged = false;

        if( inKey == 'o' || inKey == 'O' ) {
            // copy color to clipboard
            mClipboardColor->r = color->r;
            mClipboardColor->g = color->g;
            mClipboardColor->b = color->b;
            mClipboardColor->a = color->a;
            }
        else if( inKey == 'u' || inKey == 'U' ) {
            // copy clipboard to color
            color->r = mClipboardColor->r;
            color->g = mClipboardColor->g;
            color->b = mClipboardColor->b;
            color->a = mClipboardColor->a;

            colorChanged = true;
            }
        else if( inKey == 'i' || inKey == 'I' ) {
            // invert the color
            color->r = 1 - color->r;
            color->g = 1 - color->g;
            color->b = 1 - color->b;

            colorChanged = true;
            }
        else if( inKey == 'R' ) {
            color->r += mValueIncrement;
            if( color->r > 1 ) {
                color->r = 1;
                }
            colorChanged = true;
            }
        else if( inKey == 'G' ) {
            color->g += mValueIncrement;
            if( color->g > 1 ) {
                color->g = 1;
                }
            colorChanged = true;
            }        
        else if( inKey == 'B' ) {
            color->b += mValueIncrement;
            if( color->b > 1 ) {
                color->b = 1;
                }
            colorChanged = true;
            }
        else if( inKey == 'A' ) {
            color->a += mValueIncrement;
            if( color->a > 1 ) {
                color->a = 1;
                }
            colorChanged = true;
            }
        else if( inKey == 'r' ) {
            color->r -= mValueIncrement;
            if( color->r < 0 ) {
                color->r = 0;
                }
            colorChanged = true;
            }
        else if( inKey == 'g' ) {
            color->g -= mValueIncrement;
            if( color->g < 0 ) {
                color->g = 0;
                }
            colorChanged = true;
            }        
        else if( inKey == 'b' ) {
            color->b -= mValueIncrement;
            if( color->b < 0 ) {
                color->b = 0;
                }
            colorChanged = true;
            }
        else if( inKey == 'a' ) {
            color->a -= mValueIncrement;
            if( color->a < 0 ) {
                color->a = 0;
                }
            colorChanged = true;
            }

        if( colorChanged ) {
            printf( "Color = ( r=%.2f, g=%.2f, b=%.2f, a=%.2f )\n",
                    color->r, color->g, color->b, color->a );


            if( mVertexGroupingLock || mColorGroupingLock ) {
                // color other vertices that were in the same spot
                // or have the same color

                int i;
                for( i=0; i<mControlPoint->mNumTriangleVertices; i++ ) {
                    Vector3D *otherVertex =
                        mControlPoint->mTriangleVertices[i];
                    Color *otherColor =
                        mControlPoint->mTriangleVertexFillColors[i];
                
                    if( ( mVertexGroupingLock &&
                          position->getDistance( otherVertex ) <= 0.01 )
                        ||
                        ( mColorGroupingLock &&
                          oldColor->equals( otherColor ) ) ) {

                        otherColor->r = color->r;
                        otherColor->g = color->g;
                        otherColor->b = color->b;
                        otherColor->a = color->a;
                        }
                    }
                for( i=0; i<mControlPoint->mNumBorderVertices; i++ ) {
                    Vector3D *otherVertex =
                        mControlPoint->mBorderVertices[i];
                    Color *otherColor =
                        mControlPoint->mBorderVertexColors[i];
                
                    if( ( mVertexGroupingLock &&
                          position->getDistance( otherVertex ) <= 0.01 )
                        ||
                        ( mColorGroupingLock &&
                          oldColor->equals( otherColor ) ) ) {

                        otherColor->r = color->r;
                        otherColor->g = color->g;
                        otherColor->b = color->b;
                        otherColor->a = color->a;
                        }
                    }
                }

            
            }
        delete oldColor;


        if( inKey == 'y' || inKey == 'Y' ) {
            // add a new border vertex or triangle

            if( mTriangleVertexEditMode ) {
                int oldNumVertices = mControlPoint->mNumTriangleVertices;
                int newNumVertices = oldNumVertices + 3;

                Vector3D **oldTriangleVertices =
                    mControlPoint->mTriangleVertices;
                Vector3D **newTriangleVertices =
                    new Vector3D*[newNumVertices];
                
                Color **oldTriangleVertexFillColors =
                    mControlPoint->mTriangleVertexFillColors;
                Color **newTriangleVertexFillColors =
                    new Color*[newNumVertices];

                // copy the existing vertices and colors as pointers
                memcpy( newTriangleVertices, oldTriangleVertices,
                        sizeof( Vector3D * ) * oldNumVertices );
                memcpy( newTriangleVertexFillColors,
                        oldTriangleVertexFillColors,
                        sizeof( Color * ) * oldNumVertices );

                // add a new default triangle with default colors

                newTriangleVertices[ oldNumVertices ] =
                    new Vector3D( 5, 5, 0 );
                newTriangleVertices[ oldNumVertices + 1 ] =
                    new Vector3D( 5, 6, 0 );
                newTriangleVertices[ oldNumVertices + 2 ] =
                    new Vector3D( 6, 6, 0 );

                newTriangleVertexFillColors[ oldNumVertices ] =
                    new Color( 0, 0, 0, 1 );
                newTriangleVertexFillColors[ oldNumVertices + 1] =
                    new Color( 1, 0, 0, 1 );
                newTriangleVertexFillColors[ oldNumVertices + 2 ] =
                    new Color( 0, 1, 0, 1 );


                // replace old arrays
                delete [] oldTriangleVertices;
                delete [] oldTriangleVertexFillColors;

                mControlPoint->mNumTriangleVertices = newNumVertices;

                mControlPoint->mTriangleVertices = newTriangleVertices;
                mControlPoint->mTriangleVertexFillColors =
                    newTriangleVertexFillColors;

                mCurrentTriangleVertexIndex = newNumVertices - 1;
                }
            else if( mBorderVertexEditMode ) {
                int oldNumVertices = mControlPoint->mNumBorderVertices;
                int newNumVertices = oldNumVertices + 1;

                Vector3D **oldBorderVertices =
                    mControlPoint->mBorderVertices;
                Vector3D **newBorderVertices =
                    new Vector3D*[newNumVertices];
                
                Color **oldBorderVertexColors =
                    mControlPoint->mBorderVertexColors;
                Color **newBorderVertexColors =
                    new Color*[newNumVertices];

                // copy the existing vertices and colors as pointers
                memcpy( newBorderVertices, oldBorderVertices,
                        sizeof( Vector3D * ) * oldNumVertices );
                memcpy( newBorderVertexColors,
                        oldBorderVertexColors,
                        sizeof( Color * ) * oldNumVertices );

                // add a new default border with default colors

                newBorderVertices[ oldNumVertices ] =
                    new Vector3D( 5, 5, 0 );

                newBorderVertexColors[ oldNumVertices ] =
                    new Color( 0, 0, 0, 1 );


                // replace old arrays
                delete [] oldBorderVertices;
                delete [] oldBorderVertexColors;

                mControlPoint->mNumBorderVertices = newNumVertices;

                mControlPoint->mBorderVertices = newBorderVertices;
                mControlPoint->mBorderVertexColors =
                    newBorderVertexColors;

                
                mCurrentBorderVertexIndex = newNumVertices - 1;
                }
            }
      
        else if( inKey == 'h' || inKey == 'H' ) {
            printf( "Delete vertex:  Are you sure? (y/n)  " );
            fflush( stdout );

            char *buffer = new char[100];
            scanf( "%99s", buffer );

            char *lowerBuffer = stringToLowerCase( buffer );
            delete [] buffer;

            char actuallyDestroy = false;
            
            if( strstr( lowerBuffer, "y" ) != NULL ) { 
                actuallyDestroy = true;
                }

            delete [] lowerBuffer;

            
            // destroy current vertex or triangle
            if( actuallyDestroy && mTriangleVertexEditMode ) {
                int oldNumVertices = mControlPoint->mNumTriangleVertices;
                int newNumVertices = oldNumVertices - 3;

                int oldNumTriangles = oldNumVertices / 3;
                
                // make sure current vertex is first vertex in a triangle

                
                if( mCurrentTriangleVertexIndex % 3 != 0 ) {
                    mCurrentTriangleVertexIndex -=
                        ( mCurrentTriangleVertexIndex % 3 );
                    }

                
                Vector3D **oldTriangleVertices =
                    mControlPoint->mTriangleVertices;
                Vector3D **newTriangleVertices =
                    new Vector3D*[newNumVertices];
                
                Color **oldTriangleVertexFillColors =
                    mControlPoint->mTriangleVertexFillColors;
                Color **newTriangleVertexFillColors =
                    new Color*[newNumVertices];



                // copy triangles, skipping the one we are dropping

                int newIndex = 0;
                for( int oldIndex=0; oldIndex<oldNumTriangles; oldIndex++ ) {
                    int oldVertexIndex = oldIndex * 3;
                    if( oldVertexIndex == mCurrentTriangleVertexIndex ) {
                        
                        // skip this triangle
                        delete oldTriangleVertices[ oldVertexIndex ];
                        delete oldTriangleVertices[ oldVertexIndex + 1 ];
                        delete oldTriangleVertices[ oldVertexIndex + 2 ];
                        delete oldTriangleVertexFillColors[ oldVertexIndex ];
                        delete
                            oldTriangleVertexFillColors[ oldVertexIndex + 1 ];
                        delete
                            oldTriangleVertexFillColors[ oldVertexIndex + 2 ];
                        }
                    else {
                        // copy this triangle

                        int newVertexIndex = newIndex * 3;

                        newTriangleVertices[ newVertexIndex ] =
                            oldTriangleVertices[ oldVertexIndex ];
                        newTriangleVertices[ newVertexIndex + 1 ] =
                            oldTriangleVertices[ oldVertexIndex + 1];
                        newTriangleVertices[ newVertexIndex + 2] =
                            oldTriangleVertices[ oldVertexIndex + 2 ];

                        newTriangleVertexFillColors[ newVertexIndex ] =
                            oldTriangleVertexFillColors[ oldVertexIndex ];
                        newTriangleVertexFillColors[ newVertexIndex + 1 ] =
                            oldTriangleVertexFillColors[ oldVertexIndex + 1];
                        newTriangleVertexFillColors[ newVertexIndex + 2] =
                            oldTriangleVertexFillColors[ oldVertexIndex + 2 ];

                        newIndex++;
                        }
                    }

                
                // replace old arrays
                delete [] oldTriangleVertices;
                delete [] oldTriangleVertexFillColors;

                mControlPoint->mNumTriangleVertices = newNumVertices;

                mControlPoint->mTriangleVertices = newTriangleVertices;
                mControlPoint->mTriangleVertexFillColors =
                    newTriangleVertexFillColors;

                if( mCurrentTriangleVertexIndex >= newNumVertices ) {
                    mCurrentTriangleVertexIndex = newNumVertices - 1;
                    }
                }
            else if( actuallyDestroy && mBorderVertexEditMode ) {
                int oldNumVertices = mControlPoint->mNumBorderVertices;
                int newNumVertices = oldNumVertices - 1;

                Vector3D **oldBorderVertices =
                    mControlPoint->mBorderVertices;
                Vector3D **newBorderVertices =
                    new Vector3D*[newNumVertices];
                
                Color **oldBorderVertexColors =
                    mControlPoint->mBorderVertexColors;
                Color **newBorderVertexColors =
                    new Color*[newNumVertices];



                
                // copy the existing vertices and colors as pointers,
                // skipping the dropped vertex

                int newIndex = 0;
                for( int oldIndex=0; oldIndex<oldNumVertices; oldIndex++ ) {
                    if( oldIndex == mCurrentBorderVertexIndex ) {
                        
                        // skip this vertex
                        delete oldBorderVertices[ oldIndex ];
                        delete oldBorderVertexColors[ oldIndex ];
                        }
                    else {
                        // copy this vertex

                        newBorderVertices[ newIndex ] =
                            oldBorderVertices[ oldIndex ];
                        
                        newBorderVertexColors[ newIndex ] =
                            oldBorderVertexColors[ oldIndex ];
                        
                        newIndex++;
                        }
                    }


                // replace old arrays
                delete [] oldBorderVertices;
                delete [] oldBorderVertexColors;

                mControlPoint->mNumBorderVertices = newNumVertices;

                mControlPoint->mBorderVertices = newBorderVertices;
                mControlPoint->mBorderVertexColors = newBorderVertexColors;


                if( mCurrentBorderVertexIndex >= newNumVertices ) {
                    mCurrentBorderVertexIndex = newNumVertices - 1;
                    }
                }

            }

        }
    

    char borderWidthChanged = false;
    if( inKey == 'w' ) {
        mControlPoint->mBorderWidth -= 1;
        if( mControlPoint->mBorderWidth < 1 ) {
            mControlPoint->mBorderWidth = 1;
            }
        borderWidthChanged = true;
        }
    else if( inKey == 'W' ) {
        mControlPoint->mBorderWidth += 1;
        borderWidthChanged = true;
        }

    if( borderWidthChanged ) {
        printf( "Border width = %f\n", mControlPoint->mBorderWidth );
        }


    char numRotatedCopiesChanged = false;
    if( inKey == 'c' ) {
        mControlPoint->mNumRotatedCopies -= 1;
        if( mControlPoint->mNumRotatedCopies < 0 ) {
            mControlPoint->mNumRotatedCopies = 0;
            }
        numRotatedCopiesChanged = true;
        }
    else if( inKey == 'C' ) {
        mControlPoint->mNumRotatedCopies += 1;
        numRotatedCopiesChanged = true;
        }

    if( numRotatedCopiesChanged ) {
        printf( "Num rotated copies= %f\n",
                mControlPoint->mNumRotatedCopies );
        }


    char rotatedCopyScaleFactorChanged = false;
    if( inKey == 'x' ) {
        mControlPoint->mRotatedCopyScaleFactor -= mValueIncrement;
        if( mControlPoint->mRotatedCopyScaleFactor < 0 ) {
            mControlPoint->mRotatedCopyScaleFactor = 0;
            }
        rotatedCopyScaleFactorChanged = true;
        }
    else if( inKey == 'X' ) {
        mControlPoint->mRotatedCopyScaleFactor += mValueIncrement;
        rotatedCopyScaleFactorChanged = true;
        }

    if( rotatedCopyScaleFactorChanged ) {
        printf( "Rotated copy scale factor= %f\n",
                mControlPoint->mRotatedCopyScaleFactor );
        }

    char rotatedCopyAngleScaleFactorChanged = false;
    if( inKey == 'd' ) {
        mControlPoint->mRotatedCopyAngleScaleFactor -= mValueIncrement;
        if( mControlPoint->mRotatedCopyAngleScaleFactor < 0 ) {
            mControlPoint->mRotatedCopyAngleScaleFactor = 0;
            }
        rotatedCopyAngleScaleFactorChanged = true;
        }
    else if( inKey == 'D' ) {
        mControlPoint->mRotatedCopyAngleScaleFactor += mValueIncrement;
        rotatedCopyAngleScaleFactorChanged = true;
        }

    if( rotatedCopyAngleScaleFactorChanged ) {
        printf( "Rotated copy anglescale factor= %f\n",
                mControlPoint->mRotatedCopyAngleScaleFactor );
        }

    

    if( inKey == 'f' || inKey == 'F' ) {
        if( mValueIncrement == 0.01 ) {
            mValueIncrement = 0.1;
            mMovementIncrement = 1;
            }
        else {
            mValueIncrement = 0.01;
            mMovementIncrement = 0.1;
            }

        printf( "New movement increment = %f, new value increment = %f\n",
                mMovementIncrement, mValueIncrement );
        }

    

    Vector3D *viewPosition = mScreen->getViewPosition();
    if( inKey == 'z' ) {
        // zoom out
        viewPosition->mZ -= mGridSpacing * mMovementIncrement;
        }
    else if( inKey == 'Z' ) {
        // zoom in
        viewPosition->mZ += mGridSpacing * mMovementIncrement;
        }
        
    mScreen->setViewPosition( viewPosition );
    
    delete viewPosition;


    if( inKey == 's' || inKey == 'S' ) {
        // ask for save location

        char *buffer = new char[100];

        printf( "Saving -- Enter file name: " );
        fflush( stdout);
        
        scanf( "%99s", buffer );

        FILE *file = fopen( buffer, "w" );

        if( file != NULL ) {
            mControlPoint->writeToFile( file );
            fclose( file );

            printf( "Saved control point to file: %s\n", buffer );
            }
        else {
            printf( "Failed to open file for writing:  %s\n", buffer );
            }
        delete [] buffer;        
        }

    
    }



void EditorSceneHandler::keyReleased(
	unsigned char inKey, int inX, int inY ) {

	}



void EditorSceneHandler::specialKeyPressed(
	int inKey, int inX, int inY ) {

    if( ! mBorderVertexEditMode && ! mTriangleVertexEditMode &&
        ! mWholeObjectMovementMode ) {
        // move view
        Vector3D *viewPosition = mScreen->getViewPosition();
        
        if( inKey == GLUT_KEY_UP ) {
            viewPosition->mY -= mGridSpacing * mMovementIncrement;
            }
        else if( inKey == GLUT_KEY_DOWN ) {
	
            viewPosition->mY += mGridSpacing * mMovementIncrement;
            }
        else if( inKey == GLUT_KEY_LEFT ) {
            // zoom out
            viewPosition->mX += mGridSpacing * mMovementIncrement;
            }
        else if( inKey == GLUT_KEY_RIGHT ) {
            // zoom in
            viewPosition->mX -= mGridSpacing * mMovementIncrement;
            }

    
        mScreen->setViewPosition( viewPosition );

        delete viewPosition;
        }
    else if( mWholeObjectMovementMode ) {
        int i;
        for( i=0; i<mControlPoint->mNumTriangleVertices; i++ ) {

            Vector3D *vertex = mControlPoint->mTriangleVertices[i];
            if( inKey == GLUT_KEY_UP ) {
                vertex->mY += mGridSpacing * mMovementIncrement;
                }
            else if( inKey == GLUT_KEY_DOWN ) {
                
                vertex->mY -= mGridSpacing * mMovementIncrement;
                }
            else if( inKey == GLUT_KEY_LEFT ) {
                // zoom out
                vertex->mX += mGridSpacing * mMovementIncrement;
                }
            else if( inKey == GLUT_KEY_RIGHT ) {
                // zoom in
                vertex->mX -= mGridSpacing * mMovementIncrement;
                }
            }
        for( i=0; i<mControlPoint->mNumBorderVertices; i++ ) {

            Vector3D *vertex = mControlPoint->mBorderVertices[i];
            if( inKey == GLUT_KEY_UP ) {
                vertex->mY += mGridSpacing * mMovementIncrement;
                }
            else if( inKey == GLUT_KEY_DOWN ) {
                
                vertex->mY -= mGridSpacing * mMovementIncrement;
                }
            else if( inKey == GLUT_KEY_LEFT ) {
                // zoom out
                vertex->mX += mGridSpacing * mMovementIncrement;
                }
            else if( inKey == GLUT_KEY_RIGHT ) {
                // zoom in
                vertex->mX -= mGridSpacing * mMovementIncrement;
                }
            }
        
        }
    else {
        Vector3D *vertex;
        if( mBorderVertexEditMode ) {
            vertex = mControlPoint->mBorderVertices[
                mCurrentBorderVertexIndex ];
            }
        else {
            vertex = mControlPoint->mTriangleVertices[
                    mCurrentTriangleVertexIndex ];
            }

        Vector3D *oldPosition = new Vector3D( vertex );

        if( inKey == GLUT_KEY_UP ) {
            vertex->mY += mGridSpacing * mMovementIncrement;
            }
        else if( inKey == GLUT_KEY_DOWN ) {
	
            vertex->mY -= mGridSpacing * mMovementIncrement;
            }
        else if( inKey == GLUT_KEY_LEFT ) {
            // zoom out
            vertex->mX += mGridSpacing * mMovementIncrement;
            }
        else if( inKey == GLUT_KEY_RIGHT ) {
            // zoom in
            vertex->mX -= mGridSpacing * mMovementIncrement;
            }

        printf( "New vertex position = ( %f, %f )\n",
                vertex->mX, vertex->mY );
        
        if( mVertexGroupingLock ) {
            // move other vertices that were in the same spot

            int i;
            for( i=0; i<mControlPoint->mNumTriangleVertices; i++ ) {
                Vector3D *otherVertex = mControlPoint->mTriangleVertices[i];
                
                if( oldPosition->getDistance( otherVertex ) <= 0.01 ) {

                    otherVertex->mX = vertex->mX;
                    otherVertex->mY = vertex->mY;
                    }
                }
            for( i=0; i<mControlPoint->mNumBorderVertices; i++ ) {
                Vector3D *otherVertex = mControlPoint->mBorderVertices[i];
                
                if( oldPosition->getDistance( otherVertex ) <= 0.01 ) {

                    otherVertex->mX = vertex->mX;
                    otherVertex->mY = vertex->mY;
                    }
                }
            }

        delete oldPosition;
        }

    
	}



void EditorSceneHandler::specialKeyReleased(
	int inKey, int inX, int inY ) {

	}


void EditorSceneHandler::fireRedraw() {
    
    }



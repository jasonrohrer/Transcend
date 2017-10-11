/*
 * Modification History
 *
 * 2004-June-12   Jason Rohrer
 * Created.
 *
 * 2004-June-14   Jason Rohrer
 * Changed to allow specification of a directory to read colors from.
 *
 * 2004-June-15   Jason Rohrer
 * Changed to use LevelDirectoryManager to determine where to read colors from.
 *
 * 2004-August-12   Jason Rohrer
 * Optimized Color constructor.
 */



#include "NamedColorFactory.h"
#include "LevelDirectoryManager.h"


#include "minorGems/io/file/File.h"
#include "minorGems/util/stringUtils.h"


#include <stdio.h>



Color *NamedColorFactory::getColor( char *inColorName ) {

    // will remain NULL if reading the color fails.
    Color *color = NULL;
    

    File *levelDirectory = LevelDirectoryManager::getLevelDirectory();
    File *colorDirectory = levelDirectory->getChildFile( "colors" );

    delete levelDirectory;

    
    File *colorFile = colorDirectory->getChildFile( inColorName );
    delete colorDirectory;

    
    if( colorFile != NULL ) {
        
        if( colorFile->exists() ) {
            
            char *contents = colorFile->readFileContents();

            if( contents != NULL ) {

                SimpleVector<char *> *tokens = tokenizeString( contents );

                int numTokens = tokens->size();

                if( numTokens == 4 ) {

                    float red, green, blue, alpha;

                    int totalNumRead = 0;

                    totalNumRead += sscanf( *( tokens->getElement( 0 ) ),
                                            "%f", &red );
                    totalNumRead += sscanf( *( tokens->getElement( 1 ) ),
                                            "%f", &green );
                    totalNumRead += sscanf( *( tokens->getElement( 2 ) ),
                                            "%f", &blue );
                    totalNumRead += sscanf( *( tokens->getElement( 3 ) ),
                                            "%f", &alpha );

                    // make sure we read all 4 values
                    if( totalNumRead == 4 ) {
                        color = new Color( red, green, blue, alpha, false );
                        }
                    }

                for( int i=0; i<numTokens; i++ ) {
                    delete [] *( tokens->getElement( i ) );
                    }
                delete tokens;
                
                
                delete [] contents;
                }
            }
        
        delete colorFile;
        }
    
    
    return color;
    }



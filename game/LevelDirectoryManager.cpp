/*
 * Modification History
 *
 * 2004-June-15   Jason Rohrer
 * Created.
 *
 * 2004-June-17   Jason Rohrer
 * Added function for reading values from level files.
 *
 * 2004-June-21   Jason Rohrer
 * Added function for reading int values.
 */



#include "LevelDirectoryManager.h"


#include <stdio.h>



StaticLevelDirectoryFileWrapper LevelDirectoryManager::mFileWrapper;



void LevelDirectoryManager::setLevelDirectory( File *inFile ) {

    if( mFileWrapper.mFile != NULL ) {
        delete mFileWrapper.mFile;
        }

    mFileWrapper.mFile = inFile;    
    }



File *LevelDirectoryManager::getLevelDirectory() {

    if( mFileWrapper.mFile != NULL ) {
        return mFileWrapper.mFile->copy();
        }
    else {
        // return default location... level 1
        File *levelDirectory = new File( NULL, "levels" );
        File *level1Directory = levelDirectory->getChildFile( "001" );

        delete levelDirectory;

        return level1Directory;
        }
    }



File *LevelDirectoryManager::getLevelFile( char *inFileName,
                                           char inPrintErrorMessage ) {

    File *levelDirectory = getLevelDirectory();
    
    File *levelFile =
        levelDirectory->getChildFile( inFileName );

    if( levelFile == NULL && inPrintErrorMessage ) {
        char *directoryName = levelDirectory->getFullFileName();
        
        printf( "Error accessing file in directory %s\n", directoryName );
        
        delete [] directoryName;
        }
    
    delete levelDirectory;

    return levelFile;
    }



FILE *LevelDirectoryManager::getStdStream( char *inFileName,
                                           char inPrintErrorMessage ) {
    
    File *levelFile =
        getLevelFile( inFileName, inPrintErrorMessage );


    FILE *returnValue;
    
    if( levelFile != NULL ) {
        char *fileName = levelFile->getFullFileName();

        returnValue = fopen( fileName, "r" );

        if( returnValue == NULL && inPrintErrorMessage ) {
            printf( "Error opening file %s for reading\n", fileName );
            }

        delete [] fileName;
        
        delete levelFile;
        }
    else {
        returnValue = NULL;
        }
    
    return returnValue;
    }



char *LevelDirectoryManager::readFileContents( char *inFileName,
                                               char inPrintErrorMessage ) {
    
    File *levelFile =
        getLevelFile( inFileName, inPrintErrorMessage );

    char *returnValue;
    
    if( levelFile != NULL ) {
        char *fileContents = levelFile->readFileContents();
        
        if( fileContents == NULL && inPrintErrorMessage ) {
            char *fileName = levelFile->getFullFileName();
            
            printf( "Error reading contents from file %s\n", fileName );

            delete [] fileName;
            }
        
        delete levelFile;

        // will be NULL if reading from file fails
        returnValue = fileContents;
        }
    else {
        returnValue = NULL;
        }

    return returnValue;
    }
    


double LevelDirectoryManager::readDoubleFileContents(
    char *inFileName,
    char *outError,
    char inPrintErrorMessage ) {

    char *fileContents =
        LevelDirectoryManager::readFileContents( inFileName,
                                                 inPrintErrorMessage );

    double returnValue = 0;

    
    if( fileContents != NULL ) {


        int numRead = sscanf( fileContents, "%lf", &returnValue );

        if( numRead != 1 ) {
            *outError = true;

            if( inPrintErrorMessage ) {
                printf( "Error reading double from file %s\n", inFileName );
                }
            }
        else {
            *outError = false;
            }
        
        delete [] fileContents;
        }
    else {
        *outError = true;
        }

    

    return returnValue;
    }



int LevelDirectoryManager::readIntFileContents(
    char *inFileName,
    char *outError,
    char inPrintErrorMessage ) {

    char *fileContents =
        LevelDirectoryManager::readFileContents( inFileName,
                                                 inPrintErrorMessage );

    int returnValue = 0;

    
    if( fileContents != NULL ) {


        int numRead = sscanf( fileContents, "%d", &returnValue );

        if( numRead != 1 ) {
            *outError = true;

            if( inPrintErrorMessage ) {
                printf( "Error reading int from file %s\n", inFileName );
                }
            }
        else {
            *outError = false;
            }
        
        delete [] fileContents;
        }
    else {
        *outError = true;
        }

    

    return returnValue;
    }



StaticLevelDirectoryFileWrapper::StaticLevelDirectoryFileWrapper() {
    mFile = NULL;
    }


        
StaticLevelDirectoryFileWrapper::~StaticLevelDirectoryFileWrapper() {
    if( mFile != NULL ) {
        delete mFile;
        }
    }

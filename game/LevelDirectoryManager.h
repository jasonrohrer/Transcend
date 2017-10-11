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



#ifndef LEVEL_DIRECTORY_MANAGER_INCLUDED
#define LEVEL_DIRECTORY_MANAGER_INCLUDED



#include "minorGems/io/file/File.h"


#include <stdio.h>



/**
 * A wrapper class to ensure destruction of a file object at system exit.
 */
class StaticLevelDirectoryFileWrapper {

    public:

        StaticLevelDirectoryFileWrapper();
        
        ~StaticLevelDirectoryFileWrapper();

        File *mFile;
    };



/**
 * A class with static functions for setting and obtaining the current
 * level directory.
 *
 * @author Jason Rohrer.
 */
class LevelDirectoryManager {


    public:


        
        /**
         * Sets the directory where level files are stored.
         *
         * @param inFile the directory file object, or NULL to switch
         *   back to the default location.
         *   Will be destroyed by this class.
         */
        static void setLevelDirectory( File *inFile );

        

        /**
         * Gets the current level directory.
         *
         * @return the directory file object.
         *   Must be destroyed by caller.
         */
        static File *getLevelDirectory();


        /**
         * Gets a File object for a level file.
         *
         * @param inFileName the name of the file.
         *   Must be destroyed by caller.
         * @param inShowErrorMessage true to automatically print an error
         *   message to std out.  Defaults to false.
         *
         * @return the stream, or NULL if getting the file from the directory
         *   fails. Must be closed by caller.
         */
        static File *getLevelFile( char *inFileName,
                                   char inPrintErrorMessage = false );


        
        /**
         * Gets a std stream for a level file.
         *
         * @param inFileName the name of the file.
         *   Must be destroyed by caller.
         * @param inShowErrorMessage true to automatically print an error
         *   message to std out.  Defaults to false.
         *
         * @return the stream, or NULL if opening the stream fails.
         *   Must be closed by caller.
         */
        static FILE *getStdStream( char *inFileName,
                                   char inPrintErrorMessage = false );


        
        /**
         * Reads the contents of a level file as a string.
         *
         * @param inFileName the name of the file.
         *   Must be destroyed by caller.
         * @param inShowErrorMessage true to automatically print an error
         *   message to std out.  Defaults to false.
         *
         * @return the file contents, or NULL if reading from the file fails.
         *   Must be destroyed by caller.
         */
        static char *readFileContents( char *inFileName,
                                       char inPrintErrorMessage = false );

        

        /**
         * Reads a single double value from a level file.
         *
         * @param inFileName the name of the file.
         *   Must be destroyed by caller.
         * @param outError pointer to where the error flag should be returned.
         *   Will be set to true if reading a double from the file fails.
         * @param inShowErrorMessage true to automatically print an error
         *   message to std out.  Defaults to false.
         *
         * @return the first double value in the file.
         */
        static double readDoubleFileContents(
            char *inFileName,
            char *outError,
            char inPrintErrorMessage = false );



        /**
         * Reads a single int value from a level file.
         *
         * @param inFileName the name of the file.
         *   Must be destroyed by caller.
         * @param outError pointer to where the error flag should be returned.
         *   Will be set to true if reading a double from the file fails.
         * @param inShowErrorMessage true to automatically print an error
         *   message to std out.  Defaults to false.
         *
         * @return the first int value in the file.
         */
        static int readIntFileContents(
            char *inFileName,
            char *outError,
            char inPrintErrorMessage = false );

        
        
    protected:

        static StaticLevelDirectoryFileWrapper mFileWrapper;

        
    };



#endif

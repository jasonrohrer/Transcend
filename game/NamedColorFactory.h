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
 */



#ifndef NAMED_COLOR_FACTORY_INCLUDED
#define NAMED_COLOR_FACTORY_INCLUDED



#include "minorGems/graphics/Color.h"



/**
 * A class with static functions for mapping names to colors using
 * named color files on disk (currently hard coded to look in the "colors"
 * directory).
 *
 * @author Jason Rohrer.
 */
class NamedColorFactory {


    public:

        

        /**
         * Maps a color name to a color.
         *
         * @param inColorName the name of the color.
         *   Must be destroyed by caller.
         *
         * @return the color, or NULL if no color file can be found for
         *   inColorName.
         *   Must be destroyed by caller if non-NULL.
         */
        static Color *getColor( char *inColorName );


        
    };



#endif

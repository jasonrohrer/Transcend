/*
 * Modification History
 *
 * 2004-June-18   Jason Rohrer
 * Created.
 *
 * 2004-August-24   Jason Rohrer
 * Added extra parameter for enemy distance from ship.
 */



#ifndef ENEMY_INCLUDED
#define ENEMY_INCLUDED



#include "ParameterizedObject.h"



/**
 * An enemy controled with 2 parameters (one for the general shape
 * of the enemy, and one to control the shape of the enemy's explosion).
 *
 * @author Jason Rohrer.
 */
class Enemy {


    public:


        
        /**
         * Constructs an enemy by reading values from a text file
         * stream.
         *
         * @param inFILE the open file to read from.
         *   Must be closed by caller.
         * @param outError pointer to where error flag should be returned.
         *   Destination will be set to true if reading the bullet
         *   from inFILE fails.
         */
        Enemy( FILE *inFILE, char *outError );


        
        virtual ~Enemy();


        
        /**
         * Gets drawable objects for this enemy.
         *
         * @param inEnemyShapeParameter a parameter in the range [0,1] to
         *   control the shape of the enemy.
         * @param inEnemyDistanceFromShipParameter a parameter in the range
         *   [0,1] representing how far the enemy is from the ship, where 0
         *   is close and 1 is far.
         * @param inExplosionShapeParameter a parameter in the range [0,1] to
         *   control the shape of the enemy's explosion.
         * @param inExplosionProgress how close the enemy is towards a
         *   completed explosion, in the range [0,1].
         * @param outRotationRate pointer to where the enemy rotation
         *   rate should be returned.
         *
         * @return this enemy as a collection of drawable objects.
         *   Vector and objects must be destroyed by caller.
         */
        SimpleVector<DrawableObject *> *getDrawableObjects(
            double inEnemyShapeParameter,
            double inEnemyDistanceFromShipParameter,
            double inExplosionShapeParameter,
            double inExplosionProgress,
            double *outRotationRate );


        
    protected:

        ParameterizedObject *mEnemyCloseShapeObject;
        ParameterizedObject *mEnemyFarShapeObject;
        ParameterizedObject *mExplosionShapeObject;

        
    };



#endif

#pragma once
#include "Chunk.h"
#include "Utils/Direction.h"


/**
 * \brief A very simple, improvised state machine returning successive points
 *  relative to a reference point (origin) in a spiral motion.
 *
 *  Used, for example, to properly generate chunks in the right order.
 *
 * For example. Assume origin is 0,0 it returns points in this order:
 *   432.
 *   501.
 *	 6789
 */
class CoordinatesAroundOriginGetter
{
public:
	CoordinatesAroundOriginGetter(sf::Vector3i origin);

    /**
     * Returns the next state of the spiral - that is, the next object in xyz coordinates
     * @return The xyz value of the next 3D element of the spiral
     */
	sf::Vector3i nextValue();

    /**
     * The spiral begins from the beginning
     */
    void resetToStart();

private:
	sf::Vector3i mCurrentChunkPosition;
	sf::Vector3i mChangedPosition{0, 0, 0 };
	int mCurrentLineLength = 0;
	Direction mCurrentDirection = Direction::ToTheRight;
};

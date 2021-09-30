#pragma once
#include "Chunk.h"


/**
 * \brief A very simple, improvised state machine returning successive points
 *  relative to a reference point (origin) in a spiral motion.
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

	sf::Vector3i getNextValue();
	void resetToStart();

private:
	sf::Vector3i currentChunkPosition;
	sf::Vector3i changedPosition{ 0, 0, 0 };
	int currentLineLength = 0;
	Direction currentDirection = Direction::ToTheRight;
};

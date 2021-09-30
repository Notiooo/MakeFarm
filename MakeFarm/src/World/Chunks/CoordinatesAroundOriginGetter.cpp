#include "pch.h"
#include "CoordinatesAroundOriginGetter.h"

CoordinatesAroundOriginGetter::CoordinatesAroundOriginGetter(sf::Vector3i origin): currentChunkPosition(origin)
{}

void CoordinatesAroundOriginGetter::resetToStart()
{
	changedPosition = { 0, 0, 0 };
	currentLineLength = 0;
	currentDirection = Direction::ToTheRight;
}

sf::Vector3i CoordinatesAroundOriginGetter::getNextValue()
{
	if (currentLineLength == 0)
	{
		currentLineLength = 1;
		return currentChunkPosition;
	}

	switch (currentDirection)
	{
	case Direction::ToTheRight:
		changedPosition.x += 1;
		if (changedPosition.x == currentLineLength)
		{
			currentDirection = Direction::InFront;
		}
		break;

	case Direction::InFront:
		changedPosition.z += 1;
		if (changedPosition.z == currentLineLength)
			currentDirection = Direction::ToTheLeft;
		break;

	case Direction::ToTheLeft:
		changedPosition.x -= 1;
		if (changedPosition.x == -currentLineLength)
			currentDirection = Direction::Behind;
		break;

	case Direction::Behind:
		changedPosition.z -= 1;
		if (changedPosition.z == -currentLineLength)
		{
			currentDirection = Direction::ToTheRight;
			++currentLineLength;
		}
		break;
	}
	return currentChunkPosition + changedPosition;
}

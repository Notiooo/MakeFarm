#include "pch.h"
#include "CoordinateBase.h"

CoordinateBase::CoordinateBase(IntegerUnit x, IntegerUnit y, IntegerUnit z)
	: mBlockCoordinates({ x, y, z })
{
}

CoordinateBase::CoordinateBase(const sf::Vector3i& blockCoordinates)
	: mBlockCoordinates(blockCoordinates)
{

}

CoordinateBase::CoordinateBase(CoordinateBase&& rhs) noexcept
	: mBlockCoordinates(std::move(rhs.mBlockCoordinates))
	, x(mBlockCoordinates.x)
	, y(mBlockCoordinates.y)
	, z(mBlockCoordinates.z)
{
}

CoordinateBase::CoordinateBase(const CoordinateBase& rhs) noexcept
	: mBlockCoordinates(rhs.mBlockCoordinates)
	, x(mBlockCoordinates.x)
	, y(mBlockCoordinates.y)
	, z(mBlockCoordinates.z)
{
}

CoordinateBase& CoordinateBase::operator=(const CoordinateBase& rhs)
{
	mBlockCoordinates = rhs.mBlockCoordinates;
	return *this;
}

CoordinateBase CoordinateBase::operator-(const CoordinateBase& rhs) const
{
	return mBlockCoordinates - rhs.mBlockCoordinates;
}

CoordinateBase CoordinateBase::operator+(const CoordinateBase& rhs) const
{
	return mBlockCoordinates + rhs.mBlockCoordinates;
}

bool CoordinateBase::operator==(const CoordinateBase& rhs) const
{
	return mBlockCoordinates == rhs.mBlockCoordinates;
}

bool CoordinateBase::operator!=(const CoordinateBase& rhs) const
{
	return !(operator==(rhs));
}
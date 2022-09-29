#pragma once
#include <SFML/System/Vector3.hpp>

/**
 * A base class of coordinate systems in 3D space.
 * Works correctly in hashmap and has correctly created arithmetic and comparison operators.
 */
struct CoordinateBase
{
	virtual ~CoordinateBase() = default;
	using IntegerUnit = int;

	CoordinateBase(IntegerUnit x, IntegerUnit y, IntegerUnit z);
	CoordinateBase(const sf::Vector3i& blockCoordinates);

	CoordinateBase(CoordinateBase&&) noexcept;
	CoordinateBase(const CoordinateBase&) noexcept;

	operator sf::Vector3<IntegerUnit>() const { return mBlockCoordinates; }

	CoordinateBase& operator=(const CoordinateBase& rhs);
	CoordinateBase operator-(const CoordinateBase& rhs) const;
	CoordinateBase operator+(const CoordinateBase& rhs) const;
	bool operator==(const CoordinateBase& rhs) const;
	bool operator!=(const CoordinateBase& rhs) const;

private:
	sf::Vector3<IntegerUnit> mBlockCoordinates;

public:
	IntegerUnit& x = mBlockCoordinates.x;
	IntegerUnit& y = mBlockCoordinates.y;
	IntegerUnit& z = mBlockCoordinates.z;
};

namespace std {

	template <>
	struct hash<CoordinateBase>
	{
		std::size_t operator()(const CoordinateBase& k) const noexcept
		{
			using IntegerUnit = CoordinateBase::IntegerUnit;

			return ((hash<IntegerUnit>()(k.x)
				  ^ (hash<IntegerUnit>()(k.y) << 1)) >> 1)
				  ^ (hash<IntegerUnit>()(k.z) << 1);
		}
	};

}
#include "EventEqualityOperator.h"

bool operator==(const sf::Event& lhs, const sf::Event& rhs)
{
    return lhs.type == rhs.type;
}
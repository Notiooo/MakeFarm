#ifndef UTILS_H
#define UTILS_H
#include <SFML/Graphics/Rect.hpp>

/**
 * \brief Sets the origin to the centre of the drawing object.
 * \tparam T Drawable object type
 * \param drawable_obj Object to centre
 */
template<typename T>
void centerOrigin(T& drawable_obj)
{
    const sf::FloatRect rect = drawable_obj.getLocalBounds();
    drawable_obj.setOrigin(rect.left + rect.width / 2.f, rect.top + rect.height / 2.f);
}

#endif
#pragma once

#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <SFML/System/Vector2.hpp>

#include "PhysicsMgr.h"

class VoidBody : public Collider
{
public:
    using Ptr = std::shared_ptr<VoidBody>;

public:
    static Ptr Create(const sf::Vector2f &position);

private:
};
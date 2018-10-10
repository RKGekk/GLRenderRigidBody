#ifndef COLLISIONBOX_H
#define COLLISIONBOX_H

#include "CollisionPrimitive.h"

class CollisionBox : public CollisionPrimitive {
public:

	glm::vec3 halfSize;
};

#endif // !COLLISIONBOX_H


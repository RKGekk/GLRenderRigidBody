#include "CollisionPrimitive.h"

void CollisionPrimitive::calculateInternals() {
	transform = offset * body->getTransform();
}
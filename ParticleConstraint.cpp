#include "ParticleConstraint.h"

float ParticleConstraint::currentLength() const {

    glm::vec3 relativePos = particle->getPosition() - anchor;
    return glm::length(relativePos);
}
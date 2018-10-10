#ifndef PARTICLECONSTRAINT_H
#define PARTICLECONSTRAINT_H

#include "Particle.h"
#include "ParticleContactGenerator.h"
#include "glm/glm.hpp"

class ParticleConstraint : public ParticleContactGenerator {

public:
	Particle *particle;
	glm::vec3 anchor;

protected:
	float currentLength() const;

public:
	virtual unsigned addContact(ParticleContact *contact, unsigned limit) const = 0;

};

#endif // !PARTICLECONSTRAINT_H


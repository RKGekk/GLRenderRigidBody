#ifndef PARTICLECABLECONSTRAINT_H
#define PARTICLECABLECONSTRAINT_H

#include "Particle.h"
#include "ParticleContactGenerator.h"
#include "ParticleConstraint.h"
#include "glm/glm.hpp"

class ParticleCableConstraint : public ParticleConstraint {

public:
	float maxLength;
	float restitution;

public:
	virtual unsigned addContact(ParticleContact *contact, unsigned limit) const;
};

#endif // !PARTICLECABLECONSTRAINT_H


#ifndef PARTICLECABLE_H
#define PARTICLECABLE_H

#include "Particle.h"
#include "ParticleContactGenerator.h"
#include "ParticleLink.h"
#include "glm/glm.hpp"

class ParticleCable : public ParticleLink {

public:
	float maxLength;
	float restitution;

public:
	virtual unsigned addContact(ParticleContact *contact, unsigned limit) const;

};


#endif // !PARTICLECABLE_H


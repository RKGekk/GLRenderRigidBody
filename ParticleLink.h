#ifndef PARTICLELINK_H
#define PARTICLELINK_H

#include "Particle.h"
#include "ParticleContactGenerator.h"
#include "glm/glm.hpp"

class ParticleLink : public ParticleContactGenerator {

public:
	Particle *particle[2];

protected:
	float currentLength() const;

public:
	virtual unsigned addContact(ParticleContact *contact, unsigned limit) const = 0;
};


#endif // !PARTICLELINK_H


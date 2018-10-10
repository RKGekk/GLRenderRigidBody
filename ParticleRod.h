#ifndef PARTICLEROD_H
#define PARTICLEROD_H

#include "Particle.h"
#include "ParticleContactGenerator.h"
#include "ParticleLink.h"
#include "glm/glm.hpp"

class ParticleRod : public ParticleLink {

public:
	float length;

public:
	virtual unsigned addContact(ParticleContact *contact, unsigned limit) const;

};

#endif // !PARTICLEROD_H


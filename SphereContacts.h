#ifndef SPHERECONTACTS_H
#define SPHERECONTACTS_H

#include "Particle.h"
#include "ParticleContact.h"
#include "ParticleContactGenerator.h"
#include "ParticleWorld.h"
#include "glm/glm.hpp"

class SphereContacts : public ParticleContactGenerator {

private:
	ParticleWorld::Particles *particles;

public:
	void init(ParticleWorld::Particles *particles);

	virtual unsigned addContact(ParticleContact *contact, unsigned limit) const;
};

#endif // !SPHEREDCONTACTS_H


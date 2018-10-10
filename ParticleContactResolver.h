#ifndef PARTICLECONTACTRESOLVER_H
#define PARTICLECONTACTRESOLVER_H

#include "Particle.h"
#include "ParticleContact.h"
#include "glm/glm.hpp"

class ParticleContactResolver {

protected:
	unsigned iterations;
	unsigned iterationsUsed;

public:
	ParticleContactResolver(unsigned iter);

	void setIterations(unsigned iter);
	void resolveContacts(ParticleContact *contactArray, unsigned numContacts, float duration);

};

#endif // !PARTICLECONTACTRESOLVER_H

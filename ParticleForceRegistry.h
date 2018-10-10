#ifndef PARTICLEFORCEREGISTRY_H
#define PARTICLEFORCEREGISTRY_H

#include <vector>

#include "Particle.h"
#include "ParticleForceGenerator.h"

class ParticleForceRegistry {

protected:
	struct ParticleForceRegistration {
		Particle				*particle;
		ParticleForceGenerator	*forceGenerator;
	};

	typedef std::vector<ParticleForceRegistration> Registry;
	Registry registrations;

public:
	void add(Particle *particle, ParticleForceGenerator *forceGenerator);
	void remove(Particle *particle, ParticleForceGenerator *forceGenerator);
	void clear();
	void updateForces(float duration);
};

#endif 
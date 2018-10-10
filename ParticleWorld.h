#ifndef PARTICLEWORLD_H
#define PARTICLEWORLD_H

#include <vector>

#include "Particle.h"
#include "ParticleForceGenerator.h"
#include "ParticleForceRegistry.h"
#include "ParticleContact.h"
#include "ParticleContactGenerator.h"
#include "ParticleContactResolver.h"
#include "glm/glm.hpp"

class ParticleWorld {

public:
	typedef std::vector<Particle *> Particles;
	typedef std::vector<ParticleContactGenerator *> ContactGenerators;

protected:
	Particles				particles;
	bool					calculateIterations;
	ParticleForceRegistry	registry;
	ParticleContactResolver	resolver;
	ContactGenerators		contactGenerators;
	ParticleContact			*contacts;
	unsigned				maxContacts;

public:
	ParticleWorld(unsigned maxContacts, unsigned iterations = 0);
	~ParticleWorld();

	unsigned generateContacts();
	void integrate(float duration);
	void runPhysics(float duration);
	void startFrame();
	Particles& getParticles();
	ContactGenerators& getContactGenerators();
	ParticleForceRegistry& getForceRegistry();
};

#endif // !PARTICLEWORLD_H


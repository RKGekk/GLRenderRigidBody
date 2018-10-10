#ifndef PARTICLEBUNGEE_H
#define PARTICLEBUNGEE_H

#include "Particle.h"
#include "ParticleForceGenerator.h"
#include "glm/glm.hpp"

class ParticleBungee : public ParticleForceGenerator {

private:
	Particle *other;
	float springConstant;
	float restLength;

public:
	ParticleBungee(Particle *anchoreParticle, float stiffness, float restDistance);
	ParticleBungee() = default;

	virtual void updateForce(Particle *particle, float duration);
};

#endif // !PARTICLEBUNGEE_H


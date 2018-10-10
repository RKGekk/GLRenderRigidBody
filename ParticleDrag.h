#ifndef PARTICLEDRAG_H
#define PARTICLEDRAG_H

#include "Particle.h"
#include "ParticleForceGenerator.h"
#include "glm/glm.hpp"

class ParticleDrag : public ParticleForceGenerator {

private:
	float k1;
	float k2;

public:
	ParticleDrag(float koef1, float koef2);
	ParticleDrag() = default;

	//ParticleDrag &ParticleDrag::operator =(const ParticleDrag &) = default;

	virtual void updateForce(Particle *particle, float duration);
};

#endif // !PARTICLEDRAG_H


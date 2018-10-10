#ifndef PARTICLEGRAVITY_H
#define PARTICLEGRAVITY_H

#include "ParticleForceGenerator.h"
#include "glm/glm.hpp"

class ParticleGravity : public ParticleForceGenerator {

private:
	glm::vec3 gravity;

public:
	ParticleGravity(const glm::vec3 &gravityG);
	ParticleGravity() = default;

	virtual void updateForce(Particle *particle, float duration);
};

#endif // !PARTICLEGRAVITY_H


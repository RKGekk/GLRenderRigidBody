#ifndef RANDOM_H
#define RANDOM_H

#include <math.h>
#include <limits>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

class Random {
public:

	unsigned rotl(unsigned n, unsigned r);
	unsigned rotr(unsigned n, unsigned r);

	Random();
	Random(unsigned seed);

	void seed(unsigned seed);
	unsigned randomBits();
	float randomReal();
	float randomReal(float scale);
	float randomReal(float min, float max);
	unsigned randomInt(unsigned max);
	float randomBinomial(float scale);
	glm::vec3 randomVector(float scale);
	glm::vec3 randomVector(const glm::vec3 &scale);
	glm::vec3 randomVector(const glm::vec3 &min, const glm::vec3 &max);
	glm::vec3 randomXZVector(float scale);
	glm::quat randomQuaternion();

private:
	
	int p1, p2;
	unsigned buffer[17];
};

#endif // !RANDOM_H


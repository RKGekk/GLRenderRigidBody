#include <cstdlib>
#include <ctime>
#include "Random.h"

Random::Random() {
	seed(0);
}

Random::Random(unsigned seed) {
	Random::seed(seed);
}

void Random::seed(unsigned s) {
	if (s == 0) {
		s = (unsigned)clock();
	}

	for (unsigned i = 0; i < 17; i++) {
		s = s * 2891336453 + 1;
		buffer[i] = s;
	}

	p1 = 0;  p2 = 10;
}

unsigned Random::rotl(unsigned n, unsigned r) {
	return	(n << r) | (n >> (32 - r));
}

unsigned Random::rotr(unsigned n, unsigned r) {
	return	(n >> r) | (n << (32 - r));
}

unsigned Random::randomBits() {
	unsigned result;

	result = buffer[p1] = rotl(buffer[p2], 13) + rotl(buffer[p1], 9);

	if (--p1 < 0) p1 = 16;
	if (--p2 < 0) p2 = 16;

	return result;
}

#ifdef SINGLE_PRECISION
real Random::randomReal() {

	unsigned bits = randomBits();

	union {
		real value;
		unsigned word;
	} convert;

	convert.word = (bits >> 9) | 0x3f800000;

	return convert.value - 1.0f;
}
#else
float Random::randomReal() {

	unsigned bits = randomBits();

	union {
		float value;
		unsigned words[2];
	} convert;

	convert.words[0] = bits << 20;
	convert.words[1] = (bits >> 12) | 0x3FF00000;

	return convert.value - 1.0f;
}
#endif

float Random::randomReal(float min, float max) {
	return randomReal() * (max - min) + min;
}

float Random::randomReal(float scale) {
	return randomReal() * scale;
}

unsigned Random::randomInt(unsigned max) {
	return randomBits() * max;
}

float Random::randomBinomial(float scale) {
	return (randomReal() - randomReal())*scale;
}

glm::quat Random::randomQuaternion() {
	glm::quat q(
		randomReal(),
		randomReal(),
		randomReal(),
		randomReal()
	);
	q = glm::normalize(q);
	return q;
}

glm::vec3 Random::randomVector(float scale) {
	return glm::vec3(
		randomBinomial(scale),
		randomBinomial(scale),
		randomBinomial(scale)
	);
}

glm::vec3 Random::randomXZVector(float scale) {
	return glm::vec3(
		randomBinomial(scale),
		0,
		randomBinomial(scale)
	);
}

glm::vec3 Random::randomVector(const glm::vec3 &scale) {
	return glm::vec3(
		randomBinomial(scale.x),
		randomBinomial(scale.y),
		randomBinomial(scale.z)
	);
}

glm::vec3 Random::randomVector(const glm::vec3 &min, const glm::vec3 &max) {
	return glm::vec3(
		randomReal(min.x, max.x),
		randomReal(min.y, max.y),
		randomReal(min.z, max.z)
	);
}

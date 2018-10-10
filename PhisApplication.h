#ifndef PHISAPPLICATION_H
#define PHISAPPLICATION_H

#include "Application.h"
#include "Shader.h"
#include "GeometryGenerator.h"
#include "Camera.h"
#include "GameTimer.h"
#include "Particle.h"
#include "ParticleGravity.h"
#include "ParticleDrag.h"
#include "ParticleSpring.h"
#include "ParticleForceGenerator.h"
#include "ParticleForceRegistry.h"
#include "ParticleAnchoredSpring.h"
#include "ParticleBungee.h"
#include "ParticleFakeSpring.h"
#include "ParticleContact.h"
#include "ParticleConstraint.h"
#include "ParticleContactGenerator.h"
#include "ParticleContactResolver.h"
#include "ParticleCableConstraint.h"
#include "ParticleCable.h"
#include "ParticleRod.h"
#include "ParticleWorld.h"
#include "GroundContacts.h"
#include "SphereContacts.h"
#include "Random.h"
#include "Contact.h"
#include "CollisionData.h"
#include "ContactResolver.h"
#include "CollisionBox.h"
#include "RigidBody.h"
#include "CollisionSphere.h"
#include "CollisionPlane.h"
#include "CollisionDetector.h"
#include "RenderItem.h"


#include <math.h>
#include <limits>

#include "glm/glm.hpp"

#define MAX_BLOCKS 9

//Random global_random;

class Block : public CollisionBox {
public:
	bool exists;

	Block() : exists(false) {
		body = new RigidBody();
	}

	~Block() {
		delete body;
	}

	void setState(const glm::vec3 &position, const glm::quat &orientation, const glm::vec3 &extents, const glm::vec3 &velocity) {

		body->setPosition(position);
		body->setOrientation(orientation);
		body->setVelocity(velocity);
		body->setRotation(glm::vec3(0, 0, 0));
		halfSize = extents;

		float mass = halfSize.x * halfSize.y * halfSize.z * 8.0f;
		body->setMass(mass);

		glm::mat3 tensor;

		glm::vec3 squares;
		squares.x = halfSize.x * halfSize.x;
		squares.y = halfSize.y * halfSize.y;
		squares.y = halfSize.y * halfSize.y;

		float ix = 0.3f * mass * (squares.y + squares.z);
		float iy = 0.3f * mass * (squares.x + squares.z);
		float iz = 0.3f * mass * (squares.x + squares.y);
        float ixy = 0.0f;
		float ixz = 0.0f;
		float iyz = 0.0f;

        tensor[0][0] = ix;
		tensor[0][1] = -ixy;
		tensor[0][2] = -ixz;

		tensor[1][0] = -ixy;
		tensor[1][1] = iy;
		tensor[1][2] = -iyz;

		tensor[2][0] = -ixz;
		tensor[2][1] = -iyz;
		tensor[2][2] = iz;

		body->setInertiaTensor(tensor);

		body->setLinearDamping(0.95f);
		body->setAngularDamping(0.8f);
		body->clearAccumulators();
		body->setAcceleration(0, -10.0f, 0);

		//body->setCanSleep(false);
		body->setAwake();

		body->calculateDerivedData();
	}

	void calculateMassProperties(float invDensity) {
		
		if (invDensity <= 0) {

			body->setInverseMass(0);
			body->setInverseInertiaTensor(glm::mat3());
		}
		else {

			float volume = glm::length(halfSize) * 2.0f;
			float mass = volume / invDensity;

			body->setMass(mass);

			mass *= 0.333f;
			glm::mat3 tensor;

			float ix = mass * halfSize.y * halfSize.y + halfSize.z * halfSize.z;
			float iy = mass * halfSize.y * halfSize.x + halfSize.z * halfSize.z;
			float iz = mass * halfSize.y * halfSize.x + halfSize.z * halfSize.y;
			float ixy = 0.0f;
			float ixz = 0.0f;
			float iyz = 0.0f;

			tensor[0][0] = ix;
			tensor[0][1] = -ixy;
			tensor[0][2] = -ixz;

			tensor[1][0] = -ixy;
			tensor[1][1] = iy;
			tensor[1][2] = -iyz;

			tensor[2][0] = -ixz;
			tensor[2][1] = -iyz;
			tensor[2][2] = iz;

			body->setInertiaTensor(tensor);
		}
	}

	void divideBlock(const Contact& contact, Block* target, Block* blocks) {
		
		glm::vec3 normal = contact.contactNormal;
		RigidBody *body = contact.body[0];

		if (body != target->body) {

			normal *= -1.0f;
			body = contact.body[1];
		}

		glm::vec3 point = body->getPointInLocalSpace(contact.contactPoint);
		normal = body->getDirectionInLocalSpace(normal);

		point = point - normal * glm::dot(point, normal);

		glm::vec3 size = target->halfSize;

		RigidBody tempBody;
		tempBody.setPosition(body->getPosition());
		tempBody.setOrientation(body->getOrientation());
		tempBody.setVelocity(body->getVelocity());
		tempBody.setRotation(body->getRotation());
		tempBody.setLinearDamping(body->getLinearDamping());
		tempBody.setAngularDamping(body->getAngularDamping());
		tempBody.setInverseInertiaTensor(body->getInverseInertiaTensor());
		tempBody.calculateDerivedData();

		target->exists = false;

		float invDensity = glm::length(halfSize) * 8.0f * body->getInverseMass();

		for (unsigned i = 0; i < 8; i++) {

			glm::vec3 min, max;
			if ((i & 1) == 0) {
				min.x = -size.x;
				max.x = point.x;
			}
			else {
				min.x = point.x;
				max.x = size.x;
			}
			if ((i & 2) == 0) {
				min.y = -size.y;
				max.y = point.y;
			}
			else {
				min.y = point.y;
				max.y = size.y;
			}
			if ((i & 4) == 0) {
				min.z = -size.z;
				max.z = point.z;
			}
			else {
				min.z = point.z;
				max.z = size.z;
			}

			glm::vec3 halfSize = (max - min) * 0.5f;
			glm::vec3 newPos = halfSize + min;

			newPos = tempBody.getPointInWorldSpace(newPos);

			glm::vec3 direction = newPos - contact.contactPoint;
			direction = glm::normalize(direction);

			blocks[i].body->setPosition(newPos);
			blocks[i].body->setVelocity(tempBody.getVelocity() + direction * 10.0f);
			blocks[i].body->setOrientation(tempBody.getOrientation());
			blocks[i].body->setRotation(tempBody.getRotation());
			blocks[i].body->setLinearDamping(tempBody.getLinearDamping());
			blocks[i].body->setAngularDamping(tempBody.getAngularDamping());
			blocks[i].body->setAwake(true);
			blocks[i].body->setAcceleration(glm::vec3(0.0f, -10.0f, 0.0f));
			blocks[i].body->clearAccumulators();
			blocks[i].body->calculateDerivedData();
			blocks[i].offset = glm::mat4();
			blocks[i].exists = true;
			blocks[i].halfSize = halfSize;

			blocks[i].calculateMassProperties(invDensity);
		}
	}
};

class PhisApplication : public Application {

private:

	unsigned int _VBO3;
	unsigned int _VAO3;

	Camera		*_camera;
	Shader		*_sh1;
	Shader		*_sh2;

	bool lockMouse;
	bool playAnim;

	GeometryGenerator::MeshData _cube3;
	int		_vertCount3;
	float	*_vertices3;
	RenderItem *dungeon;
	RenderItem *pipe;
	RenderItem *lamp;
	RenderItem *lamp2;
	RenderItem *lamp3;
	RenderItem *cube;
	RenderItem *cube2;
	RenderItem *cube3;
	RenderItem *cube4;
	RenderItem *cube5;

	RenderItem *cubeRod;
	RenderItem *cubeRodC;

	Particle		_particle1;
	ParticleGravity	_particle1Gravity;
	ParticleDrag	_particle1Drag;
	ParticleSpring	_particle1Spring;
	
	Particle		_particle2;
	ParticleGravity	_particle2Gravity;
	ParticleDrag	_particle2Drag;
	ParticleBungee	_particle2Spring1;
	ParticleBungee	_particle2Spring2;

	Particle		_particle3;
	ParticleGravity	_particle3Gravity;
	ParticleDrag	_particle3Drag;
	ParticleBungee	_particle3Spring1;

	ParticleForceRegistry _particleRegister;
	Particle				*particleArray;
	ParticleGravity			*particleGravityArray;
	ParticleCableConstraint	*supports;
	ParticleWorld			world;
	ParticleCable			*cables;
	ParticleRod				*rods;
	GroundContacts			groundContactGenerator;
	SphereContacts			sphereContactGenerator;

	const static unsigned maxContacts = 256;
	Contact contacts[maxContacts];
	CollisionData cData;
	ContactResolver resolver;
	bool renderDebugInfo;
	bool pauseSimulation;
	bool autoPauseSimulation;

	bool hit;
    bool ball_active;
    unsigned fracture_contact;

    Random random;
    Block blocks[MAX_BLOCKS];
    CollisionSphere ball;

public:
	PhisApplication();

	PhisApplication(const PhisApplication &rhs) = delete;
	PhisApplication &operator=(const PhisApplication &rhs) = delete;
	~PhisApplication();

	virtual bool initialize() override;

	virtual void generateContacts();
	virtual void updateObjects(float duration);
	virtual void reset();

	virtual void onResize() override;
	virtual void update(const GameTimer &gt) override;
	virtual void draw(const GameTimer & gt) override;
	virtual void mouse(int button, int state, int x, int y) override;
	virtual void onKeyboardInput(GLFWwindow *window);

	void updateCamera(const GameTimer &gt);
	void animateMaterials(const GameTimer& gt);
	void animateGeometry(const GameTimer& gt);
	void loadTextures();
	void loadModels();
	void loadShaders();
	void buildGeometry();
	void buildBuffers();
	void buildCamera();
};

#endif
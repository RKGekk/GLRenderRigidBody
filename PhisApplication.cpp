#include "PhisApplication.h"

PhisApplication::PhisApplication() : Application(), resolver(maxContacts * 8), renderDebugInfo(false), pauseSimulation(false), autoPauseSimulation(false), world(30) {

	lockMouse = false;
	playAnim = false;

	cData.contactArray = contacts;

	ball.body = new RigidBody();
	ball.radius = 0.25f;
	ball.body->setMass(5.0f);
	ball.body->setDamping(0.9f, 0.9f);
	glm::mat3 it;
	it[0][0] = 5.0f;
	it[1][1] = 5.0f;
	it[2][2] = 5.0f;

	ball.body->setInertiaTensor(it);
	ball.body->setAcceleration(glm::vec3(0.0f, -10.0f, 0.0f));

	ball.body->setCanSleep(false);
	ball.body->setAwake(true);

	supports = nullptr;
	cables = nullptr;
	rods = nullptr;


	unsigned particleCount = 4;

	particleArray = new Particle[particleCount];
	particleGravityArray = new ParticleGravity[particleCount];
	for (unsigned i = 0; i < particleCount; i++) {
		world.getParticles().push_back(particleArray + i);
	}

	groundContactGenerator.init(&world.getParticles());
	sphereContactGenerator.init(&world.getParticles());

	world.getContactGenerators().push_back(&groundContactGenerator);
	world.getContactGenerators().push_back(&sphereContactGenerator);

	reset();
}

PhisApplication::~PhisApplication() {

	delete _sh1;
	delete _sh2;
	delete _vertices3;
	delete _camera;

	delete this->dungeon;
	delete this->cube;

	glDeleteVertexArrays(1, &_VAO3);
	glDeleteBuffers(1, &_VBO3);
}

bool PhisApplication::initialize() {

	if (!Application::initialize()) {
		return false;
	}

	glEnable(GL_DEPTH_TEST);

	loadShaders();
	loadTextures();
	loadModels();
	buildGeometry();
	buildBuffers();
	buildCamera();

	return true;
}

void PhisApplication::onResize() {
	Application::onResize();
}

void PhisApplication::update(const GameTimer &gt) {

	float dt = gt.deltaTime();

	static int repeatCount = 0;
	if (_camera->Position.z < -132.0f) {

		float oldZPos = _camera->Position.z;

		_camera->Position.z = 0.0f;

		pipe->positions.at(0).z += 132.0f;
		pipe->positions.at(1).z += 132.0f;
		pipe->positions.at(2).z += 132.0f;
		pipe->positions.at(3).z += 132.0f;

		pipe->positions.at(4).z += 132.0f;
		pipe->positions.at(5).z += 132.0f;
		pipe->positions.at(6).z += 132.0f;
		pipe->positions.at(7).z += 132.0f;

		if (pipe->positions.at(0).z > 0.0f) {
			pipe->positions.at(0).z -= 132.0f * 2.0f;
		}
		if (pipe->positions.at(1).z > 0.0f) {
			pipe->positions.at(1).z -= 132.0f * 2.0f;
		}
		if (pipe->positions.at(2).z > 0.0f) {
			pipe->positions.at(2).z -= 132.0f * 2.0f;
		}
		if (pipe->positions.at(3).z > 0.0f) {
			pipe->positions.at(3).z -= 132.0f * 2.0f;
		}
		if (pipe->positions.at(4).z > 0.0f) {
			pipe->positions.at(4).z -= 132.0f * 2.0f;
		}
		if (pipe->positions.at(5).z > 0.0f) {
			pipe->positions.at(5).z -= 132.0f * 2.0f;
		}
		if (pipe->positions.at(6).z > 0.0f) {
			pipe->positions.at(6).z -= 132.0f * 2.0f;
		}
		if (pipe->positions.at(7).z > 0.0f) {
			pipe->positions.at(7).z -= 132.0f * 2.0f;
		}


		++repeatCount;
	}


	if (dt <= 0.0f) {
		return;
	}
	else if (dt > 0.5f) {
		dt = 0.5f;
	}

	if (pauseSimulation) {
		return;
	}
	else if (autoPauseSimulation) {
		pauseSimulation = true;
		autoPauseSimulation = false;
	}

	world.startFrame();
    float duration = dt;
    world.runPhysics(duration);

	updateObjects(dt);

	generateContacts();

	resolver.resolveContacts(
		cData.contactArray,
		cData.contactCount,
		dt
	);

	if (hit) {
		blocks[0].divideBlock(
			cData.contactArray[fracture_contact],
			blocks,
			blocks + 1
		);
		ball_active = false;
	}
}

void PhisApplication::draw(const GameTimer &gt) {

	float dt = gt.deltaTime();
	static float time = 0.0f;
	static float bpm = 175.0f;
	static float bpm2 = 175.0f * 4.0f;

	time += dt;
	float beat = glm::clamp(glm::abs(glm::sin(time * glm::pi<float>() * (1.0f / (60.0f / bpm)))), 0.0f, 1.0f);
	float beat2 = glm::clamp(glm::abs(glm::sin(time * glm::pi<float>() * (1.0f / (60.0f / bpm2)))), 0.0f, 1.0f);

	//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClearColor(0.027451f, 0.039216f, 0.058824f, 1.0f);
	//glClearColor(0.031373f, 0.047059f, 0.086275f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(_VAO3);

	_sh2->use();
	_sh2->setVec3("viewPos", _camera->Position);

	_sh2->setFloat("material.shininess", 16.0f);

	_sh2->setVec3("dirLight.direction", 0.2f, -0.2f, 0.3f);
	_sh2->setVec3("dirLight.ambient", 0.15f, 0.15f, 0.15f);
	_sh2->setVec3("dirLight.diffuse", 0.192f, 0.184f, 0.176f);
	_sh2->setVec3("dirLight.specular", 0.1f, 0.1f, 0.1f);

	_sh2->setVec3("spotLight1.position", _camera->Position);
	_sh2->setVec3("spotLight1.direction", _camera->Front);
	_sh2->setVec3("spotLight1.ambient", 0.0f, 0.0f, 0.0f);
	_sh2->setVec3("spotLight1.diffuse", 1.0f, 1.0f, 1.0f);
	_sh2->setVec3("spotLight1.specular", 1.0f, 1.0f, 1.0f);
	_sh2->setFloat("spotLight1.constant", 1.0f);
	_sh2->setFloat("spotLight1.linear", 0.0009);
	_sh2->setFloat("spotLight1.quadratic", 0.00032);
	_sh2->setFloat("spotLight1.cutOff", glm::cos(glm::radians(12.5f)));
	_sh2->setFloat("spotLight1.outerCutOff", glm::cos(glm::radians(35.0f)));

	_sh2->setVec3("spotLight2.position", particleArray[3].getPosition());
	_sh2->setVec3("spotLight2.direction", glm::normalize(particleArray[3].getPosition() - particleArray[2].getPosition()));
	_sh2->setVec3("spotLight2.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	_sh2->setVec3("spotLight2.diffuse", glm::vec3(0.96f, 0.94f, 0.5f));
	_sh2->setVec3("spotLight2.specular", glm::vec3(0.95f, 0.945f, 0.51f));
	_sh2->setFloat("spotLight2.constant", 0.05f);
	_sh2->setFloat("spotLight2.linear", 0.0009f);
	_sh2->setFloat("spotLight2.quadratic", 0.00032f * beat);
	_sh2->setFloat("spotLight2.cutOff", glm::cos(glm::radians(6.5f)));
	_sh2->setFloat("spotLight2.outerCutOff", glm::cos(glm::radians(35.0f)));

	_sh2->setVec3("spotLight3.position", particleArray[3].getPosition() - glm::vec3(0.0f, 0.0f, 132.0f));
	_sh2->setVec3("spotLight3.direction", glm::normalize(particleArray[3].getPosition() - particleArray[2].getPosition()));
	_sh2->setVec3("spotLight3.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	_sh2->setVec3("spotLight3.diffuse", glm::vec3(0.96f, 0.94f, 0.5f));
	_sh2->setVec3("spotLight3.specular", glm::vec3(0.95f, 0.945f, 0.51f));
	_sh2->setFloat("spotLight3.constant", 0.05f);
	_sh2->setFloat("spotLight3.linear", 0.0009f);
	_sh2->setFloat("spotLight3.quadratic", 0.00032f * beat2);
	_sh2->setFloat("spotLight3.cutOff", glm::cos(glm::radians(6.5f)));
	_sh2->setFloat("spotLight3.outerCutOff", glm::cos(glm::radians(35.0f)));

	_sh2->setVec3("spotLight4.position", particleArray[3].getPosition() - glm::vec3(0.0f, 0.0f, 132.0f * 2.0f));
	_sh2->setVec3("spotLight4.direction", glm::normalize(particleArray[3].getPosition() - particleArray[2].getPosition()));
	_sh2->setVec3("spotLight4.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	_sh2->setVec3("spotLight4.diffuse", glm::vec3(0.96f, 0.94f, 0.5f));
	_sh2->setVec3("spotLight4.specular", glm::vec3(0.95f, 0.945f, 0.51f));
	_sh2->setFloat("spotLight4.constant", 0.05f);
	_sh2->setFloat("spotLight4.linear", 0.0009f);
	_sh2->setFloat("spotLight4.quadratic", 0.00032f * beat);
	_sh2->setFloat("spotLight4.cutOff", glm::cos(glm::radians(6.5f)));
	_sh2->setFloat("spotLight4.outerCutOff", glm::cos(glm::radians(35.0f)));

	_sh2->setVec3("spotLight5.position", particleArray[3].getPosition() - glm::vec3(0.0f, 0.0f, 132.0f * 3.0f));
	_sh2->setVec3("spotLight5.direction", glm::normalize(particleArray[3].getPosition() - particleArray[2].getPosition()));
	_sh2->setVec3("spotLight5.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	_sh2->setVec3("spotLight5.diffuse", glm::vec3(0.96f, 0.94f, 0.5f));
	_sh2->setVec3("spotLight5.specular", glm::vec3(0.95f, 0.945f, 0.51f));
	_sh2->setFloat("spotLight5.constant", 0.05f);
	_sh2->setFloat("spotLight5.linear", 0.0009f);
	_sh2->setFloat("spotLight5.quadratic", 0.00032f * beat);
	_sh2->setFloat("spotLight5.cutOff", glm::cos(glm::radians(6.5f)));
	_sh2->setFloat("spotLight5.outerCutOff", glm::cos(glm::radians(35.0f)));

	glActiveTexture(GL_TEXTURE0);

	glm::mat4 projection = glm::perspective(glm::radians(_camera->Zoom), ((float)_width) / ((float)_height), 0.1f, 1000.0f);
	glm::mat4 view = _camera->GetViewMatrix();
	_sh2->setMat4("view", view);
	_sh2->setMat4("projection", projection);

	if (blocks[0].exists || !playAnim) {
		blocks[0].body->getTransform(&cube->extModel);
		cube->extModel = glm::transpose(cube->extModel);
		cube->extModel = glm::scale(cube->extModel, glm::vec3(4.0f, 4.0f, 4.0f));
	}

	if (blocks[1].exists || !playAnim) {
		blocks[1].body->getTransform(&cube2->extModel);
		cube2->extModel = glm::transpose(cube2->extModel);
		cube2->extModel = glm::scale(cube2->extModel, glm::vec3(4.0f, 4.0f, 4.0f));
	}

	if (blocks[2].exists || !playAnim) {
		blocks[2].body->getTransform(&cube3->extModel);
		cube3->extModel = glm::transpose(cube3->extModel);
		cube3->extModel = glm::scale(cube3->extModel, glm::vec3(4.0f, 4.0f, 4.0f));
	}

	if (blocks[3].exists || !playAnim) {
		blocks[3].body->getTransform(&cube4->extModel);
		cube4->extModel = glm::transpose(cube4->extModel);
		cube4->extModel = glm::scale(cube4->extModel, glm::vec3(4.0f, 4.0f, 4.0f));
	}

	if (blocks[4].exists || !playAnim) {
		blocks[4].body->getTransform(&cube5->extModel);
		cube5->extModel = glm::transpose(cube5->extModel);
		cube5->extModel = glm::scale(cube5->extModel, glm::vec3(4.0f, 4.0f, 4.0f));
	}

	this->dungeon->draw(_sh2);
	if(!playAnim) {
		this->cube->draw(_sh2);
		this->cube2->draw(_sh2);
		this->cube3->draw(_sh2);
		this->cube4->draw(_sh2);
		this->cube5->draw(_sh2);
	}


	this->pipe->draw(_sh2);

	this->lamp->draw(_sh2);
	this->lamp2->draw(_sh2);
	this->lamp3->draw(_sh2);
	this->cubeRod->draw(_sh2);
	this->cubeRodC->draw(_sh2);

	/*
	glm::mat4 model;


	blocks[0].body->getTransform(&model);
	model = glm::transpose(model);
	model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));

	_sh2->setMat4("model", model);
	_sh2->setMat4("view", view);
	_sh2->setMat4("projection", projection);

	glDrawArrays(GL_TRIANGLES, 0, _vertCount3 * 3);*/
}

void PhisApplication::onKeyboardInput(GLFWwindow *window) {

	static float velocity = 8.0f;
	static glm::vec3 forwardDirection = glm::vec3(0.0f, 0.0f, -1.0f);

	float dt = _timer.deltaTime();

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		lockMouse = true;

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		lockMouse = false;

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		playAnim = true;


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (!lockMouse) {
			_camera->ProcessKeyboard(FORWARD, dt * velocity);
		}
		else {
			_camera->ProcessKeyboard(forwardDirection, dt * velocity);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (!lockMouse) {
			_camera->ProcessKeyboard(BACKWARD, dt * velocity);
		}
		else {
			_camera->ProcessKeyboard(-forwardDirection, dt * velocity);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		_camera->ProcessKeyboard(LEFT, dt * velocity);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		_camera->ProcessKeyboard(RIGHT, dt * velocity);



	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {

		/* block 4 */
		blocks[4].exists = true;
		blocks[4].body->setPosition(_camera->Position + _camera->Front * 6.0f);
		blocks[4].body->setVelocity(_camera->Front * 32.0f);
		blocks[4].body->calculateDerivedData();
		blocks[4].calculateInternals();

		blocks[4].body->setAcceleration(glm::vec3(0.0f, -10.0f, 0.0f));
		blocks[4].body->setAwake(true);
		blocks[4].body->setCanSleep(true);
	}

}

void PhisApplication::mouse(int button, int state, int x, int y) {

	if (!lockMouse) {
		if (firstMouse) {
			lastX = x;
			lastY = y;
			firstMouse = false;
		}

		float xoffset = x - lastX;
		float yoffset = lastY - y;

		lastX = x;
		lastY = y;

		_camera->ProcessMouseMovement(xoffset, yoffset);
	}
}

void PhisApplication::updateCamera(const GameTimer &gt) {

}

void PhisApplication::animateMaterials(const GameTimer& gt) {

}

void PhisApplication::animateGeometry(const GameTimer& gt) {

}

void PhisApplication::loadTextures() {

}

void PhisApplication::loadModels() {

	this->cube = new RenderItem("cube.ms3d", true);
	cube->externalModel = true;

	this->cube2 = new RenderItem("cube.ms3d", true);
	cube2->externalModel = true;

	this->cube3 = new RenderItem("cube.ms3d", true);
	cube3->externalModel = true;

	this->cube4 = new RenderItem("cube.ms3d", true);
	cube4->externalModel = true;

	this->cube5 = new RenderItem("cube.ms3d", true);
	cube5->externalModel = true;

	this->dungeon = new RenderItem("dungeon.ms3d", true);
	this->dungeon->addPos(glm::vec3(20.0f, 0.0f, -132.0f * 0.0f));
	this->dungeon->addPos(glm::vec3(20.0f, 0.0f, -132.0f * 1.0f));
	this->dungeon->addPos(glm::vec3(20.0f, 0.0f, -132.0f * 2.0f));
	this->dungeon->addPos(glm::vec3(20.0f, 0.0f, -132.0f * 3.0f));
	this->dungeon->addPos(glm::vec3(20.0f, 0.0f, -132.0f * 4.0f));


	this->pipe = new RenderItem("pipe.ms3d", true);
	this->pipe->setScale(15.0f);

	srand(0);

	this->pipe->addPos(glm::vec3(-50.0f, 40.0f, -40.0f * 0.0f));
	this->pipe->addRot(0.0f + (((float)(rand() % 100)) / 100.0f * glm::pi<float>()), glm::vec3(0.0f, 0.0f, 1.0f));

	this->pipe->addPos(glm::vec3(-50.0f, 40.0f, -40.0f * 1.0f));
	this->pipe->addRot(0.0f + (((float)(rand() % 100)) / 100.0f * glm::pi<float>()), glm::vec3(0.0f, 0.0f, 1.0f));

	this->pipe->addPos(glm::vec3(-50.0f, 40.0f, -40.0f * 2.0f));
	this->pipe->addRot(0.0f + (((float)(rand() % 100)) / 100.0f * glm::pi<float>()), glm::vec3(0.0f, 0.0f, 1.0f));

	this->pipe->addPos(glm::vec3(-50.0f, 40.0f, -40.0f * 3.0f));
	this->pipe->addRot(0.0f + (((float)(rand() % 100)) / 100.0f * glm::pi<float>()), glm::vec3(0.0f, 0.0f, 1.0f));

	this->pipe->addPos(glm::vec3(-50.0f, 40.0f, -40.0f * 4.0f));
	this->pipe->addRot(0.0f + (((float)(rand() % 100)) / 100.0f * glm::pi<float>()), glm::vec3(0.0f, 0.0f, 1.0f));

	this->pipe->addPos(glm::vec3(-50.0f, 40.0f, -40.0f * 5.0f));
	this->pipe->addRot(0.0f + (((float)(rand() % 100)) / 100.0f * glm::pi<float>()), glm::vec3(0.0f, 0.0f, 1.0f));

	this->pipe->addPos(glm::vec3(-50.0f, 40.0f, -40.0f * 6.0f));
	this->pipe->addRot(0.0f + (((float)(rand() % 100)) / 100.0f * glm::pi<float>()), glm::vec3(0.0f, 0.0f, 1.0f));

	this->pipe->addPos(glm::vec3(-50.0f, 40.0f, -40.0f * 7.0f));
	this->pipe->addRot(0.0f + (((float)(rand() % 100)) / 100.0f * glm::pi<float>()), glm::vec3(0.0f, 0.0f, 1.0f));


	this->lamp = new RenderItem("lamp.ms3d", true);
	this->lamp->externalModel = true;
	//this->lamp->setScale(15.0f);
	//this->lamp->addPos(glm::vec3(0.0f, 40.0f, 0.0f * 0.0f));
	//this->lamp->addRot(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	this->lamp2 = new RenderItem("lamp.ms3d", true);
	this->lamp2->externalModel = true;

	this->lamp3 = new RenderItem("lamp.ms3d", true);
	this->lamp3->externalModel = true;

	this->cubeRod = new RenderItem("rod.ms3d", true);
	this->cubeRod->setScale(5.0f);
	this->cubeRod->extModelArray.push_back(glm::mat4());
	this->cubeRod->extModelArray.push_back(glm::mat4());
	this->cubeRod->extModelArray.push_back(glm::mat4());
	this->cubeRod->externalModelArray = true;
	this->cubeRod->externalModel = true;

	this->cubeRod->addPos(glm::vec3(0.0f, 40.0f, 0.0f * 0.0f));
	this->cubeRod->addPos(glm::vec3(0.0f, 40.0f, 0.0f * 0.0f));
	this->cubeRod->addPos(glm::vec3(0.0f, 40.0f, 0.0f * 0.0f));

	this->cubeRodC = new RenderItem("cube.ms3d", true);
	this->cubeRodC->setScale(0.2f);
	this->cubeRodC->addPos(glm::vec3(0.0f, 40.0f, 0.0f * 0.0f));
	this->cubeRodC->addPos(glm::vec3(0.0f, 40.0f, 0.0f * 0.0f));
	this->cubeRodC->addPos(glm::vec3(0.0f, 40.0f, 0.0f * 0.0f));
}

void PhisApplication::loadShaders() {
	_sh1 = new Shader("shade_vs.glsl", "shade_fs.glsl");
	_sh1->use();
	_sh1->setInt("material.diffuse", 0);

	_sh2 = new Shader("normalMap_vs.glsl", "normalMap_fs.glsl");
	_sh2->use();
	_sh2->setInt("material.diffuse", 0);
	_sh2->setInt("material.specular", 1);

	//sh2.setVec3("gFogColor", glm::vec3(0.058f, 0.058f, 0.066f));
	_sh2->setVec3("gFogColor", glm::vec3(0.027451f, 0.039216f, 0.058824f));
	_sh2->setFloat("gFogStart", 5.0f);
	_sh2->setFloat("gFogRange", 132.0f * 3.0f);
}

void PhisApplication::buildGeometry() {

	GeometryGenerator vertexGenerator;

	_cube3 = vertexGenerator.CreateBox(1.0f, 1.0f, 1.0f, 1);
	_vertCount3 = _cube3.Indices32.size();
	_vertices3 = new float[_vertCount3 * 8];
	for (int i = 0; i < _vertCount3; ++i) {
		_vertices3[i * 8 + 0] = _cube3.Vertices[_cube3.Indices32[i]].Position.x;
		_vertices3[i * 8 + 1] = _cube3.Vertices[_cube3.Indices32[i]].Position.y;
		_vertices3[i * 8 + 2] = _cube3.Vertices[_cube3.Indices32[i]].Position.z;

		_vertices3[i * 8 + 3] = _cube3.Vertices[_cube3.Indices32[i]].Normal.x;
		_vertices3[i * 8 + 4] = _cube3.Vertices[_cube3.Indices32[i]].Normal.y;
		_vertices3[i * 8 + 5] = _cube3.Vertices[_cube3.Indices32[i]].Normal.z;

		_vertices3[i * 8 + 6] = _cube3.Vertices[_cube3.Indices32[i]].TexC.x;
		_vertices3[i * 8 + 7] = _cube3.Vertices[_cube3.Indices32[i]].TexC.y;
	}
}

void PhisApplication::buildBuffers() {

	glGenVertexArrays(1, &_VAO3);
	glGenBuffers(1, &_VBO3);

	glBindVertexArray(_VAO3);

	glBindBuffer(GL_ARRAY_BUFFER, _VBO3);
	glBufferData(GL_ARRAY_BUFFER, _vertCount3 * sizeof(float) * 8, _vertices3, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void PhisApplication::buildCamera() {

	_camera = new Camera(glm::vec3(-20.0f, 5.0f, 25.0f), glm::vec3(0.0f, 1.0f, 0.0f), -75.0f, 35.0f);
}

void PhisApplication::generateContacts() {

	hit = false;

	CollisionPlane plane;
	plane.direction = glm::vec3(0.0f, 1.0f, 0.0f);
	plane.offset = 0.0f;

	cData.reset(maxContacts);
	cData.friction = 0.9f;
	cData.restitution = 0.2f;
	cData.tolerance = 0.1f;

	glm::mat4 transform, otherTransform;
	glm::vec3 position, otherPosition;
	for (Block *block = blocks; block < blocks + MAX_BLOCKS; block++) {

		if (!block->exists) {
			continue;
		}

		if (!cData.hasMoreContacts()) {
			return;
		}
		CollisionDetector::boxAndHalfSpace(*block, plane, &cData);

		if (ball_active) {

			if (!cData.hasMoreContacts()) return;
			if (CollisionDetector::boxAndSphere(*block, ball, &cData)) {
				hit = true;
				fracture_contact = cData.contactCount - 1;
			}
		}

		for (Block *other = block + 1; other < blocks + MAX_BLOCKS; other++) {

			if (!other->exists) {
				continue;
			}

			if (!cData.hasMoreContacts()) {
				return;
			}
			CollisionDetector::boxAndBox(*block, *other, &cData);
		}
	}

	/*
	if (ball_active) {
		if (!cData.hasMoreContacts()) {
			return;
		}
		CollisionDetector::sphereAndHalfSpace(ball, plane, &cData);
	}*/
}

void PhisApplication::reset() {

	blocks[0].exists = true;
	blocks[1].exists = true;
	blocks[2].exists = true;
	blocks[3].exists = true;
	blocks[4].exists = true;
	for (Block *block = blocks + 5; block < blocks + MAX_BLOCKS; block++) {
		block->exists = false;
	}

	/* block 1 */
	blocks[0].halfSize = glm::vec3(4.0f, 4.0f, 4.0f);
	blocks[0].body->setPosition(0.0f, 7.0f, 0.0f);
	blocks[0].body->setOrientation(1.0f, 0.0f, 0.0f, 0.0f);
	blocks[0].body->setVelocity(0.0f, -10.0f, 0.0f);
	blocks[0].body->setRotation(0.0f, 0.0f, 0.0f);
	blocks[0].body->setMass(100.0f);

	glm::mat3 it;
	glm::vec3 squares;
	squares.x = blocks[0].halfSize.x * blocks[0].halfSize.x;
	squares.y = blocks[0].halfSize.x * blocks[0].halfSize.x;
	squares.z = blocks[0].halfSize.x * blocks[0].halfSize.x;

	float ix = 0.3f * 100.0f * (squares.y + squares.z);
	float iy = 0.3f * 100.0f * (squares.x + squares.z);
	float iz = 0.3f * 100.0f * (squares.x + squares.y);
	float ixy = 0.0f;
	float ixz = 0.0f;
	float iyz = 0.0f;

	it[0][0] = ix;
	it[0][1] = it[1][0] = -ixy;
	it[0][2] = it[2][0] = -ixz;
	it[1][1] = iy;
	it[1][2] = it[2][1] = -iyz;
	it[2][2] = iz;

	blocks[0].body->setInertiaTensor(it);
	blocks[0].body->setDamping(0.99f, 0.99f);
	blocks[0].body->calculateDerivedData();
	blocks[0].calculateInternals();

	blocks[0].body->setAcceleration(glm::vec3(0.0f, -10.0f, 0.0f));
	blocks[0].body->setAwake(true);
	blocks[0].body->setCanSleep(true);

	/* block 2 */
	blocks[1].halfSize = glm::vec3(4.0f, 4.0f, 4.0f);
	blocks[1].body->setPosition(5.0f, 25.0f, 7.0f);
	blocks[1].body->setOrientation(1.0f, 0.0f, 0.0f, 0.0f);
	blocks[1].body->setVelocity(0.0f, -10.0f, 0.0f);
	blocks[1].body->setRotation(0.0f, 0.0f, 0.0f);
	blocks[1].body->setMass(100.0f);

	glm::mat3 it2;
	glm::vec3 squares2;
	squares2.x = blocks[1].halfSize.x * blocks[1].halfSize.x;
	squares2.y = blocks[1].halfSize.x * blocks[1].halfSize.x;
	squares2.z = blocks[1].halfSize.x * blocks[1].halfSize.x;

	float ix2 = 0.3f * 100.0f * (squares2.y + squares2.z);
	float iy2 = 0.3f * 100.0f * (squares2.x + squares2.z);
	float iz2 = 0.3f * 100.0f * (squares2.x + squares2.y);
	float ixy2 = 0.0f;
	float ixz2 = 0.0f;
	float iyz2 = 0.0f;

	it2[0][0] = ix2;
	it2[0][1] = it2[1][0] = -ixy2;
	it2[0][2] = it2[2][0] = -ixz2;
	it2[1][1] = iy2;
	it2[1][2] = it2[2][1] = -iyz2;
	it2[2][2] = iz2;

	blocks[1].body->setInertiaTensor(it2);
	blocks[1].body->setDamping(0.99f, 0.99f);
	blocks[1].body->calculateDerivedData();
	blocks[1].calculateInternals();

	blocks[1].body->setAcceleration(glm::vec3(0.0f, -10.0f, 0.0f));
	blocks[1].body->setAwake(true);
	blocks[1].body->setCanSleep(true);


	/* block 3 */
	blocks[2].halfSize = glm::vec3(4.0f, 4.0f, 4.0f);
	blocks[2].body->setPosition(-5.0f, 30.0f, -3.0f);
	blocks[2].body->setOrientation(1.0f, 0.0f, 0.0f, 0.0f);
	blocks[2].body->setVelocity(0.0f, -10.0f, 0.0f);
	blocks[2].body->setRotation(0.0f, 0.0f, 0.0f);
	blocks[2].body->setMass(100.0f);

	glm::mat3 it3;
	glm::vec3 squares3;
	squares3.x = blocks[2].halfSize.x * blocks[2].halfSize.x;
	squares3.y = blocks[2].halfSize.x * blocks[2].halfSize.x;
	squares3.z = blocks[2].halfSize.x * blocks[2].halfSize.x;

	float ix3 = 0.3f * 100.0f * (squares3.y + squares3.z);
	float iy3 = 0.3f * 100.0f * (squares3.x + squares3.z);
	float iz3 = 0.3f * 100.0f * (squares3.x + squares3.y);
	float ixy3 = 0.0f;
	float ixz3 = 0.0f;
	float iyz3 = 0.0f;

	it3[0][0] = ix3;
	it3[0][1] = it3[1][0] = -ixy3;
	it3[0][2] = it3[2][0] = -ixz3;
	it3[1][1] = iy3;
	it3[1][2] = it3[2][1] = -iyz3;
	it3[2][2] = iz3;

	blocks[2].body->setInertiaTensor(it3);
	blocks[2].body->setDamping(0.99f, 0.99f);
	blocks[2].body->calculateDerivedData();
	blocks[2].calculateInternals();

	blocks[2].body->setAcceleration(glm::vec3(0.0f, -10.0f, 0.0f));
	blocks[2].body->setAwake(true);
	blocks[2].body->setCanSleep(true);

	/* block 4 */
	blocks[3].halfSize = glm::vec3(4.0f, 4.0f, 4.0f);
	blocks[3].body->setPosition(-4.0f, 50.0f, -7.0f);
	blocks[3].body->setOrientation(1.0f, 0.0f, 0.0f, 0.0f);
	blocks[3].body->setVelocity(0.0f, -10.0f, 0.0f);
	blocks[3].body->setRotation(0.0f, 0.0f, 0.0f);
	blocks[3].body->setMass(100.0f);

	glm::mat3 it4;
	glm::vec3 squares4;
	squares4.x = blocks[3].halfSize.x * blocks[3].halfSize.x;
	squares4.y = blocks[3].halfSize.x * blocks[3].halfSize.x;
	squares4.z = blocks[3].halfSize.x * blocks[3].halfSize.x;

	float ix4 = 0.3f * 100.0f * (squares4.y + squares4.z);
	float iy4 = 0.3f * 100.0f * (squares4.x + squares4.z);
	float iz4 = 0.3f * 100.0f * (squares4.x + squares4.y);
	float ixy4 = 0.0f;
	float ixz4 = 0.0f;
	float iyz4 = 0.0f;

	it4[0][0] = ix4;
	it4[0][1] = it4[1][0] = -ixy4;
	it4[0][2] = it4[2][0] = -ixz4;
	it4[1][1] = iy4;
	it4[1][2] = it4[2][1] = -iyz4;
	it4[2][2] = iz4;

	blocks[3].body->setInertiaTensor(it4);
	blocks[3].body->setDamping(0.99f, 0.99f);
	blocks[3].body->calculateDerivedData();
	blocks[3].calculateInternals();

	blocks[3].body->setAcceleration(glm::vec3(0.0f, -10.0f, 0.0f));
	blocks[3].body->setAwake(true);
	blocks[3].body->setCanSleep(true);


	/* block 5 */
	blocks[4].halfSize = glm::vec3(4.0f, 4.0f, 4.0f);
	blocks[4].body->setPosition(-2.0f, 70.0f, -6.0f);
	blocks[4].body->setOrientation(1.0f, 0.0f, 0.0f, 0.0f);
	blocks[4].body->setVelocity(0.0f, -10.0f, 0.0f);
	blocks[4].body->setRotation(0.0f, 0.0f, 0.0f);
	blocks[4].body->setMass(100.0f);

	glm::mat3 it5;
	glm::vec3 squares5;
	squares5.x = blocks[4].halfSize.x * blocks[4].halfSize.x;
	squares5.y = blocks[4].halfSize.x * blocks[4].halfSize.x;
	squares5.z = blocks[4].halfSize.x * blocks[4].halfSize.x;

	float ix5 = 0.3f * 100.0f * (squares5.y + squares5.z);
	float iy5 = 0.3f * 100.0f * (squares5.x + squares5.z);
	float iz5 = 0.3f * 100.0f * (squares5.x + squares5.y);
	float ixy5 = 0.0f;
	float ixz5 = 0.0f;
	float iyz5 = 0.0f;

	it5[0][0] = ix5;
	it5[0][1] = it5[1][0] = -ixy5;
	it5[0][2] = it5[2][0] = -ixz4;
	it5[1][1] = iy5;
	it5[1][2] = it5[2][1] = -iyz5;
	it5[2][2] = iz5;

	blocks[4].body->setInertiaTensor(it5);
	blocks[4].body->setDamping(0.99f, 0.99f);
	blocks[4].body->calculateDerivedData();
	blocks[4].calculateInternals();

	blocks[4].body->setAcceleration(glm::vec3(0.0f, -10.0f, 0.0f));
	blocks[4].body->setAwake(true);
	blocks[4].body->setCanSleep(true);


	particleArray[0].setPosition(0.0f, 100.0f, -40.0f);
	particleArray[0].setMass(1.0f);
	particleArray[0].setRadius(0.25f);
	particleArray[0].setVelocity(0.0f, 0.0f, 0.0f);
	particleArray[0].setAcceleration(0.0f, 0.0f, 0.0f);
	particleArray[0].setDamping(0.99f);
	particleArray[0].clearAccumulator();

	particleGravityArray[0] = ParticleGravity(glm::vec3(0.0f, -9.8f, 0.0f));

	particleArray[1].setPosition(3.0f, 95.0f, -40.0f);
	particleArray[1].setMass(1.0f);
	particleArray[1].setRadius(0.25f);
	particleArray[1].setVelocity(0.0f, 0.0f, 0.0f);
	particleArray[1].setAcceleration(0.0f, 0.0f, 0.0f);
	particleArray[1].setDamping(0.99f);
	particleArray[1].clearAccumulator();

	particleGravityArray[1] = ParticleGravity(glm::vec3(0.0f, -9.8f, 0.0f));

	particleArray[2].setPosition(5.0f, 90.0f, -40.0f);
	particleArray[2].setMass(1.0f);
	particleArray[2].setRadius(0.25f);
	particleArray[2].setVelocity(0.0f, 0.0f, 0.0f);
	particleArray[2].setAcceleration(0.0f, 0.0f, 0.0f);
	particleArray[2].setDamping(0.99f);
	particleArray[2].clearAccumulator();

	particleGravityArray[2] = ParticleGravity(glm::vec3(0.0f, -9.8f, 0.0f));

	particleArray[3].setPosition(6.0f, 85.0f, -45.0f);
	particleArray[3].setMass(1.0f);
	particleArray[3].setRadius(0.25f);
	particleArray[3].setVelocity(0.0f, 0.0f, 0.0f);
	particleArray[3].setAcceleration(0.0f, 0.0f, 0.0f);
	particleArray[3].setDamping(0.99f);
	particleArray[3].clearAccumulator();

	particleGravityArray[3] = ParticleGravity(glm::vec3(0.0f, -9.8f, 0.0f));

	supports = new ParticleCableConstraint[1];
	supports[0].particle = &particleArray[1];
	supports[0].anchor = glm::vec3(0.0f, 100.0f, -40.0f);
	supports[0].maxLength = 5.0f;
	supports[0].restitution = 0.3f;
	world.getContactGenerators().push_back(&supports[0]);

	cables = new ParticleCable[2];

	cables[0].particle[0] = &particleArray[1];
	cables[0].particle[1] = &particleArray[2];
	cables[0].maxLength = 5.0f;
	cables[0].restitution = 0.3f;
	world.getContactGenerators().push_back(&cables[0]);

	cables[1].particle[0] = &particleArray[2];
	cables[1].particle[1] = &particleArray[3];
	cables[1].maxLength = 5.0f;
	cables[1].restitution = 0.3f;
	world.getContactGenerators().push_back(&cables[1]);
	

	//world.getForceRegistry().add(&particleArray[0], &particleGravityArray[0]);
	world.getForceRegistry().add(&particleArray[1], &particleGravityArray[1]);
	world.getForceRegistry().add(&particleArray[2], &particleGravityArray[2]);
	world.getForceRegistry().add(&particleArray[3], &particleGravityArray[3]);




	//ball_active = true;
	ball_active = false;

	ball.body->setPosition(0.0f, 5.0f, 20.0f);
	ball.body->setOrientation(1, 0, 0, 0);
	ball.body->setVelocity(
		random.randomBinomial(4.0f),
		random.randomReal(1.0f, 6.0f),
		-20.0f
	);
	ball.body->setRotation(0.0f, 0.0f, 0.0f);
	ball.body->calculateDerivedData();
	ball.body->setAwake(true);
	ball.calculateInternals();

	hit = false;

	cData.contactCount = 0;
}

void PhisApplication::updateObjects(float duration) {

	for (Block *block = blocks; block < blocks + MAX_BLOCKS; block++) {
		if (block->exists) {
			block->body->integrate(duration);
			block->calculateInternals();
		}
	}

	static float angle0 = this->pipe->rotationAngs.at(0);
	static float angle1 = this->pipe->rotationAngs.at(1);
	static float angle2 = this->pipe->rotationAngs.at(2);
	static float angle3 = this->pipe->rotationAngs.at(3);
	static float angle4 = this->pipe->rotationAngs.at(4);
	static float angle5 = this->pipe->rotationAngs.at(5);
	static float angle6 = this->pipe->rotationAngs.at(6);
	static float angle7 = this->pipe->rotationAngs.at(7);

	static float time = 0.0f;
	static float bpm = 175.0f;
	static float bpm2 = 175.0f * 4.0f;

	static float beatDuration = 60.0f / bpm;
	static float patternDuration = beatDuration * 4.0f;
	static float hquadDuration = beatDuration * 32.0f;
	static float quadDuration = beatDuration * 32.0f;

	static int beatCounter = 0;
	static int patternCounter = 0;
	static int hquadCounter = 0;
	static int quadCounter = 0;

	time += duration;

	if((((float)beatCounter) * beatDuration) < time) {
		++beatCounter;
	}

	if((((float)patternCounter) * patternDuration) < time) {
		++patternCounter;
	}

	if((((float)hquadCounter) * hquadDuration) < time) {
		++hquadCounter;
	}

	if((((float)quadCounter) * quadDuration) < time) {
		++quadCounter;
	}

	float beat = glm::clamp(glm::abs(glm::sin(time * glm::pi<float>() * (1.0f / (60.0f / bpm)))), 0.0f, 1.0f);
	float beat2 = glm::abs(glm::sin(time * glm::pi<float>() * (1.0f / (60.0f / bpm))));
	float beat3 = glm::clamp(glm::abs(glm::sin(time * glm::pi<float>() * (1.0f / (60.0f / bpm)))), 0.0f, 1.0f);

	float div = 1.0f;
	if(patternCounter  >= 4 && patternCounter  <= 12) {
		angle0 += glm::pi<float>() / div * time * 0.01f;
	}
	else {
		angle0 += glm::pi<float>() / div * duration * beat * (playAnim ? 1.0f : 0.0f);
	}
	if (angle0 >= 2.0f * glm::pi<float>()) {
		angle0 -= 2.0f * glm::pi<float>();
	}
	this->pipe->rotationAngs.at(0) = angle0;

	if(patternCounter  >= 4 && patternCounter  <= 12) {
		angle1 += glm::pi<float>() / div * time * 0.01f;
	}
	else {
		angle1 += glm::pi<float>() / div * duration * beat * (playAnim ? 1.0f : 0.0f);
	}
	if (angle1 >= 2.0f * glm::pi<float>()) {
		angle1 -= 2.0f * glm::pi<float>();
	}
	this->pipe->rotationAngs.at(1) = angle1;

	if(patternCounter  >= 4 && patternCounter  <= 12) {
		angle2 += glm::pi<float>() / div * time * 0.01f;
	}
	else {
		angle2 += glm::pi<float>() / div * duration * beat * (playAnim ? 1.0f : 0.0f);
	}
	if (angle2 >= 2.0f * glm::pi<float>()) {
		angle2 -= 2.0f * glm::pi<float>();
	}
	this->pipe->rotationAngs.at(2) = angle2;

	if(patternCounter  >= 4 && patternCounter  <= 12) {
		angle3 += glm::pi<float>() / div * time * 0.01f;
	}
	else {
		angle3 += glm::pi<float>() / div * duration * beat * (playAnim ? 1.0f : 0.0f);
	}
	if (angle3 >= 2.0f * glm::pi<float>()) {
		angle3 -= 2.0f * glm::pi<float>();
	}
	this->pipe->rotationAngs.at(3) = angle3;

	if(patternCounter  >= 4 && patternCounter  <= 12) {
		angle4 += glm::pi<float>() / div * time * 0.01f;
	}
	else {
		angle4 += glm::pi<float>() / div * duration * beat * (playAnim ? 1.0f : 0.0f);
	}
	if (angle4 >= 2.0f * glm::pi<float>()) {
		angle4 -= 2.0f * glm::pi<float>();
	}
	this->pipe->rotationAngs.at(4) = angle4;

	if(patternCounter  >= 4 && patternCounter  <= 12) {
		angle5 += glm::pi<float>() / div * time * 0.01f;
	}
	else {
		angle5 += glm::pi<float>() / div * duration * beat * (playAnim ? 1.0f : 0.0f);
	}
	if (angle5 >= 2.0f * glm::pi<float>()) {
		angle5 -= 2.0f * glm::pi<float>();
	}
	this->pipe->rotationAngs.at(5) = angle5;

	if(patternCounter  >= 4 && patternCounter  <= 12) {
		angle6 += glm::pi<float>() / div * time * 0.01f;
	}
	else {
		angle6 += glm::pi<float>() / div * duration * beat * (playAnim ? 1.0f : 0.0f);
	}
	if (angle6 >= 2.0f * glm::pi<float>()) {
		angle6 -= 2.0f * glm::pi<float>();
	}
	this->pipe->rotationAngs.at(6) = angle6;

	if(patternCounter  >= 4 && patternCounter  <= 12) {
		angle7 += glm::pi<float>() / div * time * 0.01f;
	}
	else {
		angle7 += glm::pi<float>() / div * duration * beat * (playAnim ? 1.0f : 0.0f);
	}
	if (angle7 >= 2.0f * glm::pi<float>()) {
		angle7 -= 2.0f * glm::pi<float>();
	}
	this->pipe->rotationAngs.at(7) = angle7;

	static glm::vec3 dir = glm::vec3(0.0f, 0.0f, -1.0f);
	static bool firstFrame = true;
	static float cameraSpeedPith = 16.0f;
	static float cameraSpeedYaw = 16.0f;

	static float cameraSpeedPithDir = 1.0f;
	static float cameraSpeedYawDir = 1.0f;

	static glm::vec3 camPos =  glm::vec3(-75.0f, 15.0f, 0.0f);

	static float cameraFloat = 0.0f;

	bool change = false;

	static float cameraVelocity = 32.0f;
	if (playAnim) {
		if (firstFrame) {
			_camera->Position = glm::vec3(-75.0f, 15.0f, 0.0f);
			_camera->Pitch = 45.0f;
			_camera->Yaw = -45.0f;
			_camera->updateCameraVectors();

			blocks[0].exists = false;
			blocks[2].exists = false;
			blocks[2].exists = false;
			blocks[3].exists = false;
			blocks[4].exists = false;

			firstFrame = false;
		}
		else {

			_camera->Pitch -= duration * cameraSpeedPith * cameraSpeedPithDir;
			_camera->Yaw -= duration * cameraSpeedYaw * cameraSpeedYawDir;
			_camera->updateCameraVectors();
			//_camera->Position += glm::vec3(0.0f, cameraFloat * beat2, 0.0f);

			
			if (_camera->Pitch < 0.0f || _camera->Pitch > 45.0f) {
				cameraSpeedPithDir *= -1.0f;
				camPos = glm::vec3(-5.0f, 35.0f, 0.0f);
				_camera->Position = camPos;
				_camera->Pitch = 10.0f;;
				_camera->Yaw = -105.0f;;
				_camera->updateCameraVectors();
				cameraFloat = 1.0f;
			}
		}

		_camera->ProcessKeyboard(dir, duration * cameraVelocity);
	}

	glm::quat q1 = glm::quat(glm::normalize(particleArray[3].getPosition() - particleArray[2].getPosition()));
	glm::mat4 m1 = glm::mat4();
	_calculateTransformMatrix(m1, particleArray[3].getPosition(), q1);
	m1 = glm::transpose(m1);
	lamp->extModel = m1;

	q1 = glm::quat(glm::normalize(particleArray[3].getPosition() - particleArray[2].getPosition()));
	m1 = glm::mat4();
	_calculateTransformMatrix(m1, glm::vec3(particleArray[3].getPosition().x, particleArray[3].getPosition().y, particleArray[3].getPosition().z -132.0f), q1);
	m1 = glm::transpose(m1);
	lamp2->extModel = m1;

	q1 = glm::quat(glm::normalize(particleArray[3].getPosition() - particleArray[2].getPosition()));
	m1 = glm::mat4();
	_calculateTransformMatrix(m1, glm::vec3(particleArray[3].getPosition().x, particleArray[3].getPosition().y, particleArray[3].getPosition().z -132.0f * 2.0f), q1);
	m1 = glm::transpose(m1);
	lamp3->extModel = m1;

	float len1 = glm::length(particleArray[1].getPosition() - particleArray[0].getPosition());
	q1 = glm::quat(glm::normalize(particleArray[1].getPosition() - particleArray[0].getPosition()));
	q1 *= q1;
	m1 = glm::mat4();
	_calculateTransformMatrix(m1, particleArray[0].getPosition(), q1);
	m1 = glm::transpose(m1);
	m1 = glm::scale(m1, glm::vec3(len1, len1, len1));
	cubeRod->extModelArray.at(0) = m1;

	float len2 = glm::length(particleArray[2].getPosition() - particleArray[1].getPosition());
	q1 = glm::quat(glm::normalize(particleArray[2].getPosition() - particleArray[1].getPosition()));
	q1 *= q1;
	m1 = glm::mat4();
	_calculateTransformMatrix(m1, particleArray[1].getPosition(), q1);
	m1 = glm::transpose(m1);
	m1 = glm::scale(m1, glm::vec3(len2, len2, len2));
	cubeRod->extModelArray.at(1) = m1;

	float len3 = glm::length(particleArray[3].getPosition() - particleArray[2].getPosition());
	q1 = glm::quat(glm::normalize(particleArray[3].getPosition() - particleArray[2].getPosition()));
	q1 *= q1;
	m1 = glm::mat4();
	_calculateTransformMatrix(m1, particleArray[2].getPosition(), q1);
	m1 = glm::transpose(m1);
	m1 = glm::scale(m1, glm::vec3(len3, len3, len3));
	cubeRod->extModelArray.at(2) = m1;

	cubeRodC->positions.at(0) = particleArray[1].getPosition();
	cubeRodC->positions.at(1) = particleArray[2].getPosition();
	cubeRodC->positions.at(2) = particleArray[3].getPosition();

	if (ball_active) {
		ball.body->integrate(duration);
		ball.calculateInternals();
	}
}
#ifndef RENDERITEM_H
#define RENDERITEM_H

#include "glad.h"
#include "glfw3.h"
#include "Shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <vector>
#include <map>
#include "MS3DFile.h"

using namespace std;

struct RenderGroup {
	int triangle;
	int materialIndex;
};

struct InervalMaterial {
	int triangleStart;
	int triangleEnd;
	unsigned int diffuseMapTexture;
	unsigned int specularMapTexture;
};

class RenderItem {

public:

	CMS3DFile charModel;

	unsigned int VAO;
	unsigned int VBO;

	vector<float> vb;
	int vertexCount;
	int indexCount;

	unsigned int diffuseMapTexture;
	unsigned int specularMapTexture;

	vector<RenderGroup> groups;
	vector<InervalMaterial> itervals;
	vector<glm::vec3> positions;
	vector<glm::vec3> rotationVecs;
	vector<float> rotationAngs;
	float scaleFactor;

	unsigned int loadTexture(char const * path);

	bool normalMap;
	bool externalModel;
	bool externalModelArray;

	glm::mat4 extModel;
	vector<glm::mat4> extModelArray;

public:
	RenderItem(const char* model, bool nm = false);
	~RenderItem();

	int vb_size();
	unsigned int getVAO();
	unsigned int getVBO();
	unsigned int getDiffText();
	unsigned int getSpecTex();

	void setNormalMap(bool);
	void setScale(float);
	void setPos(vector<glm::vec3>);
	void addRot(float, glm::vec3);
	void addPos(glm::vec3);

	void draw(Shader *sh);
};

unsigned int loadCubemap(vector<std::string> faces);

#endif

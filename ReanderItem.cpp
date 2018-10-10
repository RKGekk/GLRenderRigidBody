#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include "RenderItem.h"
#include "IntervalSearch.h"

using namespace std;

unsigned int  RenderItem::loadTexture(char const * path) {

	static map<string, int> paths;

	for (auto it = paths.begin(); it != paths.end(); ++it) {
		if (it->first == path) {
			return it->second;
		}
	}

	unsigned int textureID;
	glGenTextures(1, &textureID);

	paths.insert(pair<string, int>(path, textureID));

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

RenderItem::RenderItem(const char* model, bool nm) {

	IntervalSearch is;
	normalMap = nm;
	scaleFactor = 1.0f;
	externalModel = false;
	externalModelArray = false;

	charModel.LoadFromFile(model);

	vertexCount = charModel.GetNumVertices();
	indexCount = charModel.GetNumTriangles();
	for (int i = 0; i < indexCount; ++i) {

		ms3d_triangle_t *tempTriangle;
		charModel.GetTriangleAt(i, &tempTriangle);

		ms3d_group_t *tempGrp;
		int groupIndex = tempTriangle->groupIndex;
		charModel.GetGroupAt(groupIndex, &tempGrp);
		groups.push_back(RenderGroup{ i, tempGrp->materialIndex });
		is.push(IntervalVal{ i, tempGrp->materialIndex });


		ms3d_vertex_t *tempVertex1;
		charModel.GetVertexAt(tempTriangle->vertexIndices[0], &tempVertex1);

		ms3d_vertex_t *tempVertex2;
		charModel.GetVertexAt(tempTriangle->vertexIndices[1], &tempVertex2);

		ms3d_vertex_t *tempVertex3;
		charModel.GetVertexAt(tempTriangle->vertexIndices[2], &tempVertex3);

		glm::vec3 tangent1, bitangent1;
		if (normalMap) {

			glm::vec3 pos1(tempVertex1->vertex[0], tempVertex1->vertex[1], tempVertex1->vertex[2]);
			glm::vec3 pos2(tempVertex2->vertex[0], tempVertex2->vertex[1], tempVertex2->vertex[2]);
			glm::vec3 pos3(tempVertex3->vertex[0], tempVertex3->vertex[1], tempVertex3->vertex[2]);

			glm::vec2 uv1(tempTriangle->s[0], tempTriangle->t[0]);
			glm::vec2 uv2(tempTriangle->s[1], tempTriangle->t[1]);
			glm::vec2 uv3(tempTriangle->s[2], tempTriangle->t[2]);

			glm::vec3 edge1 = pos2 - pos1;
			glm::vec3 edge2 = pos3 - pos1;
			glm::vec2 deltaUV1 = uv2 - uv1;
			glm::vec2 deltaUV2 = uv3 - uv1;

			GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent1 = glm::normalize(tangent1);

			bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
			bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
			bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
			bitangent1 = glm::normalize(bitangent1);
		}

		vb.push_back(tempVertex1->vertex[0]);
		vb.push_back(tempVertex1->vertex[1]);
		vb.push_back(tempVertex1->vertex[2]);
		vb.push_back(tempTriangle->vertexNormals[0][0]);
		vb.push_back(tempTriangle->vertexNormals[0][1]);
		vb.push_back(tempTriangle->vertexNormals[0][2]);
		vb.push_back(tempTriangle->s[0]);
		vb.push_back(tempTriangle->t[0]);
		if (normalMap) {
			vb.push_back(tangent1.x);
			vb.push_back(tangent1.y);
			vb.push_back(tangent1.z);

			vb.push_back(bitangent1.x);
			vb.push_back(bitangent1.y);
			vb.push_back(bitangent1.z);
		}

		vb.push_back(tempVertex2->vertex[0]);
		vb.push_back(tempVertex2->vertex[1]);
		vb.push_back(tempVertex2->vertex[2]);
		vb.push_back(tempTriangle->vertexNormals[1][0]);
		vb.push_back(tempTriangle->vertexNormals[1][1]);
		vb.push_back(tempTriangle->vertexNormals[1][2]);
		vb.push_back(tempTriangle->s[1]);
		vb.push_back(tempTriangle->t[1]);
		if (normalMap) {
			vb.push_back(tangent1.x);
			vb.push_back(tangent1.y);
			vb.push_back(tangent1.z);

			vb.push_back(bitangent1.x);
			vb.push_back(bitangent1.y);
			vb.push_back(bitangent1.z);
		}

		vb.push_back(tempVertex3->vertex[0]);
		vb.push_back(tempVertex3->vertex[1]);
		vb.push_back(tempVertex3->vertex[2]);
		vb.push_back(tempTriangle->vertexNormals[2][0]);
		vb.push_back(tempTriangle->vertexNormals[2][1]);
		vb.push_back(tempTriangle->vertexNormals[2][2]);
		vb.push_back(tempTriangle->s[2]);
		vb.push_back(tempTriangle->t[2]);
		if (normalMap) {
			vb.push_back(tangent1.x);
			vb.push_back(tangent1.y);
			vb.push_back(tangent1.z);

			vb.push_back(bitangent1.x);
			vb.push_back(bitangent1.y);
			vb.push_back(bitangent1.z);
		}
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	int vb_size;
	if (normalMap) {
		vb_size = indexCount * 14 * sizeof(float) * 3;
		glBufferData(GL_ARRAY_BUFFER, vb_size, vb.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
		glEnableVertexAttribArray(4);
	}
	else {
		vb_size = indexCount * 8 * sizeof(float) * 3;
		glBufferData(GL_ARRAY_BUFFER, vb_size, vb.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	vector<IntervalGroup> igs = is.getIntervals();

	for (int j = 0; j < igs.size(); ++j) {
		ms3d_material_t *material;
		charModel.GetMaterialAt(igs[j].id, &material);

		itervals.push_back(
			InervalMaterial{
				igs[j].from,
				igs[j].to,
				loadTexture(material->texture),
				loadTexture(material->alphamap)
		}
		);
	}
}

RenderItem::~RenderItem() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

int RenderItem::vb_size() {
	return indexCount * 8 * sizeof(float) * 3;
}

unsigned int RenderItem::getVAO() {
	return VAO;
}

unsigned int RenderItem::getVBO() {
	return VBO;
}

unsigned int RenderItem::getDiffText() {
	return diffuseMapTexture;
}

unsigned int RenderItem::getSpecTex() {
	return specularMapTexture;
}

void RenderItem::setScale(float s) {
	scaleFactor = s;
}

void RenderItem::addRot(float angle, glm::vec3 v) {
	rotationAngs.push_back(angle);
	rotationVecs.push_back(v);
}

void RenderItem::setPos(vector<glm::vec3> pos) {
	positions = pos;
}

void RenderItem::addPos(glm::vec3 pos) {
	positions.push_back(pos);
}

void RenderItem::setNormalMap(bool b) {
	normalMap = b;
}

void RenderItem::draw(Shader *sh) {

	glBindVertexArray(VAO);
	int modelsCount = positions.size();

	for (int k = 0; k < itervals.size(); ++k) {

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, itervals[k].diffuseMapTexture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, itervals[k].specularMapTexture);

		if(externalModel) {
			if(externalModelArray) {
				for (unsigned int i = 0; i < modelsCount; i++) {
					sh->setMat4("model", extModelArray.at(i));
					glDrawArrays(GL_TRIANGLES, itervals[k].triangleStart * 3, (itervals[k].triangleEnd + 1) * 3);
				}
			}
			else {
				sh->setMat4("model", extModel);
				glDrawArrays(GL_TRIANGLES, itervals[k].triangleStart * 3, (itervals[k].triangleEnd + 1) * 3);
			}
		}
		else
		if(rotationAngs.size() > 0) {
			for (unsigned int i = 0; i < modelsCount; i++) {

				glm::mat4 model;
				
				model = glm::translate(model, positions[i]);
				model = glm::rotate(model, rotationAngs[i], rotationVecs[i]);
				model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
				sh->setMat4("model", model);

				glDrawArrays(GL_TRIANGLES, itervals[k].triangleStart * 3, (itervals[k].triangleEnd - itervals[k].triangleStart + 1) * 3);
			}
		}
		else {
			for (unsigned int i = 0; i < modelsCount; i++) {

				glm::mat4 model;
				model = glm::translate(model, positions[i]);
				model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
				sh->setMat4("model", model);

				glDrawArrays(GL_TRIANGLES, itervals[k].triangleStart * 3, (itervals[k].triangleEnd - itervals[k].triangleStart + 1) * 3);
			}
		}
	}
}

unsigned int loadCubemap(vector<std::string> faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
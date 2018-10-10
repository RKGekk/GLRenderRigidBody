#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 ourColor;
out vec2 TexCoord;

void main() {
	ourColor = aNormal;
    TexCoord = aTexCoords;
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	//gl_Position = vec4(aPos.x, aPos.z, aPos.y, 1.0);
}
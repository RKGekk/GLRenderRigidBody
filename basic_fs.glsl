#version 450 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

//uniform vec4 ourColor;
uniform sampler2D ourTexture;

void main() {
	//FragColor = ourColor;
	FragColor = texture(ourTexture, TexCoord);
}
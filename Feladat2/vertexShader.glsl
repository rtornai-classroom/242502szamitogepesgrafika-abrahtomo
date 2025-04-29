#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 color;

out vec3 myColor;

void main(void) {
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	myColor = color;
}

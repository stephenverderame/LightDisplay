#version 460 core
in vec4 fragColor;

out vec4 _fragColor;

void main() {
	_fragColor = fragColor;
}
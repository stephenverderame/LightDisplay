#version 460 core
layout (location = 0) in vec2 pos;

out vec2 texCoords;

/// Converts coordinates from -1 to 1 to coordinates from 0 to 1
vec2 ndcToTex(vec2 v);
void main() {
	gl_Position = vec4(pos, 1.0, 1.0);
	texCoords = ndcToTex(pos);
}
vec2 ndcToTex(vec2 v) {
	return (v + vec2(1)) / 2.0;
}
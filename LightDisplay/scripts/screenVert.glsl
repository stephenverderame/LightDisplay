#version 460 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec4 color;


struct VsOut {
	vec4 fragColor;
};
out VsOut vsOut;

uniform mat4 projection;

void main()
{
	gl_Position = vec4(pos, 1.0, 1.0);
	vsOut.fragColor = color;
}
#version 460 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec4 color;

out vec4 fragColor;

uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(pos, 0.0, 1.0);
	fragColor = color;
}
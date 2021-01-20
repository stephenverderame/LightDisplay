#version 460 core
struct GsOut {
	vec4 fragColor;
};
in GsOut gsOut;

out vec4 frag_color;

void main() {
	frag_color = gsOut.fragColor;
}
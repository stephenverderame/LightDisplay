#version 460 core
layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

struct VsOut {
	vec4 fragColor;
};
in VsOut vsOut[];

struct GsOut {
	vec4 fragColor;
};
out GsOut gsOut;

void main() {
	const vec4 pos = gl_in[0].gl_Position;
	const vec4 color = vsOut[0].fragColor;
	const float width = 0.005;
	const float extraHeight = 0.1;
	const float glowFac = 0.2;

	// turn a point into a rectangle going to bottom of screen
	// vertices listed counter-clockwise

	// extra bit above the main bar for a glow effect
	gl_Position = pos + vec4(width, extraHeight, 0.0, 0.0);
	gsOut.fragColor = color * glowFac;
	EmitVertex();
	gl_Position = pos + vec4(0.0, extraHeight, 0.0, 0.0);
	EmitVertex();

	// main "bar"
	gl_Position = pos + vec4(width, 0.0, 0.0, 0.0);
	gsOut.fragColor = color;
	EmitVertex();
	gl_Position = pos;
	EmitVertex();
	gl_Position = vec4(pos.x + width, -1.0, pos.z, pos.w);
	EmitVertex();
	gl_Position = vec4(pos.x, -1.0, pos.z, pos.w);
	EmitVertex();

	EndPrimitive();
}
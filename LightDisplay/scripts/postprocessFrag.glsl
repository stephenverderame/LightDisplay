#version 460 core
in vec2 texCoords;

uniform sampler2D tex;

out vec4 fragColor;

/**
* @return the guassian blurred value of the texture at the given texCoords
*/
vec4 blur();
void main() {
	fragColor = texture(tex, texCoords) + blur();
}
// 5x5 guassian kernel
const float[5][5] kernel = {
	{1, 4, 7, 4, 1},
	{4, 16, 24, 16, 4},
	{7, 26, 41, 26, 7},
	{4, 16, 24, 16, 4},
	{1, 4, 7, 4, 1}
};
vec4 blur() {
	const vec2 texOffset = 1.0 / textureSize(tex, 0);
	vec4 result = vec4(0);
	for(int i = -2; i <= 2; ++i){
		for(int j = -2; j <= 2; ++j){
			result += kernel[i + 2][j + 2] / 273.0 * 
				texture(tex, vec2(texCoords.x + texOffset.x * i, texCoords.y + texOffset.y * j));
		}
	}
	return result;
}
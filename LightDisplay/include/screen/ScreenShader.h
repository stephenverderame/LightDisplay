#pragma once
#include <iosfwd>
#define GLM_FORCE_SILENT_WARNINGS //running on W4/Werror so ignore glm warnings
#include <glm/glm.hpp>
class ScreenShader {
private:
	static unsigned int programInUse;
protected:
	unsigned int programId; ///< invalid_program iff this shader was moved
private:
public:
	ScreenShader(std::istream& vertex, std::istream& fragment);
	ScreenShader(std::istream& vertex, std::istream& geometry, std::istream& fragment);
	~ScreenShader();
	ScreenShader(const ScreenShader&) = delete;
	ScreenShader& operator=(const ScreenShader&) = delete;

	void swap(ScreenShader& other) noexcept;
	ScreenShader& operator=(ScreenShader&& other) noexcept;
	ScreenShader(ScreenShader&& other) noexcept;

	/// Uses the shader. Called internally by all member functions
	void use();

	void setVec3(const char* varName, const glm::vec3& v);
	void setVec4(const char* varName, const glm::vec4& v);
	void setMat4(const char* varName, const glm::mat4& m);
};

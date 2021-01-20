#include <screen/ScreenShader.h>
#include <istream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

enum class ShaderType : GLenum {
	vertex = GL_VERTEX_SHADER,
	frag = GL_FRAGMENT_SHADER,
	geom = GL_GEOMETRY_SHADER
};
struct RawShader {
	ShaderType type;
	GLuint shader;
	RawShader(ShaderType t, GLuint s) :
		type(t), shader(s) {}
	~RawShader() {
		glDeleteShader(shader);
	}
};

constexpr GLuint invalid_program = ~0u;
GLuint ScreenShader::programInUse = invalid_program;

inline auto compileShader(const char* sourceCode, ShaderType type) {
	auto shader = glCreateShader(static_cast<GLenum>(type));
	glShaderSource(shader, 1, &sourceCode, nullptr);
	glCompileShader(shader);
	int success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[1024];
		glGetShaderInfoLog(shader, 1024, nullptr, log);
		glDeleteShader(shader);
		fprintf(stderr, log);
		throw std::runtime_error(log);
	}
	return RawShader{ type, shader };
}
inline auto linkShaders(const RawShader& vertex, const RawShader& fragment, const RawShader* geometry = nullptr) {
	auto program = glCreateProgram();
	glAttachShader(program, vertex.shader);
	glAttachShader(program, fragment.shader);
	if (geometry != nullptr) glAttachShader(program, geometry->shader);
	glLinkProgram(program);
	int success = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char log[1024];
		glGetProgramInfoLog(program, 1024, nullptr, log);
		fprintf(stderr, log);
		glDeleteProgram(program);
		throw std::runtime_error(log);
	}
	return program;

}
inline auto streamToShader(std::istream& s, ShaderType type) {
	std::noskipws(s);
	std::string str{ std::istream_iterator<char>(s), {} };
	return compileShader(str.c_str(), type);
}

ScreenShader::ScreenShader(std::istream& vertex, std::istream& fragment)
{
	auto vertexShader = streamToShader(vertex, ShaderType::vertex);
	auto fragmentShader = streamToShader(fragment, ShaderType::frag);
	programId = linkShaders(vertexShader, fragmentShader);


}

ScreenShader::ScreenShader(std::istream& vertex, std::istream& geometry, std::istream& fragment)
{
	auto vertexShader = streamToShader(vertex, ShaderType::vertex);
	auto fragmentShader = streamToShader(fragment, ShaderType::frag);
	auto geomShader = streamToShader(geometry, ShaderType::geom);
	programId = linkShaders(vertexShader, fragmentShader, &geomShader);
}

ScreenShader::~ScreenShader()
{
	if(programId != invalid_program) glDeleteProgram(programId);
}

void ScreenShader::swap(ScreenShader & other) noexcept
{
	std::swap(programId, other.programId);
}

ScreenShader& ScreenShader::operator=(ScreenShader&& other) noexcept
{
	swap(other);
	return *this;
}

ScreenShader::ScreenShader(ScreenShader&& other) noexcept : programId(other.programId)
{
	other.programId = invalid_program;
}

void ScreenShader::use()
{
	if (programInUse != programId) {
		programInUse = programId;
		glUseProgram(programId);
	}
}

void ScreenShader::setVec3(const char* varName, const glm::vec3& v)
{
	use();
	glUniform3f(glGetUniformLocation(programId, varName), v[0], v[1], v[2]);
}

void ScreenShader::setVec4(const char* varName, const glm::vec4& v)
{
	use();
	glUniform4f(glGetUniformLocation(programId, varName), v[0], v[1], v[2], v[3]);
}

void ScreenShader::setMat4(const char* varName, const glm::mat4& m)
{
	use();
	glUniformMatrix4fv(glGetUniformLocation(programId, varName), 1, GL_FALSE, glm::value_ptr(m));
}

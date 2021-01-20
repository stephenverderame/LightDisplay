#include <screen/2DScreenDisplay.h>
#include <screen/Framebuffer.h>
#include <glad/glad.h>
#include <screen/ScreenShader.h>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <filesystem>
class PointSet {
private:
	GLuint vao, vbo;
	struct VertexData {
		float x, y;
		float r, g, b, a;
		VertexData(const Visual& v) : x(v.ndcX), y(v.ndcY), r(v.color.r / 255.f),
			g(v.color.g / 255.f), b(v.color.b / 255.f), a(v.color.a / 255.f) {}
		VertexData() : x(0), y(0), r(0), g(0), b(0), a(0) {}
	};
	std::vector<VertexData> buffer;
	GLuint verticesPerBatch;
public:
	explicit PointSet(GLuint verticesPerBuffer = 1024);
	~PointSet();
	PointSet(const PointSet&) = delete;
	PointSet& operator=(const PointSet&) = delete;
	void draw(const std::vector<Visual>& vis);
private:
	/// Adds a batch to the buffer
	void addToBuffer(const std::vector<Visual>& v);
	/**
	 * Requires buffer to not be empty
	 * Draws at most verticesPerBatch vertices
	 */
	auto drawReadyBatch(decltype(buffer)::const_iterator start) -> decltype(start);
};
class Rect {
private:
	GLuint vao, vbo;
	const static float vertices[8]; 
public:
	Rect();
	~Rect();
	void draw();
};
const float Rect::vertices[8] = {
		-1.0f, 1.0f,
		-1.0f, -1.0f,
		1.0f, 1.0f,
		1.0f, -1.0f,
};
struct ScreenDisplay2D::impl {
	std::unique_ptr<ScreenShader> shader = nullptr, postShader = nullptr;
	std::unique_ptr<Framebuffer> fbo = nullptr;
	glm::mat4 proj = glm::identity<glm::mat4>();
	PointSet points;
	Rect rect;
};

inline void clearScreen() {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.f, 0.f, 0.f, 1.f);
}

void ScreenDisplay2D::render(const std::vector<Visual>& visuals)
{
	clearScreen();
	updateShaderValues();
	pimpl->fbo->enableWrite();
	clearScreen();
	pimpl->points.draw(visuals);
	pimpl->fbo->enableRead();
	pimpl->postShader->use();
	pimpl->rect.draw();
}

void ScreenDisplay2D::resize(int width, int height)
{
	glViewport(0, 0, width, height);
	pimpl->fbo = std::make_unique<Framebuffer>(width, height);
}

ScreenDisplay2D::~ScreenDisplay2D() = default;

inline void setupGLScreen() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

ScreenDisplay2D::ScreenDisplay2D() : pimpl(std::make_unique<impl>()) {
	std::ifstream vert("scripts/screenVert.glsl");
	std::ifstream frag("scripts/screenFrag.glsl");
	std::ifstream geom("scripts/screenGeom.glsl");
	if (!vert.is_open() || !frag.is_open()) throw std::runtime_error("Could not find scripts");
	pimpl->shader = std::make_unique<ScreenShader>(vert, geom, frag);
	std::ifstream postVert("scripts/postprocessVert.glsl");
	std::ifstream postFrag("scripts/postprocessFrag.glsl");
	pimpl->postShader = std::make_unique<ScreenShader>(postVert, postFrag);
	pimpl->proj = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
	setupGLScreen();

}

void ScreenDisplay2D::updateShaderValues()
{
	pimpl->shader->setMat4("projection", pimpl->proj);
}

PointSet::PointSet(GLuint vertices) : verticesPerBatch(vertices)
{
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vertices, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), 
		reinterpret_cast<const void*>(0));
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), 
		reinterpret_cast<const void*>(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	glPointSize(10.0f);
}

PointSet::~PointSet()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}
auto PointSet::drawReadyBatch(decltype(buffer)::const_iterator start) -> decltype(start)
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	const auto newVertices = std::min(verticesPerBatch, (GLuint)std::distance(start, buffer.cend()));
	glBufferSubData(GL_ARRAY_BUFFER, 0,
		static_cast<GLsizei>(sizeof(VertexData) * newVertices),
		&*start);
	glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(newVertices));
	glBindVertexArray(0);
	std::advance(start, newVertices);
	return start;
}
void PointSet::draw(const std::vector<Visual>& vis)
{
	addToBuffer(vis);
	auto start = buffer.cbegin();
	decltype(start) end;
	do {
		end = drawReadyBatch(start);
	} while (end != buffer.cend());
	buffer.clear();
}
void PointSet::addToBuffer(const std::vector<Visual>& v) {
	buffer.insert(buffer.end(), v.begin(), v.end());
}
Rect::Rect() {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2,
		reinterpret_cast<const void*>(0));
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}
Rect::~Rect() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}
void Rect::draw()
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

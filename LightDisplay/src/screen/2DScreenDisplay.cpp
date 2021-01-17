#include <screen/2DScreenDisplay.h>
#include <glad/glad.h>
#include <screen/ScreenShader.h>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <filesystem>
class PointSet {
private:
	GLuint vao, vbo;
#pragma pack(push, 4)
	struct VertexData {
		float x, y;
		float r, g, b, a;
		VertexData(const Visual& v) : x(v.ndcX), y(v.ndcY),
			r(v.color.r / 255.f), g(v.color.g / 255.f), b(v.color.b / 255.f), a(v.color.a / 255.f) {}
	};
#pragma pack(pop)
public:
	PointSet();
	~PointSet();
	PointSet(const PointSet&) = delete;
	PointSet& operator=(const PointSet&) = delete;
	void draw(const std::vector<Visual>& vis);
};
struct ScreenDisplay2D::impl {
	std::unique_ptr<ScreenShader> shader = nullptr;
	glm::mat4 proj = glm::identity<glm::mat4>();
	PointSet points;
};

inline void clearScreen() {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.f, 0.f, 0.f, 1.f);
}

void ScreenDisplay2D::render(const std::vector<Visual>& visuals)
{
	clearScreen();
	updateShaderValues();
	pimpl->points.draw(visuals);
}

void ScreenDisplay2D::resize(int width, int height)
{
	glViewport(0, 0, width, height);
}

ScreenDisplay2D::~ScreenDisplay2D() = default;

ScreenDisplay2D::ScreenDisplay2D() : pimpl(std::make_unique<impl>()) {
	std::ifstream vert("scripts/screenVert.glsl");
	std::ifstream frag("scripts/screenFrag.glsl");
	if (!vert.is_open() || !frag.is_open()) throw std::runtime_error("Could not find scripts");
	pimpl->shader = std::make_unique<ScreenShader>(vert, frag);
	pimpl->proj = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);

}

void ScreenDisplay2D::updateShaderValues()
{
	pimpl->shader->setMat4("projection", pimpl->proj);
}

PointSet::PointSet()
{
	Visual testV(Color::RED, 0, 0);
	VertexData v(testV);
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_VERTEX_ARRAY, vbo);
	glBufferData(GL_VERTEX_ARRAY, sizeof(v), &v, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
		reinterpret_cast<const void*>(0));
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 
		reinterpret_cast<const void*>(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	glEnable(GL_POINTS);
	glPointSize(10.2f);
}

PointSet::~PointSet()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void PointSet::draw(const std::vector<Visual>& vis)
{
	if (vis.empty()) return;
	const auto vec = std::vector<VertexData>{ vis.begin(), vis.end() };
	glBindVertexArray(vao);
//	glBindBuffer(GL_VERTEX_ARRAY, vbo);
//	glBufferData(GL_VERTEX_ARRAY, static_cast<GLsizei>(sizeof(VertexData) * vec.size()), 
//		vec.data(), GL_DYNAMIC_DRAW);
	glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(vec.size()));
	glBindVertexArray(0);
}

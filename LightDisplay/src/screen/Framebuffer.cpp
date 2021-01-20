#include <screen/Framebuffer.h>
#include <stdexcept>
#include <glad/glad.h>
constexpr unsigned int invalid_obj = ~0u;

Framebuffer::Framebuffer(int width, int height) : width(width), height(height)
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, width, height, GL_TRUE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("Incomplete fbo");
	}

	glGenFramebuffers(1, &intermediateFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, intermediateFbo);
	glGenTextures(1, &intermediateTexture);
	glBindTexture(GL_TEXTURE_2D, intermediateTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intermediateTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("Incomplete intermediate fbo");
	}
}

Framebuffer::~Framebuffer()
{
	if (texture != invalid_obj) {
		glDeleteTextures(1, &texture);
		glDeleteTextures(1, &intermediateTexture);
		glDeleteFramebuffers(1, &fbo);
		glDeleteFramebuffers(1, &intermediateFbo);
	}
}

void Framebuffer::enableWrite()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void Framebuffer::enableRead(unsigned int texTarget)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFbo);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE0 + texTarget);
	glBindTexture(GL_TEXTURE_2D, intermediateTexture);
}

void Framebuffer::swap(Framebuffer& other) noexcept
{
	std::swap(width, other.width);
	std::swap(height, other.height);
	std::swap(fbo, other.fbo);
	std::swap(intermediateFbo, other.intermediateFbo);
	std::swap(texture, other.texture);
	std::swap(intermediateTexture, other.intermediateTexture);
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept :
	fbo(other.fbo), texture(other.texture), intermediateFbo(other.intermediateFbo),
	intermediateTexture(other.intermediateTexture), width(other.width),
	height(other.height)
{
	other.fbo = invalid_obj;
	other.texture = invalid_obj;
	other.intermediateFbo = invalid_obj;
	other.intermediateTexture = invalid_obj;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
{
	swap(other);
	return *this;
}

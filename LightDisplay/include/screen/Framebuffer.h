#pragma once
class Framebuffer {
private:
	/// Invariant: the texture objects are all valid or all invalid if they were moved
	unsigned int fbo, texture;
	unsigned int intermediateFbo, intermediateTexture; 
	int width, height;
public:
	Framebuffer(int width, int height);
	~Framebuffer();
	void enableWrite();
	void enableRead(unsigned int texTarget = 0);

	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator=(const Framebuffer&) = delete;

	void swap(Framebuffer& other) noexcept;
	Framebuffer(Framebuffer&& other) noexcept;
	Framebuffer& operator=(Framebuffer&& other) noexcept;
};

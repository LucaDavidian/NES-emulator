#ifndef RENDERING_SYSTEM_H
#define RENDERING_SYSTEM_H

struct SDL_Texture;
class Screen;

class RenderingSystem
{
public:
	static RenderingSystem &GetInstance();

	~RenderingSystem();

	void Render();
private:
	RenderingSystem();
};

#endif  // RENDERING_SYSTEM_H

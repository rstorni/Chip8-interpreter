#pragma once
 
#include <cstdint>
#include <SDL.h>
#include <glad/glad.h>

class GameWindow
{
	friend class Imgui;

public:

	GameWindow(char const* title, int windowWidth, int windowHeight, int texturedWidth, int texturedHeight);
	~GameWindow();
	void Update(void const* buffer, int pitch);
	bool processInput(uint8_t* keys);
	
private:

	SDL_Window* window;
	SDL_GLContext gl_context;
	GLuint framebuffer_texture;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
}; 	
	

#include "gameWindow.h"

GameWindow::GameWindow(char const* title, int windowWidth, int windowHeight, int texturedWidth, int texturedHeight)
{
	//Initializes the SDL Video lib needed for graphics
	SDL_Init(SDL_INIT_VIDEO)
	
	//Specifying the OpenGL Version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	//Create the SDL Window
	window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	//After creating a window set the context
	gl_context = SDL_GL_CreateContext(window);
	SDL_SetSwapInterval(1);
	
	//Setup GL function pointers
	gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress);
	
	//creating a texture to go on our window
	glGenTexture(1, &framebuffer_texture);

	//Binding the created texture
	glBindTexture(GL_TEXTURE_2D, framebuffer_texture);

	//If the texture is shrunk or enlarged use the nearest neighbor algo to map the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FLITER, GL_NEAREST);

	//If the screen is larger than the texture clamp the S and T axis to the edge
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
	
	//Creates an image texture that will be passed to the screen
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 640, 320, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
		
}

GameWindow::~GameWindow()
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void GameWindow::Update(void const* buffer, int pitch)
{
	SDL_UpdateTexture(texture, nullptr, buffer, pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);	
}

bool GameWindow::ProcessInput(uint8_t* keys)
{
	
}

#define SDL_MAIN_HANDLED
#include <SDL.h>

int main() {

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", SDL_GetError(), NULL);

		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Hello, Vulkan!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_VULKAN);

	if (!window)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", SDL_GetError(), NULL);
	}

    SDL_Event event;

    bool isRunning = true;

    while (isRunning)
    {
        SDL_PollEvent(&event);
        switch (event.type)
        {
            case SDL_QUIT:
            {
                isRunning = false;
                break;
            }
            case SDL_KEYDOWN:
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    isRunning = false;
                }
            }
            default:
            {
                break;
            }
        }
    }

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
#include "Button.h"

void Button::show(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLine(renderer, 1100, 75, 1250, 75);
	SDL_RenderDrawLine(renderer, 1100, 75, 1100, 50);
	SDL_RenderDrawLine(renderer, 1100, 50, 1250, 50);
	SDL_RenderDrawLine(renderer, 1250, 50, 1250, 75);
	TTF_Init();
	TTF_Font* Sans = TTF_OpenFont("FreeSans.ttf", 24);
	if (Sans == NULL) {
		fprintf(stderr, "error: font not found\n");
		exit(EXIT_FAILURE);
	}

	SDL_Color colour = { 255, 255, 255 };
	std::string messageS = "Deploy UAVs";
	char const* Char = messageS.c_str();

	SDL_Surface* SurfaceMessage =
		TTF_RenderText_Solid(Sans, Char, colour);
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, SurfaceMessage);

	SDL_Rect Message_rect;

	Message_rect.x = 1110;  //controls the rect's x coordinate 
	Message_rect.y = 50; // controls the rect's y coordinte
	Message_rect.w = 120; // controls the width of the rect
	Message_rect.h = 30; // controls the height of the rect
	SDL_FreeSurface(SurfaceMessage);
	SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
	SDL_DestroyTexture(Message);
}


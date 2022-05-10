#include "BaseStationConditions.h"
void drawTable(SDL_Renderer* renderer, int numberOfChannels, int baseStationID, int caseT)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_Rect rect{rect.x = 30, rect.y = 100 , rect.h = 100, rect.w = 350};
	SDL_RenderDrawRect(renderer, &rect);
	SDL_Rect rect1{ rect.x = 30, rect.y = 30 , rect.h = 350, rect.w = 70 };
	SDL_RenderDrawRect(renderer, &rect1);

	TTF_Init();
	TTF_Font* Sans = TTF_OpenFont("FreeSans.ttf", 24);
	if (Sans == NULL) {
		fprintf(stderr, "error: font not found\n");
		exit(EXIT_FAILURE);
	}

	std::string id = std::to_string(baseStationID);
	char const* idChar = id.c_str();

	std::string numUsers = std::to_string(numberOfChannels);
	char const* userChar = numUsers.c_str();

	SDL_Color colour = { 255, 255, 255 };
	if (numberOfChannels >= 20 && numberOfChannels<30)
	{
		colour = { 255, 165, 0 };
	}
	else if (numberOfChannels >= 20)
	{
		colour = { 255, 0, 0 };
	}
	SDL_Surface* idSurfaceMessage =
		TTF_RenderText_Solid(Sans, idChar, colour);
	SDL_Texture* idMessage = SDL_CreateTextureFromSurface(renderer, idSurfaceMessage);

	SDL_Surface* userSurfaceMessage =
		TTF_RenderText_Solid(Sans, userChar, colour);
	SDL_Texture* userMessage = SDL_CreateTextureFromSurface(renderer, userSurfaceMessage);

	SDL_Rect idMessage_rect; //create a rect
	idMessage_rect.x = 80;  //controls the rect's x coordinate 
	idMessage_rect.y = 70 + (35 * baseStationID); // controls the rect's y coordinte
	idMessage_rect.w = 20; // controls the width of the rect
	idMessage_rect.h = 20; // controls the height of the rect
	SDL_FreeSurface(idSurfaceMessage);
	SDL_RenderCopy(renderer, idMessage, NULL, &idMessage_rect);
	SDL_DestroyTexture(idMessage);


	//titles of the colomns

	SDL_Color colour1 = { 255, 255, 255 };
	std::string  title1String = "BS ID";
	char const* title1Char = title1String.c_str();

	SDL_Surface* title1 =
		TTF_RenderText_Solid(Sans, title1Char, colour1);
	SDL_Texture* title1Message = SDL_CreateTextureFromSurface(renderer, title1);
	SDL_Rect title1_rect;

	title1_rect.x = 75;  //controls the rect's x coordinate 
	title1_rect.y = 55; // controls the rect's y coordinte
	title1_rect.w = 40; // controls the width of the rect
	title1_rect.h = 30; // controls the height of the rect
	SDL_FreeSurface(title1);
	SDL_RenderCopy(renderer, title1Message, NULL, &title1_rect);
	SDL_DestroyTexture(title1Message);


	std::string  title2String = "Channels before";
	char const* title2Char = title2String.c_str();

	SDL_Surface* title2 =
		TTF_RenderText_Solid(Sans, title2Char, colour1);
	SDL_Texture* title2Message = SDL_CreateTextureFromSurface(renderer, title2);
	SDL_Rect title2_rect;

	title2_rect.x = 135;  //controls the rect's x coordinate 
	title2_rect.y = 40; // controls the rect's y coordinte
	title2_rect.w = 110; // controls the width of the rect
	title2_rect.h = 30; // controls the height of the rect
	SDL_FreeSurface(title2);
	SDL_RenderCopy(renderer, title2Message, NULL, &title2_rect);
	SDL_DestroyTexture(title2Message);


	std::string  title2_1String = "UAV deployment";
	char const* title2_1Char = title2_1String.c_str();

	SDL_Surface* title2_1 =
		TTF_RenderText_Solid(Sans, title2_1Char, colour1);
	SDL_Texture* title2_1Message = SDL_CreateTextureFromSurface(renderer, title2_1);
	SDL_Rect title2_1_rect;

	title2_1_rect.x = 135;  //controls the rect's x coordinate 
	title2_1_rect.y = 60; // controls the rect's y coordinte
	title2_1_rect.w = 110; // controls the width of the rect
	title2_1_rect.h = 30; // controls the height of the rect
	SDL_FreeSurface(title2_1);
	SDL_RenderCopy(renderer, title2_1Message, NULL, &title2_1_rect);
	SDL_DestroyTexture(title2_1Message);


	std::string  title3String = "Channels after";
	char const* title3Char = title3String.c_str();

	SDL_Surface* title3 =
		TTF_RenderText_Solid(Sans, title3Char, colour1);
	SDL_Texture* title3Message = SDL_CreateTextureFromSurface(renderer, title3);
	SDL_Rect title3_rect;

	title3_rect.x = 260;  //controls the rect's x coordinate 
	title3_rect.y = 40; // controls the rect's y coordinte
	title3_rect.w = 110; // controls the width of the rect
	title3_rect.h = 30; // controls the height of the rect
	SDL_FreeSurface(title3);
	SDL_RenderCopy(renderer, title3Message, NULL, &title3_rect);
	SDL_DestroyTexture(title3Message);


	std::string  title3_1String = "UAV deployment";
	char const* title3_1Char = title2_1String.c_str();

	SDL_Surface* title3_1 =
		TTF_RenderText_Solid(Sans, title3_1Char, colour1);
	SDL_Texture* title3_1Message = SDL_CreateTextureFromSurface(renderer, title3_1);
	SDL_Rect title3_1_rect;

	title3_1_rect.x = 260;  //controls the rect's x coordinate 
	title3_1_rect.y = 60; // controls the rect's y coordinte
	title3_1_rect.w = 110; // controls the width of the rect
	title3_1_rect.h = 30; // controls the height of the rect
	SDL_FreeSurface(title3_1);
	SDL_RenderCopy(renderer, title3_1Message, NULL, &title3_1_rect);
	SDL_DestroyTexture(title3_1Message);



	//2 seperate calls. before/after UAV deployment

	switch (caseT)
	{
	case 1:
		SDL_Rect userMessage_rect; //create a rect
		userMessage_rect.x = 180;  //controls the rect's x coordinate 
		userMessage_rect.y = 70 + (35 * baseStationID); // controls the rect's y coordinte
		userMessage_rect.w = 20; // controls the width of the rect
		userMessage_rect.h = 20; // controls the height of the rect
		SDL_FreeSurface(userSurfaceMessage);
		SDL_RenderCopy(renderer, userMessage, NULL, &userMessage_rect);
		SDL_DestroyTexture(userMessage);
		break;
	case 2:
		//SDL_Rect userMessage_rect; //create a rect
		userMessage_rect.x = 280;  //controls the rect's x coordinate 
		userMessage_rect.y = 70 + (35 * baseStationID); // controls the rect's y coordinte
		userMessage_rect.w = 20; // controls the width of the rect
		userMessage_rect.h = 20; // controls the height of the rect
		SDL_FreeSurface(userSurfaceMessage);
		SDL_RenderCopy(renderer, userMessage, NULL, &userMessage_rect);
		SDL_DestroyTexture(userMessage);
		break;
	}
}

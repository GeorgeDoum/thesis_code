#include <iostream>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include "SDL_render.h"
#include <SDL_ttf.h>
#include <math.h>
#include <vector>
#include <map>
#include "Hexagon.cpp"
#include "Hexagon.h"
#include "BaseStation.h"
#include "BaseStation.cpp"
#include "User.h"
#include "User.cpp"

int main(int argc, char* argv[])
{
	int length_of_side = 90;
	int distance_to_side =0;
	std::vector<int> x_points{ 625 ,490 ,490 ,760 ,625 ,625 ,760 ,760 , 625, 490};
	std::vector<int> y_points{ 145 ,225 ,380 ,225 ,455 ,300 ,380 , 535 ,610, 535};
	std::vector<Hexagon> hexagons;
	std::vector<BaseStation> baseStations;
	std::multimap<int, User> users;
	for (int i = 0; i < x_points.size(); i++)
	{
		Hexagon hexa(length_of_side, distance_to_side, x_points.at(i), y_points.at(i) , i+1);
		hexagons.push_back(hexa);
		hexagons.at(i).FillTheVectors();
		int baseStationCenterX = hexagons.at(i).getXofHexagonCenter();
		int baseStationCenterY = hexagons.at(i).getYofHexagonCenter();
		BaseStation station (baseStationCenterX, baseStationCenterY, i+1);
		baseStations.push_back(station);
	}

	if (SDL_Init(SDL_INIT_VIDEO) == 0) {
		SDL_Window* window = NULL;
		SDL_Renderer* renderer = NULL;
		const char* title = "UAV Optimal Placement";

		if (SDL_CreateWindowAndRenderer(1280, 800, 0, &window, &renderer) == 0) {
			SDL_bool done = SDL_FALSE;
			while (!done) {
				SDL_Event event;
				SDL_SetWindowTitle(window, title);
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
				SDL_RenderClear(renderer);
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
				for (int i = 0; i < hexagons.size(); i++)
				{
					hexagons.at(i).DrawHexagon(renderer);
					baseStations.at(i).DrawLocation(renderer);
				}
				srand(time(NULL));
				
				for (int i = 0; i < x_points.size(); i++)
				{
					int t = rand() % 20;
					int temp = 0;
					do
					{
						int userX = rand() % 850;
						int userY = rand() % 687;
						if (hexagons.at(i).CheckIfisInside(userX, userY) == true)
						{
							User user(userX, userY, i + 1);
							users.insert(std::pair<int, User>(i + 1, user));
							temp++;
							SDL_RenderDrawPoint(renderer, userX, userY);
						}
					} while (temp <= t);
				}

				SDL_RenderPresent(renderer);
				while (SDL_PollEvent(&event)) {
					if (event.type == SDL_QUIT) {
						done = SDL_TRUE;
					}
				}
			}
		}
		if (renderer) {
			SDL_DestroyRenderer(renderer);
		}
		if (window) {
			SDL_DestroyWindow(window);
		}
	}
	SDL_Quit();
	return 0;
}




/*
* points for hexagons
int x = 625;
int y = 145;

int x1 = 490;
int y1=225;

int x2 = 490;
int y2 = 380;

int x3 = 760;
int y3 = 225;

int x4 = 625;
int y4 = 455;

int x5 = 625;
int y5 = 300;

int x6 = 760;
int y6 = 380;

x_points.push_back(625);
x_points.push_back(490);
x_points.push_back(490);
x_points.push_back(760);
x_points.push_back(625);
x_points.push_back(625);
x_points.push_back(760);

y_points.push_back(145);
y_points.push_back(225);
y_points.push_back(380);
y_points.push_back(225);
y_points.push_back(455);
y_points.push_back(300);
y_points.push_back(380);*/
//distance_to_side = sqrt(3) * (length_of_side / 2);






/*
// You shouldn't really use this statement, but it's fine for small programs
using namespace std;

// You must include the command line parameters for your main function to be recognized by SDL
int main(int argc, char** args) {

	// Pointers to our window and surface
	SDL_Surface* winSurface = NULL;
	SDL_Window* window = NULL;

	// Initialize SDL. SDL_Init will return -1 if it fails.
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		cout << "Error initializing SDL: " << SDL_GetError() << endl;
		system("pause");
		// End the program
		return 1;
	}

	// Create our window
	window = SDL_CreateWindow("Uav optimal Placement", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN);

	// Make sure creating the window succeeded
	if (!window) {
		cout << "Error creating window: " << SDL_GetError() << endl;
		system("pause");
		// End the program
		return 1;
	}

	// Get the surface from the window
	winSurface = SDL_GetWindowSurface(window);

	// Make sure getting the surface succeeded
	if (!winSurface) {
		cout << "Error getting surface: " << SDL_GetError() << endl;
		system("pause");
		// End the program
		return 1;
	}

	// Fill the window with a white rectangle
	SDL_FillRect(winSurface, NULL, SDL_MapRGB(winSurface->format, 255, 255, 255));

	// Update the window display
	SDL_UpdateWindowSurface(window);

	// Wait
	system("pause");

	// Destroy the window. This will also destroy the surface
	SDL_DestroyWindow(window);

	// Quit SDL
	SDL_Quit();
	// End the program
	return 0;
}*/


/*
for (int i = 0; i < x_points.size(); i++)
{
	SDL_RenderDrawLine(renderer, (x_points.at(i) + (length_of_side / 2)), (y_points.at(i) - distance_to_side), (x_points.at(i) + length_of_side), y_points.at(i));
	SDL_RenderDrawLine(renderer, (x_points.at(i) + length_of_side), y_points.at(i), (x_points.at(i) + (length_of_side / 2)), (y_points.at(i) + distance_to_side));

	SDL_RenderDrawLine(renderer, (x_points.at(i) + (length_of_side / 2)), (y_points.at(i) + distance_to_side), (x_points.at(i) - (length_of_side / 2)), (y_points.at(i) + distance_to_side)); //test

	SDL_RenderDrawLine(renderer, (x_points.at(i) - (length_of_side/2)), (y_points.at(i) + distance_to_side), (x_points.at(i) - length_of_side), y_points.at(i));
	SDL_RenderDrawLine(renderer, (x_points.at(i) - length_of_side), y_points.at(i), (x_points.at(i) - (length_of_side / 2)), (y_points.at(i) - distance_to_side));
	SDL_RenderDrawLine(renderer, (x_points.at(i) - (length_of_side / 2)), (y_points.at(i) - distance_to_side), (x_points.at(i) + (length_of_side / 2)), (y_points.at(i) - distance_to_side));
	x0 = (x_points.at(i) - (length_of_side / 2));
	y0 = (y_points.at(i) + distance_to_side);
	xc = x0 - distance_to_side + length_of_side + distance_to_side -(distance_to_side/2) - 5;
	yc = y0 - distance_to_side + 1,5*length_of_side;

	SDL_RenderDrawPoint(renderer, xc, yc);
}
SDL_RenderPresent(renderer);
*/
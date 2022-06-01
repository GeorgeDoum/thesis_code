#include <iostream>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include "SDL_render.h"
#include <SDL_ttf.h>
#include <math.h>
#include <string>
#include <vector>
#include <map>
#include "Hexagon.h"
#include "BaseStation.h"
#include "BaseStationConditions.h"
#include "User.h"
#include "checkSurroundings.h"
#include "Button.h"
#include "Optimal_points.h"
#include "Drone.h"
#include <chrono>
#include <thread>
#include <dos.h>
int main(int argc, char* argv[])
{
	int length_of_side = 90;
	int distance_to_side =0;
	std::vector<int> x_points{ 625 ,490, 760, 625, 490, 760, 625, 490, 625, 760};
	std::vector<int> y_points{ 145, 225, 225, 300, 380, 380, 455, 535, 610, 535};

	std::vector<Hexagon> hexagons;
	std::vector<BaseStation> baseStations;
	std::vector<User> users;
	std::vector<Drone> drones;

	if (SDL_Init(SDL_INIT_VIDEO) == 0) {
		SDL_Window* window = NULL;
		SDL_Renderer* renderer = NULL;

		const char* title = "UAV Optimal Placement";

		if (SDL_CreateWindowAndRenderer(1280, 800, 0, &window, &renderer) == 0) {
			SDL_bool done = SDL_FALSE;
			SDL_Event event;
			SDL_SetWindowTitle(window, title);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderClear(renderer);
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

			srand(time(NULL));
			//create objects for each entity of the simulator (hexagons, baseStations etc)
			for (int i = 0; i < x_points.size(); i++)
			{
				Hexagon hexa(length_of_side, distance_to_side, x_points.at(i), y_points.at(i), i + 1);
				hexagons.push_back(hexa);
				hexagons.at(i).FillTheVectors();
				int baseStationCenterX = hexagons.at(i).getXofHexagonCenter();
				int baseStationCenterY = hexagons.at(i).getYofHexagonCenter();
				BaseStation station (baseStationCenterX, baseStationCenterY, i+1);
				baseStations.push_back(station);
				int t = rand() % 30;
				int temp = 0;
				//create and post random number of users at random places across the simulated areas of cells
				do
				{
					int userX = rand() % 850;
					int userY = rand() % 687;
					if (hexagons.at(i).CheckIfisInside(userX, userY) == true)
					{
						User user(userX, userY, i + 1);
						users.push_back(user);
						temp++;
						SDL_RenderDrawPoint(renderer, userX, userY);
						double channel = baseStations.at(i).provideService(user, 0);
						users.back().setChannel(channel);
					}
				} while (temp <= t);
			}
			// Handover technique. Check if surrounding baseStations offer a better channel for each user.
			for (User& user : users)
			{
				checkSurroundingBasestations(user, baseStations, user.getStationId()-1);
			}
			//Draw table with channel load displays
			for (BaseStation& station : baseStations)
			{
				drawTable(renderer, station.getNumberOfChannels(), station.getID(), 1);
			}
			Button uavButton;
			uavButton.show(renderer);
			int deploymentcounter = 0;
			while (!done) {

				for (int i = 0; i < hexagons.size(); i++)
				{
					hexagons.at(i).DrawHexagon(renderer);
					baseStations.at(i).DrawLocation(renderer);
				}
				SDL_RenderPresent(renderer);
				while (SDL_PollEvent(&event)) {
					if (event.type == SDL_QUIT) {
						done = SDL_TRUE;
					}
					/* DEPLOY UAVs */
					if (event.type == SDL_MOUSEBUTTONDOWN)
					{
						//if button "Deploy UAVS" is pushed
						if ( (event.button.x >= 1100 && event.button.x <= 1250 )
							&& (event.button.y >= 50 && event.button.y <= 75) && (deploymentcounter == 0))
						{
							//Deploy UAVs
							deploymentcounter++;
							drones = calculateOptimalPoints(baseStations, users, renderer);
							//update baseStation Load Table
							for (BaseStation& station : baseStations)
							{
								drawTable(renderer, station.getNumberOfChannels(), station.getID(), 2);
							}
						}
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
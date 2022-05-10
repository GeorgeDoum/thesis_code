#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "SDL_render.h"
#include "User.h"
#include <vector>
#include <random>
#include <map>
#include <math.h>
#include <complex>
class Drone
{
public:

	Drone(int x, int y, SDL_Renderer* renderer, int id);

	void setX(int x);

	int getX();

	void setY(int y);

	int getY();

	void setHexagonID(int id);

	int getHexagonID();

	void setPower();

	void publishPosition(int x, int y, SDL_Renderer* renderer);

	std::vector<int> provideService(std::vector<User>& users);

	double calculatePathLoss(User& user);

	double ricianFadingChannel(double variance);

	double rayleighFadingChannel(double variance);

private:
	int battery_power; //in percent
	int x_axis;
	int y_axis;
	int hexagonID;
	int frequency = 2000; //in Mhz
	int height_Drone = 200; //in meters
	std::multimap<int, double> assignedChannels;
};


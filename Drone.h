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

	Drone(int x, int y, int id, SDL_Renderer* renderer);

	void setX(int x);

	int getX();

	void setY(int y);

	int getY();

	int getId();

	void setHexagonID(int id);

	int getHexagonID();

	void setDroneId(int i);

	void publishPosition(int x, int y, SDL_Renderer* renderer);

	std::vector<int> provideService(std::map<int, User> clusterUsers);

	double calculatePathLoss(User& user);

	double ricianFadingChannel(double variance);

	double rayleighFadingChannel(double variance);

	std::multimap<int, double> getChannels()
	{
		return assignedChannels;
	}

	std::multimap<int, double> getPathlosses()
	{
		return assignedPathloss;
	}

	double getPrs()
	{
		return Prs;
	}
private:
	int x_axis;
	int y_axis;
	int id;
	int hexagonID;
	int frequency = 2000; //in Mhz
	int height_Drone = 200; //in meters
	std::multimap<int, double> assignedChannels;
	std::multimap<int, double> assignedPathloss;
	int BandWidth = 5000000; //in Hz (5MHz)
	double Prs = 23; //in dB
};


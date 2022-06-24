#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "SDL_render.h"
#include <math.h>
#include <vector>
#include <string>
#include <map>
#include "User.h"
#include <cstdlib>
#include <math.h>
#include <random>
#include <complex> 
class BaseStation
{
public:
	BaseStation(int x, int y, int hex_ID);

	void setXaxis(int x);

	void setYaxis(int y);

	void setID(int hex_ID);

	int getX();

	int getY();

	int getID();

	double getPrs();

	int getNumberOfChannels();

	void DrawLocation(SDL_Renderer* renderer);

	void StationStatus();

	void assignChannel(double channel, int userId);

	double provideService(User& user, int temp);

	double calculatePathLoss(User& user);

	double rayleigh_fading(long double variance);

	//After the handover to new BaseStations we erase the already ocupied channels of the current baseStation
	void eraseChannels(std::vector<int>& ids);

	std::multimap<int, double> getChannels()
	{
		return assignedChannels;
	}

	std::multimap<int, double> getPathlosses()
	{
		return assignedPathlosses;
	}
private:
	int x_axis;
	int y_axis;
	int hexagonId;
	int frequency = 2000; //Mhz
	std::multimap<int,double> assignedChannels;
	std::multimap<int, double> assignedPathlosses;
	int heightOfStationAntenna = 35; //in meters
	double Prs = 43;  //transmit power in dBm
	int BandWidth = 5000000; //in Hz (5MHz)
};

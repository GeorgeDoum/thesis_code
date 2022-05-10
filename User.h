#pragma once
#include "SDL_render.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <SDL.h>
#include <SDL_ttf.h>

class User
{
public:

	User(int x, int y, int id);

	void setXaxis(int x);

	void setYaxis(int y);

	void setStationId(int id);

	void setChannel (double& cha);
	
	void setUniqueID(int id);

	int getAntennaHeight();

	int getX();

	int getY();
	
	int getUniqueID();

	double getChannel();

	int getStationId();

	void DrawLocation(SDL_Renderer* renderer);

private:
	int x_axis;
	int y_axis;
	int stationId;
	int heightOfUserAntenna;
	int uniqueID;
	double channel;
};
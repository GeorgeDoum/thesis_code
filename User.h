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

	void setDroneId(int id);

	void setChannel (double& cha);
	
	void setUniqueID(int id);

	void setPathLoss(double ploss);

	void setTemporaryPathLoss(double loss);

	void setSINR(double s_i_n_r);

	int getAntennaHeight();

	int getX();

	int getY();
	
	int getUniqueID();

	double getChannel();

	int getStationId();

	double getPathLoss();

	double getTemporaryPathLoss();

	double getSINR();
	
	int getDroneId();

	void DrawLocation(SDL_Renderer* renderer);

	void setNumU(int i);
	int getNumU();
private:
	int x_axis;
	int y_axis;
	int stationId;
	int droneId;
	int heightOfUserAntenna;
	int uniqueID;
	double channel;
	double pathLoss;
	double tempPathLoss;
	double SINR;
	int droneNumOfUsers;
};
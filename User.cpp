#include "User.h"

User::User(int x, int y , int id)
{
	setXaxis(x);
	setYaxis(y);
	setStationId(id);
	setUniqueID(getStationId());
}

void User::setXaxis(int x)
{
	x_axis = x;
}

void User::setYaxis(int y)
{
	y_axis = y;
}

void User::setStationId(int id)
{
	stationId = id;
}

void User::setUniqueID(int id)
{
	int t = rand() % 999 + 100;
	uniqueID = id * t;
}
void User::setChannel(double& cha)
{
	channel = cha;
}

double User::getChannel()
{
	return channel;
}

int User::getX()
{
	return x_axis;
}

int User::getY()
{
	return y_axis;
}

int User::getStationId()
{
	return stationId;
}

int User::getAntennaHeight()
{
	int t = rand() % 10 + 1;
	heightOfUserAntenna = t;
	return heightOfUserAntenna;
}

int User::getUniqueID()
{
	return uniqueID;
}

void User::DrawLocation(SDL_Renderer* renderer)
{
	SDL_RenderDrawPoint(renderer, x_axis, y_axis);
}
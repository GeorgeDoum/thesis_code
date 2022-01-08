#include "BaseStation.h"
#include <iostream>
#include <SDL.h>
#include "SDL_render.h"
#include <math.h>
#include <vector>
#include <SDL_ttf.h>
class BaseStation
{
public:
	BaseStation(int x, int y, int hex_ID)
	{
		setXaxis(x);
		setYaxis(y);
		setID(hex_ID);
	}
	void setXaxis(int x)
	{
		x_axis = x;
	}
	void setYaxis(int y)
	{
		y_axis = y;
	}
	void setID(int hex_ID)
	{
		hexagonId = hex_ID;
	}
	int getX()
	{
		return x_axis;
	}
	int getY()
	{
		return y_axis;
	}
	int getID()
	{
		return hexagonId;
	}
	void DrawLocation(SDL_Renderer* renderer)
	{
		SDL_RenderDrawPoint(renderer, x_axis, y_axis);
	}
	void StationStatus()
	{
		std::cout << "BaseStation No: " << hexagonId << std::endl;
		std::cout << "Xpoint: " << x_axis << std::endl;
		std::cout << "Ypoint: " << y_axis << std::endl;
		std::cout << std::endl;
	}
private:
	int x_axis;
	int y_axis;
	int hexagonId;
	int transmit_power;
};
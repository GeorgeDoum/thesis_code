#pragma once
#include <iostream>
#include <SDL.h>
#include "SDL_render.h"
#include <math.h>
#include <vector>

class Hexagon
{
public:
	Hexagon(int len_of_side, int dist_to_side, int x, int y, int id);

	void setLengthOfSide(int length_side);

	void setDistanceToSide(int distance_side, int length_side);

	void setXaxis(int x);

	void setYaxis(int y);

	void setID(int id);

	int getLengthOfSide();

	int getDistanceToSide();

	int getXaxis();

	int getYaxis();

	int getID();

	void DrawHexagon(SDL_Renderer* renderer);

	int getXofHexagonCenter();

	int getYofHexagonCenter();

	void DrawCenterOfHexagon(SDL_Renderer* renderer);

	void FillTheVectors();

	bool CheckIfisInside(int userX, int userY);

private:
	int length_of_side;
	int distance_to_side;
	int x_axis;
	int y_axis;
	int pointOfUpperLeftX;
	int pointOfUpperLeftY;
	int centerX;
	int centerY;
	int ID;
	std::vector<int> peaksX;
	std::vector<int> peaksY;
};

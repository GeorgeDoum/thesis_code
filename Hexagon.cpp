#include "Hexagon.h"
#include <iostream>
#include <SDL.h>
#include "SDL_render.h"
#include <math.h>
#include <vector>
class Hexagon
{
public:
	Hexagon(int len_of_side, int dist_to_side, int x, int y, int id)
	{
		setLengthOfSide(len_of_side);
		setDistanceToSide(dist_to_side, length_of_side);
		setXaxis(x);
		setYaxis(y);
		setID(id);
	}
	void setLengthOfSide(int length_side)
	{
		length_of_side = length_side;
	}
	void setDistanceToSide(int distance_side, int length_side)
	{
		distance_to_side = sqrt(3) * (length_of_side / 2);
	}
	void setXaxis(int x)
	{
		x_axis = x;
	}
	void setYaxis(int y)
	{
		y_axis = y;
	}
	void setID(int id)
	{
		ID = id;
	}
	int getLengthOfSide()
	{
		return length_of_side;
	}
	int getDistanceToSide()
	{
		return distance_to_side;
	}
	int getXaxis()
	{
		return x_axis;
	}
	int getYaxis()
	{
		return y_axis;
	}
	int getID()
	{
		return ID;
	}
	void DrawHexagon(SDL_Renderer* renderer)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawLine(renderer, (x_axis + (length_of_side / 2)), (y_axis - distance_to_side), (x_axis + length_of_side), y_axis); //p1
		SDL_RenderDrawLine(renderer, (x_axis + length_of_side), y_axis, (x_axis + (length_of_side / 2)), (y_axis + distance_to_side)); //p2

		SDL_RenderDrawLine(renderer, (x_axis + (length_of_side / 2)), (y_axis + distance_to_side), (x_axis - (length_of_side / 2)), (y_axis + distance_to_side)); //test p3

		SDL_RenderDrawLine(renderer, (x_axis - (length_of_side / 2)), (y_axis + distance_to_side), (x_axis - length_of_side), y_axis); //p4
		SDL_RenderDrawLine(renderer, (x_axis - length_of_side), y_axis, (x_axis - (length_of_side / 2)), (y_axis - distance_to_side)); //p5
		SDL_RenderDrawLine(renderer, (x_axis - (length_of_side / 2)), (y_axis - distance_to_side), (x_axis + (length_of_side / 2)), (y_axis - distance_to_side));
	}
	int getXofHexagonCenter()
	{
		pointOfUpperLeftX = (x_axis - (length_of_side / 2));
		centerX = pointOfUpperLeftX - distance_to_side + length_of_side + distance_to_side - (distance_to_side / 2) - 5;
		return centerX;
	}

	int getYofHexagonCenter()
	{
		pointOfUpperLeftY = (y_axis + distance_to_side);
		centerY = pointOfUpperLeftY - distance_to_side + 1, 5 * length_of_side;
		return centerY;
	}
	
	void DrawCenterOfHexagon(SDL_Renderer* renderer)
	{
		pointOfUpperLeftX = (x_axis - (length_of_side / 2));
		pointOfUpperLeftY = (y_axis + distance_to_side);
		centerX = pointOfUpperLeftX - distance_to_side + length_of_side + distance_to_side - (distance_to_side / 2) - 5;
		centerY = pointOfUpperLeftY - distance_to_side + 1, 5 * length_of_side;
		SDL_RenderDrawPoint(renderer, centerX, centerY);
	}
	void FillTheVectors()
	{
		/*ksekiname apo thn panw deksia korufh ths kupselhs  kai akolouthoume thn fora tou rologiou
* oi times parthikan apo thn drawHexagon
*/
		peaksX.push_back((x_axis + (length_of_side / 2)));
		peaksX.push_back((x_axis + length_of_side));
		peaksX.push_back((x_axis + (length_of_side / 2)));
		peaksX.push_back((x_axis - (length_of_side / 2)));
		peaksX.push_back((x_axis - length_of_side));
		peaksX.push_back((x_axis - (length_of_side / 2)));

		peaksY.push_back((y_axis - distance_to_side));
		peaksY.push_back(y_axis);
		peaksY.push_back((y_axis + distance_to_side));
		peaksY.push_back((y_axis + distance_to_side));
		peaksY.push_back(y_axis);
		peaksY.push_back((y_axis - distance_to_side));

	}
	bool CheckIfisInside(int userX, int userY)
	{
		if ( (userX < peaksX.at(0) && userX > peaksX.at(3)) && (userY > peaksY.at(0) && userY < peaksY.at(2) )  )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
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

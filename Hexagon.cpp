#include "Hexagon.h"

Hexagon::Hexagon(int len_of_side, int dist_to_side, int x, int y, int id)
{
	setLengthOfSide(len_of_side);
	setDistanceToSide(dist_to_side, length_of_side);
	setXaxis(x);
	setYaxis(y);
	setID(id);
}

void Hexagon::setLengthOfSide(int length_side)
{
	length_of_side = length_side;
}

void Hexagon::setDistanceToSide(int distance_side, int length_side)
{
	distance_to_side = sqrt(3) * (length_of_side / 2);
}

void Hexagon::setXaxis(int x)
{
	x_axis = x;
}

void Hexagon::setYaxis(int y)
{
	y_axis = y;
}

void Hexagon::setID(int id)
{
	ID = id;
}

int Hexagon::getLengthOfSide()
{
	return length_of_side;
}

int Hexagon::getDistanceToSide()
{
	return distance_to_side;
}

int Hexagon::getXaxis()
{
	return x_axis;
}

int Hexagon::getYaxis()
{
	return y_axis;
}

int Hexagon::getID()
{
	return ID;
}

void Hexagon::DrawHexagon(SDL_Renderer* renderer)
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

int Hexagon::getXofHexagonCenter()
{
	pointOfUpperLeftX = (x_axis - (length_of_side / 2));
	centerX = pointOfUpperLeftX - distance_to_side + length_of_side + distance_to_side - (distance_to_side / 2) - 5;
	return centerX;
}

int Hexagon::getYofHexagonCenter()
{
	pointOfUpperLeftY = (y_axis + distance_to_side);
	centerY = pointOfUpperLeftY - distance_to_side + 1, 5 * length_of_side;
	return centerY;
}

void Hexagon::DrawCenterOfHexagon(SDL_Renderer* renderer)
{
	pointOfUpperLeftX = (x_axis - (length_of_side / 2));
	pointOfUpperLeftY = (y_axis + distance_to_side);
	centerX = pointOfUpperLeftX - distance_to_side + length_of_side + distance_to_side - (distance_to_side / 2) - 5;
	centerY = pointOfUpperLeftY - distance_to_side + 1, 5 * length_of_side;
	SDL_RenderDrawPoint(renderer, centerX, centerY);
}

void Hexagon::FillTheVectors()
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

bool Hexagon::CheckIfisInside(int userX, int userY)
{
	if ((userX < peaksX.at(0) && userX > peaksX.at(3)) && (userY > peaksY.at(0) && userY < peaksY.at(2)))
	{
		return true;
	}
	else
	{
		return false;
	}
}


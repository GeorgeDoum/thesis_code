#include "User.h"
#include "SDL_render.h"
class User
{
public:
	User(int x, int y , int id)
	{
		setXaxis(x);
		setYaxis(y);
		hexagonId = id;
	}
	User()
	{

	}
	void setXaxis(int x)
	{
		x_axis = x;
	}
	void setYaxis(int y)
	{
		y_axis = y;
	}
	int getX()
	{
		return x_axis;
	}
	int getY()
	{
		return y_axis;
	}
	void DrawLocation(SDL_Renderer* renderer)
	{
		SDL_RenderDrawPoint(renderer, x_axis, y_axis);
	}
private:
	int x_axis;
	int y_axis;
	int hexagonId;
};

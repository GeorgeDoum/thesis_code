#include "BaseStation.h"

BaseStation::BaseStation(int x, int y, int hex_ID)
{
	setXaxis(x);
	setYaxis(y);
	setID(hex_ID);
}

void BaseStation::setXaxis(int x)
{
	x_axis = x;
}

void BaseStation::setYaxis(int y)
{
	y_axis = y;
}

void BaseStation::setID(int hex_ID)
{
	hexagonId = hex_ID;
}

int BaseStation::getX()
{
	return x_axis;
}

int BaseStation::getY()
{
	return y_axis;
}

int BaseStation::getID()
{
	return hexagonId;
}

int BaseStation::getNumberOfChannels()
{
	return assignedChannels.size();
}

void BaseStation::DrawLocation(SDL_Renderer* renderer)
{
	TTF_Init();
	TTF_Font* Sans = TTF_OpenFont("FreeSans.ttf", 24);
	if (Sans == NULL) {
		fprintf(stderr, "error: font not found\n");
		exit(EXIT_FAILURE);
	}

	std::string s = std::to_string(hexagonId);
	char const* pchar = s.c_str();

	SDL_Color White = { 255, 255, 255 };
	SDL_Surface* surfaceMessage =
		TTF_RenderText_Solid(Sans, pchar, White);
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	SDL_Rect Message_rect; //create a rect
	Message_rect.x = x_axis - 10;  //controls the rect's x coordinate 
	Message_rect.y = y_axis - 10; // controls the rect's y coordinte
	Message_rect.w = 20; // controls the width of the rect
	Message_rect.h = 20; // controls the height of the rect
	SDL_FreeSurface(surfaceMessage);
	SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
	SDL_DestroyTexture(Message);
}

void BaseStation::StationStatus()
{
	std::cout << "BaseStation No: " << hexagonId << std::endl;
	std::cout << "Xpoint: " << x_axis << std::endl;
	std::cout << "Ypoint: " << y_axis << std::endl;
	std::cout << std::endl;
}

//COST 231 Hata model for pathloss
double BaseStation::calculatePathLoss(User& user)
{
	int xOfUser = user.getX();
	int yOfUser = user.getY();
	int distX = abs(x_axis - xOfUser);
	int distY = abs(y_axis - yOfUser);
	int linkDistance = sqrt((distX ^ 2) + (distY ^ 2));
	int hR = user.getAntennaHeight();

	//in this simulator we assume we are in an urban area thus we calculate the mobile antenna correction as follows
	double aHR = (3.20 * (log10(11.75 * hR)) * 2) - 4.97;
	// C = 0 we are not simulating a metropolitan area

	double pathloss = (46.3) + (33.9*log(frequency)) - (13.82*log(heightOfStationAntenna)) - (aHR) + ((44.9 - 6.55*log(heightOfStationAntenna))*(log(linkDistance)));

	return pathloss;
}

double BaseStation::rayleigh_fading(long double variance)
{
	double min_real = 0.00000000000000001;
	double max_real = 0.00000000000000009;
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<double> distr(min_real, max_real);
	double randomRealSide = distr(eng);

	double min_imaginarry = 0.1;
	double max_imaginarry = 0.9;
	std::random_device rd1;
	std::default_random_engine eng1(rd1());
	std::uniform_real_distribution<double> distr1(min_imaginarry, max_imaginarry); //normal_dist
	double channelImaginarrySide = distr1(eng1);

	long double channelRealSide = sqrt(variance/2)*randomRealSide;
	std::complex<double> complexNumberForChannel (channelRealSide, channelImaginarrySide);

	double absoluteofComplexNumber = std::abs(complexNumberForChannel);
	double channel = absoluteofComplexNumber * absoluteofComplexNumber;

	return channel;
}

void BaseStation::assignChannel(double channel, int userId)
{
	assignedChannels.insert(std::pair<int, double>(userId, channel));
}


double BaseStation::provideService(User& user)
{
	int userId = user.getUniqueID();
	double pathloss = calculatePathLoss(user);
	double pathLossTemp = pathloss / 10;;
	long double variance = pow(10, pathLossTemp);
	double channel = rayleigh_fading(variance);

	return channel;
}

void BaseStation::eraseChannels(std::vector<int>& ids)
{
	for (auto& id : ids)
	{
		std::multimap<int, double>::iterator it = assignedChannels.find(id);
		if (it != assignedChannels.end())
		{
			assignedChannels.erase(it);
		}
	}
}
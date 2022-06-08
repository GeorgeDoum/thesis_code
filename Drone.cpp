#include "Drone.h"

Drone::Drone(int x, int y, SDL_Renderer* renderer)
{
	setX(x);
	setY(y);
	publishPosition(getX(), getY(), renderer);
}

void Drone::setX(int x)
{
	x_axis = x;
}

int Drone::getX()
{
	return x_axis;
}

void Drone::setY(int y)
{
	y_axis = y;
}

int Drone::getY()
{
	return y_axis;
}

void Drone::setHexagonID(int id)
{
	hexagonID = id;
}

int Drone::getHexagonID()
{
	return hexagonID;
}

void Drone::publishPosition(int x , int y, SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	TTF_Init();
	TTF_Font* Sans = TTF_OpenFont("FreeSans.ttf", 24);
	if (Sans == NULL) {
		fprintf(stderr, "error: font not found\n");
		exit(EXIT_FAILURE);
	}
	SDL_Color colour = { 255, 0, 0 };
	std::string messageS = "X";
	char const* Char = messageS.c_str();

	SDL_Surface* SurfaceMessage =
		TTF_RenderText_Solid(Sans, Char, colour);
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, SurfaceMessage);

	SDL_Rect Message_rect;

	Message_rect.x = x;  //controls the rect's x coordinate 
	Message_rect.y = y; // controls the rect's y coordinte
	Message_rect.w = 10; // controls the width of the rect
	Message_rect.h = 5; // controls the height of the rect
	SDL_FreeSurface(SurfaceMessage);
	SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
	SDL_DestroyTexture(Message);
}

double Drone::calculatePathLoss(User& user)
{    
	                 /*    Optimal LAP Altitude for Maximum Coverage
          Akram Al-Hourani, Student Member, IEEE, Sithamparanathan Kandeepan, Senior Member, 
		                       IEEE, and Simon Lardner      
                IEEE WIRELESS COMMUNICATIONS LETTERS, VOL. 3, NO. 6, DECEMBER 2014  
											&&
				Unmanned Aerial Vehicle Applications over Cellular Networks for 5G and Beyond
								Hongliang Zhang Lingyang Song Zhu Han
											
								Elevation Angle-Based Model Pathloss Calculation
											*/


	double light_spd = 2.99792458e8; // The speed of light in vacuum in meters per second
	double pi = 3.1415926535897932385; // Pi
	int xOfUser = user.getX();
	int yOfUser = user.getY();
	int distX = abs(x_axis - xOfUser);
	int distY = abs(y_axis - yOfUser);
	int linkDistance = sqrt((distX ^ 2) + (distY ^ 2));
	//In this simulation we assume we are in an urban enviroment thus we define the following parameters based on the thesis references
	// (buildUpRatio,buildingsPerUnitArea,heightDestribution) Urban(0.5,500,50)
	//(nLos, nNlos) | Urban (1.0,20)
	double buildUpRatio = 0.5;
	int buildingsPerUnitArea = 500;
	int a = buildUpRatio * buildingsPerUnitArea;
	int b = 50;
	double nLos = 1.0;
	double nNlos = 20;
	double freeSpacePathloss = (20 * log10(frequency)) + (20 * log10( (4 * pi) / light_spd) );

	double lineOfSightPL = freeSpacePathloss + (20 * log10(linkDistance)) + nLos;
	double nonLineOfSightPL = freeSpacePathloss + (20 * log10(linkDistance)) + nNlos;

	double angleOfElevation = height_Drone / linkDistance;
	double lineOfSightProbability = 1 / ( 1+ (a * exp(-b * (angleOfElevation-a) ) ) );
	double nonLineOfSightProbability = 1 - lineOfSightProbability;

	double pathloss = (lineOfSightPL * lineOfSightProbability) + (nonLineOfSightPL * nonLineOfSightProbability);

	return abs(pathloss);
}

double Drone::rayleighFadingChannel(double variance)
{
	std::default_random_engine eng;
	std::normal_distribution<double> distr(0, 1);
	double randomRealSide = distr(eng);

	std::default_random_engine eng1;
	std::normal_distribution<double> distr1(0, 1);
	double channelImaginarrySide = distr1(eng1);

	long double channelRealSide = sqrt(variance / 2) * randomRealSide;
	std::complex<double> complexNumberForChannel(channelRealSide, channelImaginarrySide);

	double absoluteofComplexNumber = std::abs(complexNumberForChannel);
	double channel = absoluteofComplexNumber * absoluteofComplexNumber;

	return channel;
}

double Drone::ricianFadingChannel(double variance)
{
	int k = 4;
	double  channel = (k / (k + static_cast<double>(1))) + ((1 / (k + static_cast<double>(1))) * rayleighFadingChannel(variance));
	return channel;
}

std::vector<int> Drone::provideService(std::map<int, User> clusterUsers)
{
	int counter = 0;
	std::vector<int> usersIds;
	std::vector<User> users;

	for (auto& clUsr : clusterUsers)
	{
		users.push_back(clUsr.second);
	}

	for (auto& usr : users)
	{
		if (counter < 10)
		{
			double pathloss = calculatePathLoss(usr);
			double channel = ricianFadingChannel(pathloss);
			assignedChannels.insert(std::pair<int, double>(usr.getUniqueID(), channel));
			std::cout << channel <<std::endl;
			usersIds.push_back(usr.getUniqueID());
		}
		counter++;
	}

	return usersIds;
}


/*  FROM PAPER  OPTIMAL_LAP_ALTITUDE_FOR_MAXIMUM_COVERAGE    */
// a : Ratio of build up land area to the total land area
// b : representst the mean number of buildings per unit area (buildings/km^2)
// g : parameter that describes the build height destribution
// (a,b,g) / suburban(0.1,750,8) | Urban(0.5,500,50) | DenseUrban(0.5,300,50) | HighRise Urban(0.5,300,50)
// to ease the calculation the first S -curve parameter is a*b and the second S-curve parameter is g
// 
// 
// (nLos, nNlos) / suburban(0.1,21) | Urban (1.0,20) | Dense Urban(1.6 , 23) | HighRise Urban(2.3,34)
#include "BaseStation.h"

BaseStation::BaseStation(int x, int y, int hex_ID)
{
	setXaxis(x);
	setYaxis(y);
	setID(hex_ID);
	setPrs();
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

void BaseStation::setPrs()
{
	/*    Energy-Efficient and Interference-Aware Handover
          Decision for the LTE-Advanced Femtocell Network   
     IEEE ICC 2013 - Communication QoS, Reliability and Modeling Symposium   */

	std::random_device rd{};
	std::mt19937 gen{ rd() };
	std::normal_distribution<double> distr(23, 3);
	Prs = distr(gen);
}

double BaseStation::getPrs()
{
	return Prs;
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
	                 /*           Reference
	       International Journal of Computer Applications (0975 – 8887)
		                 Volume 59– No.11, December 2012
               Comparison of Okumura, Hata and COST-231 Models on
                   the Basis of Path Loss and Signal Strength
                                Yuvraj Singh  
	    https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.303.4057&rep=rep1&type=pdf  */

	int xOfUser = user.getX();
	int yOfUser = user.getY();
	int distX = abs(x_axis - xOfUser);
	int distY = abs(y_axis - yOfUser);
	int linkDistance = sqrt((distX ^ 2) + (distY ^ 2));
	int hR = user.getAntennaHeight();

	//in this simulator we assume we are in an urban area thus we calculate the mobile antenna correction as follows
	double aHR = (3.20 * (log10(11.75 * hR)) * 2) - 4.97;
	// C = 0 we are not simulating a metropolitan area

	double A = 46.3 + (33.9 * log10(frequency)) - (13.28 * log10(heightOfStationAntenna)) - aHR;
	double B = 44.9 - (6.55 * log10(heightOfStationAntenna));
	double pathloss = A + (B * log10(linkDistance));
	return pathloss;
}

double BaseStation::rayleigh_fading(long double variance)
{
	std::default_random_engine eng;
	std::normal_distribution<double> distr(0, 1);
	double randomRealSide = distr(eng);

	std::default_random_engine eng1;
	std::normal_distribution<double> distr1(0, 1);
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


double BaseStation::provideService(User& user, int temp)
{
	int userId = user.getUniqueID();
	double pathloss = calculatePathLoss(user);

	/*  - different temp, equals call of funtion from different algorithm .
	    - temp = 0 equals the pathloss from the initial BaseStation for each user
	    - temp = 1 equals the pathloss when we calculating the interference  */

	if (temp == 0)
	{
		user.setPathLoss(pathloss);
	}
	if (temp == 1)
	{
		user.setTemporaryPathLoss(pathloss);
	}
	double channel = rayleigh_fading(pathloss);

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
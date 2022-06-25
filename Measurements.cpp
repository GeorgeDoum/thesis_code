#include "Measurements.h"


double calculateInterference2(User& user, std::vector<BaseStation>& basestations, int currentStationID)
{
	double sum = 0;
	for (BaseStation& station : basestations)
	{
		if (station.getID() != currentStationID)
		{
			double Pr = (station.getPrs() * user.getChannel() * (-10)) / user.getTemporaryPathLoss(); //Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
			sum = sum + Pr;
		}
	}
	return sum;
}

void doMeasurements(std::vector<BaseStation>& basestations, std::vector<User>& users)
{
//to calculate the Sum Rate
double sumRate = 0.0;
for (auto& station : basestations)
{
	for (auto& usr : users)
	{
		if (usr.getStationId() == station.getID())
		{
			double Pr = (13 * usr.getChannel() * (-10)) / usr.getPathLoss();
			double I = calculateInterference2(usr, basestations, usr.getStationId());
			double SINR = (Pr / (4 + I));
			double rate = (5e+6 / station.getChannels().size()) * (log2(1 + abs(SINR)));
			sumRate = sumRate + rate;
		}
	}
	//std::cout << "SUM RATE " << sumRate << std::endl;
}

std::cout<< "BEFORE DRONES SUM RATE " << sumRate << std::endl;


//to calculate spectral efficiency
double spectral = 0.0;
for (auto& station : basestations)
{
	for (auto& usr : users)
	{
		if (usr.getStationId() == station.getID())
		{
			double Pr = (13 * usr.getChannel() * (-10)) / usr.getPathLoss();
			double I = calculateInterference2(usr, basestations, usr.getStationId());
			double SINR = (Pr / (4 + I));
			double rate = ((5e+6/station.getChannels().size())/ (5e+6 + 5e+6)) * (log2(1 + abs(SINR)));
			spectral = spectral + rate;
		}
	}
}

std::cout << "BEFORE DRONES SPECTRAL EFFICIENCY " << spectral << std::endl;
}
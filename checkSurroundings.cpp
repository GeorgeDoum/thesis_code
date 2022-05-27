#include "checkSurroundings.h"

/*		https://ieeexplore.ieee.org/document/6654902 
      Energy-Efficient and Interference-Aware Handover
      Decision for the LTE-Advanced Femtocell Network   
   IEEE ICC 2013 - Communication QoS, Reliability and Modeling Symposium  */

/*  https://mdpi-res.com/d_attachment/electronics/electronics-08-00796/article_deploy/electronics-08-00796.pdf?version=1563278598  */


/* Calculate Interference from the rest of the basestations of the system */
double calculateInterference(User& user, std::vector<BaseStation>& basestations, int currentStationID)
{
	double sum = 0;
	for (BaseStation& station : basestations)
	{
		if (station.getID() != currentStationID)
		{
			double channel = station.provideService(user , 1);
			double Pr = (station.getPrs() * channel * (-10)) / user.getTemporaryPathLoss(); //Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
			sum = sum + Pr;
		}
	}
	return sum;
}


void checkSurroundingBasestations(User& user, std::vector<BaseStation>& basestations, int i)
{
	std::cout << user.getUniqueID() << std::endl;
	std::cout << basestations.at(i).getID() << std::endl;
	int realStationId = i + 1;
	std::vector<BaseStation> stationsThatProvideServiceContinuety;
	std::map<int,double> RIPcontainer;
	std::map<int,double> RSRPcontainer;
	int nf = 4; //noise ratio
	int gt = 3; //Mean UL SINR target
	std::map<int,double> HOcriterionResults;
	std::map<int, double> candidateChannelsContainer;
	std::map<int, double> SINRcontainer;

	double Pr = (basestations.at(i).getPrs() * user.getChannel() *(-10)) / user.getPathLoss();//Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
	double I = calculateInterference(user, basestations, user.getStationId()); 
	double SINR = Pr / (nf + I);
	std::cout << "THE SINR IS => " << SINR << std::endl;
	SINRcontainer.insert({ basestations.at(i).getID(), SINR });
	double RSRP = basestations.at(i).getPrs() * user.getChannel();
	double RIP = (Pr * user.getChannel()) + nf;

	double RSRPth = -53.9794; //minimum RSRPth is 100dbm. 100dbm equals 3.33333333e-6 power(W). pow2db(3.33333333e-6) = -53.9794 dB
	double Pmax = -50; // https://mdpi-res.com/d_attachment/electronics/electronics-08-00796/article_deploy/electronics-08-00796.pdf?version=1563278598 Pmax = 18 dbm equals 0.000001 power (W) which equals -50 dB

	double sustainedServiceContinuety = (RSRPth + basestations.at(i).getPrs() - Pmax);

	if (RSRP > sustainedServiceContinuety)
	{
		std::cout << "YES" << std::endl;
		stationsThatProvideServiceContinuety.push_back(basestations.at(i));
		candidateChannelsContainer.insert({ basestations.at(i).getID(), user.getChannel() });
		RIPcontainer.insert( {basestations.at(i).getID(), RIP  });
		RSRPcontainer.insert({basestations.at(i).getID(), RSRP });
	}

	if (realStationId == 1)
	{
		std::vector<BaseStation> L { basestations.at(1), basestations.at(2), basestations.at(3) }; //candidate cells

		for (BaseStation& station : L)
		{
			double candidateChannel = station.provideService(user, 1);
			double candidateRSRP = station.getPrs() * candidateChannel;
			double candidateServiceContinuety = (RSRPth + station.getPrs() - Pmax);
			double candidatePr = (station.getPrs() * candidateChannel *(-10)) / user.getTemporaryPathLoss();//Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
			double interf = calculateInterference(user, basestations, station.getID());
			double candidateSINR = candidatePr / (nf + interf);
			double candidateRIP = (candidatePr * candidateChannel) + nf;
			if (candidateRSRP > candidateServiceContinuety)
			{
				stationsThatProvideServiceContinuety.push_back(station);
				RIPcontainer.insert( {station.getID(), candidateRIP  });
				RSRPcontainer.insert({station.getID(), candidateRSRP });
				candidateChannelsContainer.insert({ station.getID(), candidateChannel });
				SINRcontainer.insert({ station.getID(),candidateSINR });
			}
		}
		//if no other station provides service continuety rather than the initial BaseStation, no handover is done
		if (stationsThatProvideServiceContinuety.size() == 1)
		{
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(user.getStationId()) - 1).assignChannel(user.getChannel(), user.getUniqueID());
		}
		else
		{
			//evaluate handover hysteresis margins
			for (BaseStation& station : stationsThatProvideServiceContinuety)
			{
				auto itRIP = RIPcontainer.begin(); //RIPcontainer iterator
				auto itRSRP = RSRPcontainer.begin();//RSRPcontainer iterator
				double A = (gt * stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second) / RSRPcontainer.begin()->second;
				double B = itRSRP->second / station.getPrs();
				double C = stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second;

				double HHM = 10 * log10((station.getPrs() * (itRIP->second - (A * B))) / C);

				double HOcriterio = RSRPcontainer.begin()->second + HHM;
				if (station.getID() == basestations.at(i).getID())
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				else if (itRSRP->second > HOcriterio)
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				itRIP++;
				itRSRP++;
			}

			auto max = std::max_element(HOcriterionResults.begin(), HOcriterionResults.end(), [](const auto& x, const auto& y) {
				return x.second < y.second;
				});
			int id = max->first;
			double finalChannel;
			double finalSINR;
			for (auto it = candidateChannelsContainer.begin(); it != candidateChannelsContainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalChannel = it->second;
				}
			}
			for (auto it = SINRcontainer.begin(); it != SINRcontainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalSINR = it->second;
				}
			}
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(id) - 1).assignChannel(finalChannel, user.getUniqueID());
			user.setSINR(finalSINR);
		}
	}
	if (realStationId == 2)
	{
		std::vector<BaseStation> L { basestations.at(0), basestations.at(3), basestations.at(4) }; //candidate cells

		for (BaseStation& station : L)
		{
			double candidateChannel = station.provideService(user, 1);
			double candidateRSRP = station.getPrs() * candidateChannel;
			double candidateServiceContinuety = (RSRPth + station.getPrs() - Pmax);
			double candidatePr = (station.getPrs() * candidateChannel * (-10)) / user.getTemporaryPathLoss();//Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
			double interf = calculateInterference(user, basestations, station.getID());
			double candidateSINR = candidatePr / (nf + interf);
			double candidateRIP = (candidatePr * candidateChannel) + nf;
			if (candidateRSRP > candidateServiceContinuety)
			{
				stationsThatProvideServiceContinuety.push_back(station);
				RIPcontainer.insert({ station.getID(), candidateRIP });
				RSRPcontainer.insert({ station.getID(), candidateRSRP });
				candidateChannelsContainer.insert({ station.getID(), candidateChannel });
				SINRcontainer.insert({ station.getID(),candidateSINR });
			}
		}
		//if no other station provides service continuety rather than the initial BaseStation, no handover is done
		if (stationsThatProvideServiceContinuety.size() == 1)
		{
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(user.getStationId()) - 1).assignChannel(user.getChannel(), user.getUniqueID());
		}
		else
		{
			//evaluate handover hysteresis margins
			for (BaseStation& station : stationsThatProvideServiceContinuety)
			{
				auto itRIP = RIPcontainer.begin(); //RIPcontainer iterator
				auto itRSRP = RSRPcontainer.begin();//RSRPcontainer iterator
				double A = (gt * stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second) / RSRPcontainer.begin()->second;
				double B = itRSRP->second / station.getPrs();
				double C = stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second;

				double HHM = 10 * log10((station.getPrs() * (itRIP->second - (A * B))) / C);

				double HOcriterio = RSRPcontainer.begin()->second + HHM;
				if (station.getID() == basestations.at(i).getID())
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				else if (itRSRP->second > HOcriterio)
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				itRIP++;
				itRSRP++;
			}

			auto max = std::max_element(HOcriterionResults.begin(), HOcriterionResults.end(), [](const auto& x, const auto& y) {
				return x.second < y.second;
				});
			int id = max->first;
			double finalChannel;
			double finalSINR;
			for (auto it = candidateChannelsContainer.begin(); it != candidateChannelsContainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalChannel = it->second;
				}
			}
			for (auto it = SINRcontainer.begin(); it != SINRcontainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalSINR = it->second;
				}
			}
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(id) - 1).assignChannel(finalChannel, user.getUniqueID());
			user.setSINR(finalSINR);
		}

	}
	if (realStationId == 3)
	{
		std::vector<BaseStation> L { basestations.at(0), basestations.at(3), basestations.at(5) }; //candidate cells

		for (BaseStation& station : L)
		{
			double candidateChannel = station.provideService(user, 1);
			double candidateRSRP = station.getPrs() * candidateChannel;
			double candidateServiceContinuety = (RSRPth + station.getPrs() - Pmax);
			double candidatePr = (station.getPrs() * candidateChannel * (-10)) / user.getTemporaryPathLoss();//Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
			double interf = calculateInterference(user, basestations, station.getID());
			double candidateSINR = candidatePr / (nf + interf);
			double candidateRIP = (candidatePr * candidateChannel) + nf;
			if (candidateRSRP > candidateServiceContinuety)
			{
				stationsThatProvideServiceContinuety.push_back(station);
				RIPcontainer.insert({ station.getID(), candidateRIP });
				RSRPcontainer.insert({ station.getID(), candidateRSRP });
				candidateChannelsContainer.insert({ station.getID(), candidateChannel });
				SINRcontainer.insert({ station.getID(),candidateSINR });
			}
		}
		//if no other station provides service continuety rather than the initial BaseStation, no handover is done
		if (stationsThatProvideServiceContinuety.size() == 1)
		{
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(user.getStationId()) - 1).assignChannel(user.getChannel(), user.getUniqueID());
		}
		else
		{
			//evaluate handover hysteresis margins
			for (BaseStation& station : stationsThatProvideServiceContinuety)
			{
				auto itRIP = RIPcontainer.begin(); //RIPcontainer iterator
				auto itRSRP = RSRPcontainer.begin();//RSRPcontainer iterator
				double A = (gt * stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second) / RSRPcontainer.begin()->second;
				double B = itRSRP->second / station.getPrs();
				double C = stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second;

				double HHM = 10 * log10((station.getPrs() * (itRIP->second - (A * B))) / C);

				double HOcriterio = RSRPcontainer.begin()->second + HHM;
				if (station.getID() == basestations.at(i).getID())
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				else if (itRSRP->second > HOcriterio)
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				itRIP++;
				itRSRP++;
			}

			auto max = std::max_element(HOcriterionResults.begin(), HOcriterionResults.end(), [](const auto& x, const auto& y) {
				return x.second < y.second;
				});
			int id = max->first;
			double finalChannel;
			double finalSINR;
			for (auto it = candidateChannelsContainer.begin(); it != candidateChannelsContainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalChannel = it->second;
				}
			}
			for (auto it = SINRcontainer.begin(); it != SINRcontainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalSINR = it->second;
				}
			}
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(id) - 1).assignChannel(finalChannel, user.getUniqueID());
			user.setSINR(finalSINR);
		}
	}
	if (realStationId == 4)
	{
		std::vector<BaseStation> L { basestations.at(0), basestations.at(1), basestations.at(2),  basestations.at(4), basestations.at(5), basestations.at(6) }; //candidate cells

		for (BaseStation& station : L)
		{
			double candidateChannel = station.provideService(user, 1);
			double candidateRSRP = station.getPrs() * candidateChannel;
			double candidateServiceContinuety = (RSRPth + station.getPrs() - Pmax);
			double candidatePr = (station.getPrs() * candidateChannel *(-10)) / user.getTemporaryPathLoss();//Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
			double interf = calculateInterference(user, basestations, station.getID());
			double candidateSINR = candidatePr / (nf + interf);
			double candidateRIP = (candidatePr * candidateChannel) + nf;
			if (candidateRSRP > candidateServiceContinuety)
			{
				stationsThatProvideServiceContinuety.push_back(station);
				RIPcontainer.insert({ station.getID(), candidateRIP });
				RSRPcontainer.insert({ station.getID(), candidateRSRP });
				candidateChannelsContainer.insert({ station.getID(), candidateChannel });
				SINRcontainer.insert({ station.getID(),candidateSINR });
			}
		}
		//if no other station provides service continuety rather than the initial BaseStation, no handover is done
		if (stationsThatProvideServiceContinuety.size() == 1)
		{
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(user.getStationId()) - 1).assignChannel(user.getChannel(), user.getUniqueID());
		}
		else
		{
			//evaluate handover hysteresis margins
			for (BaseStation& station : stationsThatProvideServiceContinuety)
			{
				auto itRIP = RIPcontainer.begin(); //RIPcontainer iterator
				auto itRSRP = RSRPcontainer.begin();//RSRPcontainer iterator
				double A = (gt * stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second) / RSRPcontainer.begin()->second;
				double B = itRSRP->second / station.getPrs();
				double C = stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second;

				double HHM = 10 * log10((station.getPrs() * (itRIP->second - (A * B))) / C);

				double HOcriterio = RSRPcontainer.begin()->second + HHM;
				if (station.getID() == basestations.at(i).getID())
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				else if (itRSRP->second > HOcriterio)
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				itRIP++;
				itRSRP++;
			}

			auto max = std::max_element(HOcriterionResults.begin(), HOcriterionResults.end(), [](const auto& x, const auto& y) {
				return x.second < y.second;
				});
			int id = max->first;
			double finalChannel;
			double finalSINR;
			for (auto it = candidateChannelsContainer.begin(); it != candidateChannelsContainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalChannel = it->second;
				}
			}
			for (auto it = SINRcontainer.begin(); it != SINRcontainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalSINR = it->second;
				}
			}
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(id) - 1).assignChannel(finalChannel, user.getUniqueID());
			user.setSINR(finalSINR);
		}
	}
	if (realStationId == 5)
	{
		std::vector<BaseStation> L { basestations.at(1), basestations.at(3), basestations.at(6), basestations.at(7) }; //candidate cells

		for (BaseStation& station : L)
		{
			double candidateChannel = station.provideService(user, 1);
			double candidateRSRP = station.getPrs() * candidateChannel;
			double candidateServiceContinuety = (RSRPth + station.getPrs() - Pmax);
			double candidatePr = (station.getPrs() * candidateChannel * (-10)) / user.getTemporaryPathLoss();//Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
			double interf = calculateInterference(user, basestations, station.getID());
			double candidateSINR = candidatePr / (nf + interf);
			double candidateRIP = (candidatePr * candidateChannel) + nf;
			if (candidateRSRP > candidateServiceContinuety)
			{
				stationsThatProvideServiceContinuety.push_back(station);
				RIPcontainer.insert({ station.getID(), candidateRIP });
				RSRPcontainer.insert({ station.getID(), candidateRSRP });
				candidateChannelsContainer.insert({ station.getID(), candidateChannel });
				SINRcontainer.insert({ station.getID(),candidateSINR });
			}
		}
		//if no other station provides service continuety rather than the initial BaseStation, no handover is done
		if (stationsThatProvideServiceContinuety.size() == 1)
		{
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(user.getStationId()) - 1).assignChannel(user.getChannel(), user.getUniqueID());
		}
		else
		{
			//evaluate handover hysteresis margins
			for (BaseStation& station : stationsThatProvideServiceContinuety)
			{
				auto itRIP = RIPcontainer.begin(); //RIPcontainer iterator
				auto itRSRP = RSRPcontainer.begin();//RSRPcontainer iterator
				double A = (gt * stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second) / RSRPcontainer.begin()->second;
				double B = itRSRP->second / station.getPrs();
				double C = stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second;

				double HHM = 10 * log10((station.getPrs() * (itRIP->second - (A * B))) / C);

				double HOcriterio = RSRPcontainer.begin()->second + HHM;
				if (station.getID() == basestations.at(i).getID())
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				else if (itRSRP->second > HOcriterio)
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				itRIP++;
				itRSRP++;
			}

			auto max = std::max_element(HOcriterionResults.begin(), HOcriterionResults.end(), [](const auto& x, const auto& y) {
				return x.second < y.second;
				});
			int id = max->first;
			double finalChannel;
			double finalSINR;
			for (auto it = candidateChannelsContainer.begin(); it != candidateChannelsContainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalChannel = it->second;
				}
			}
			for (auto it = SINRcontainer.begin(); it != SINRcontainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalSINR = it->second;
				}
			}
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(id) - 1).assignChannel(finalChannel, user.getUniqueID());
			user.setSINR(finalSINR);
		}
	}
	if (realStationId == 6)
	{
		std::vector<BaseStation> L { basestations.at(2), basestations.at(3), basestations.at(6) , basestations.at(9) }; //candidate cells

		for (BaseStation& station : L)
		{
			double candidateChannel = station.provideService(user, 1);
			double candidateRSRP = station.getPrs() * candidateChannel;
			double candidateServiceContinuety = (RSRPth + station.getPrs() - Pmax);
			double candidatePr = (station.getPrs() * candidateChannel * (-10)) / user.getTemporaryPathLoss();//Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
			double interf = calculateInterference(user, basestations, station.getID());
			double candidateSINR = candidatePr / (nf + interf);
			double candidateRIP = (candidatePr * candidateChannel) + nf;
			if (candidateRSRP > candidateServiceContinuety)
			{
				stationsThatProvideServiceContinuety.push_back(station);
				RIPcontainer.insert({ station.getID(), candidateRIP });
				RSRPcontainer.insert({ station.getID(), candidateRSRP });
				candidateChannelsContainer.insert({ station.getID(), candidateChannel });
				SINRcontainer.insert({ station.getID(),candidateSINR });
			}
		}
		//if no other station provides service continuety rather than the initial BaseStation, no handover is done
		if (stationsThatProvideServiceContinuety.size() == 1)
		{
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(user.getStationId()) - 1).assignChannel(user.getChannel(), user.getUniqueID());
		}
		else
		{
			//evaluate handover hysteresis margins
			for (BaseStation& station : stationsThatProvideServiceContinuety)
			{
				auto itRIP = RIPcontainer.begin(); //RIPcontainer iterator
				auto itRSRP = RSRPcontainer.begin();//RSRPcontainer iterator
				double A = (gt * stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second) / RSRPcontainer.begin()->second;
				double B = itRSRP->second / station.getPrs();
				double C = stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second;

				double HHM = 10 * log10((station.getPrs() * (itRIP->second - (A * B))) / C);

				double HOcriterio = RSRPcontainer.begin()->second + HHM;
				if (station.getID() == basestations.at(i).getID())
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				else if (itRSRP->second > HOcriterio)
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				itRIP++;
				itRSRP++;
			}

			auto max = std::max_element(HOcriterionResults.begin(), HOcriterionResults.end(), [](const auto& x, const auto& y) {
				return x.second < y.second;
				});
			int id = max->first;
			double finalChannel;
			double finalSINR;
			for (auto it = candidateChannelsContainer.begin(); it != candidateChannelsContainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalChannel = it->second;
				}
			}
			for (auto it = SINRcontainer.begin(); it != SINRcontainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalSINR = it->second;
				}
			}
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(id) - 1).assignChannel(finalChannel, user.getUniqueID());
			user.setSINR(finalSINR);
		}
	}
	if (realStationId == 7)
	{
		std::vector<BaseStation> L { basestations.at(3), basestations.at(4), basestations.at(5), basestations.at(7), basestations.at(8), basestations.at(9) }; //candidate cells

		for (BaseStation& station : L)
		{
			double candidateChannel = station.provideService(user, 1);
			double candidateRSRP = station.getPrs() * candidateChannel;
			double candidateServiceContinuety = (RSRPth + station.getPrs() - Pmax);
			double candidatePr = (station.getPrs() * candidateChannel * (-10)) / user.getTemporaryPathLoss();//Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
			double interf = calculateInterference(user, basestations, station.getID());
			double candidateSINR = candidatePr / (nf + interf);
			double candidateRIP = (candidatePr * candidateChannel) + nf;
			if (candidateRSRP > candidateServiceContinuety)
			{
				stationsThatProvideServiceContinuety.push_back(station);
				RIPcontainer.insert({ station.getID(), candidateRIP });
				RSRPcontainer.insert({ station.getID(), candidateRSRP });
				candidateChannelsContainer.insert({ station.getID(), candidateChannel });
				SINRcontainer.insert({ station.getID(),candidateSINR });
			}
		}
		//if no other station provides service continuety rather than the initial BaseStation, no handover is done
		if (stationsThatProvideServiceContinuety.size() == 1)
		{
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(user.getStationId()) - 1).assignChannel(user.getChannel(), user.getUniqueID());
		}
		else
		{
			//evaluate handover hysteresis margins
			for (BaseStation& station : stationsThatProvideServiceContinuety)
			{
				auto itRIP = RIPcontainer.begin(); //RIPcontainer iterator
				auto itRSRP = RSRPcontainer.begin();//RSRPcontainer iterator
				double A = (gt * stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second) / RSRPcontainer.begin()->second;
				double B = itRSRP->second / station.getPrs();
				double C = stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second;

				double HHM = 10 * log10((station.getPrs() * (itRIP->second - (A * B))) / C);

				double HOcriterio = RSRPcontainer.begin()->second + HHM;
				if (station.getID() == basestations.at(i).getID())
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				else if (itRSRP->second > HOcriterio)
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				itRIP++;
				itRSRP++;
			}

			auto max = std::max_element(HOcriterionResults.begin(), HOcriterionResults.end(), [](const auto& x, const auto& y) {
				return x.second < y.second;
				});
			int id = max->first;
			double finalChannel;
			double finalSINR;
			for (auto it = candidateChannelsContainer.begin(); it != candidateChannelsContainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalChannel = it->second;
				}
			}
			for (auto it = SINRcontainer.begin(); it != SINRcontainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalSINR = it->second;
				}
			}
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(id) - 1).assignChannel(finalChannel, user.getUniqueID());
			user.setSINR(finalSINR);
		}
	}
	if (realStationId == 8)
	{
		std::vector<BaseStation> L { basestations.at(4), basestations.at(6), basestations.at(8) }; //candidate cells

		for (BaseStation& station : L)
		{
			double candidateChannel = station.provideService(user, 1);
			double candidateRSRP = station.getPrs() * candidateChannel;
			double candidateServiceContinuety = (RSRPth + station.getPrs() - Pmax);
			double candidatePr = (station.getPrs() * candidateChannel * (-10)) / user.getTemporaryPathLoss();//Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
			double interf = calculateInterference(user, basestations, station.getID());
			double candidateSINR = candidatePr / (nf + interf);
			double candidateRIP = (candidatePr * candidateChannel) + nf;
			if (candidateRSRP > candidateServiceContinuety)
			{
				stationsThatProvideServiceContinuety.push_back(station);
				RIPcontainer.insert({ station.getID(), candidateRIP });
				RSRPcontainer.insert({ station.getID(), candidateRSRP });
				candidateChannelsContainer.insert({ station.getID(), candidateChannel });
				SINRcontainer.insert({ station.getID(),candidateSINR });
			}
		}
		//if no other station provides service continuety rather than the initial BaseStation, no handover is done
		if (stationsThatProvideServiceContinuety.size() == 1)
		{
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(user.getStationId()) - 1).assignChannel(user.getChannel(), user.getUniqueID());
		}
		else
		{
			//evaluate handover hysteresis margins
			for (BaseStation& station : stationsThatProvideServiceContinuety)
			{
				auto itRIP = RIPcontainer.begin(); //RIPcontainer iterator
				auto itRSRP = RSRPcontainer.begin();//RSRPcontainer iterator
				double A = (gt * stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second) / RSRPcontainer.begin()->second;
				double B = itRSRP->second / station.getPrs();
				double C = stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second;

				double HHM = 10 * log10((station.getPrs() * (itRIP->second - (A * B))) / C);

				double HOcriterio = RSRPcontainer.begin()->second + HHM;
				if (station.getID() == basestations.at(i).getID())
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				else if (itRSRP->second > HOcriterio)
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				itRIP++;
				itRSRP++;
			}

			auto max = std::max_element(HOcriterionResults.begin(), HOcriterionResults.end(), [](const auto& x, const auto& y) {
				return x.second < y.second;
				});
			int id = max->first;
			double finalChannel;
			double finalSINR;
			for (auto it = candidateChannelsContainer.begin(); it != candidateChannelsContainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalChannel = it->second;
				}
			}
			for (auto it = SINRcontainer.begin(); it != SINRcontainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalSINR = it->second;
				}
			}
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(id) - 1).assignChannel(finalChannel, user.getUniqueID());
			user.setSINR(finalSINR);
		}
	}
	if (realStationId == 9)
	{
		std::vector<BaseStation> L { basestations.at(6), basestations.at(7), basestations.at(9) }; //candidate cells

		for (BaseStation& station : L)
		{
			double candidateChannel = station.provideService(user, 1);
			double candidateRSRP = station.getPrs() * candidateChannel;
			double candidateServiceContinuety = (RSRPth + station.getPrs() - Pmax);
			double candidatePr = (station.getPrs() * candidateChannel * (-10)) / user.getTemporaryPathLoss();//Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
			double interf = calculateInterference(user, basestations, station.getID());
			double candidateSINR = candidatePr / (nf + interf);
			double candidateRIP = (candidatePr * candidateChannel) + nf;
			if (candidateRSRP > candidateServiceContinuety)
			{
				stationsThatProvideServiceContinuety.push_back(station);
				RIPcontainer.insert({ station.getID(), candidateRIP });
				RSRPcontainer.insert({ station.getID(), candidateRSRP });
				candidateChannelsContainer.insert({ station.getID(), candidateChannel });
				SINRcontainer.insert({ station.getID(),candidateSINR });
			}
		}
		//if no other station provides service continuety rather than the initial BaseStation, no handover is done
		if (stationsThatProvideServiceContinuety.size() == 1)
		{
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(user.getStationId()) - 1).assignChannel(user.getChannel(), user.getUniqueID());
		}
		else
		{
			//evaluate handover hysteresis margins
			for (BaseStation& station : stationsThatProvideServiceContinuety)
			{
				auto itRIP = RIPcontainer.begin(); //RIPcontainer iterator
				auto itRSRP = RSRPcontainer.begin();//RSRPcontainer iterator
				double A = (gt * stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second) / RSRPcontainer.begin()->second;
				double B = itRSRP->second / station.getPrs();
				double C = stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second;

				double HHM = 10 * log10((station.getPrs() * (itRIP->second - (A * B))) / C);

				double HOcriterio = RSRPcontainer.begin()->second + HHM;
				if (station.getID() == basestations.at(i).getID())
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				else if (itRSRP->second > HOcriterio)
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				itRIP++;
				itRSRP++;
			}

			auto max = std::max_element(HOcriterionResults.begin(), HOcriterionResults.end(), [](const auto& x, const auto& y) {
				return x.second < y.second;
				});
			int id = max->first;
			double finalChannel;
			double finalSINR;
			for (auto it = candidateChannelsContainer.begin(); it != candidateChannelsContainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalChannel = it->second;
				}
			}
			for (auto it = SINRcontainer.begin(); it != SINRcontainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalSINR = it->second;
				}
			}
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(id) - 1).assignChannel(finalChannel, user.getUniqueID());
			user.setSINR(finalSINR);
		}
	}
	if (realStationId == 10)
	{
		std::vector<BaseStation> L { basestations.at(5), basestations.at(6), basestations.at(8) }; //candidate cells

		for (BaseStation& station : L)
		{
			double candidateChannel = station.provideService(user, 1);
			double candidateRSRP = station.getPrs() * candidateChannel;
			double candidateServiceContinuety = (RSRPth + station.getPrs() - Pmax);
			double candidatePr = (station.getPrs() * candidateChannel * (-10)) / user.getTemporaryPathLoss();//Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
			double interf = calculateInterference(user, basestations, station.getID());
			double candidateSINR = candidatePr / (nf + interf);
			double candidateRIP = (candidatePr * candidateChannel) + nf;
			if (candidateRSRP > candidateServiceContinuety)
			{
				stationsThatProvideServiceContinuety.push_back(station);
				RIPcontainer.insert({ station.getID(), candidateRIP });
				RSRPcontainer.insert({ station.getID(), candidateRSRP });
				candidateChannelsContainer.insert({ station.getID(), candidateChannel });
				SINRcontainer.insert({ station.getID(),candidateSINR });
			}
		}
		//if no other station provides service continuety rather than the initial BaseStation, no handover is done
		if (stationsThatProvideServiceContinuety.size() == 1)
		{
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(user.getStationId()) - 1).assignChannel(user.getChannel(), user.getUniqueID());
		}
		else
		{
			//evaluate handover hysteresis margins
			for (BaseStation& station : stationsThatProvideServiceContinuety)
			{
				auto itRIP = RIPcontainer.begin(); //RIPcontainer iterator
				auto itRSRP = RSRPcontainer.begin();//RSRPcontainer iterator
				double A = (gt * stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second) / RSRPcontainer.begin()->second;
				double B = itRSRP->second / station.getPrs();
				double C = stationsThatProvideServiceContinuety.at(0).getPrs() * RIPcontainer.begin()->second;

				double HHM = 10 * log10((station.getPrs() * (itRIP->second - (A * B))) / C);

				double HOcriterio = RSRPcontainer.begin()->second + HHM;
				if (station.getID() == basestations.at(i).getID())
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				else if (itRSRP->second > HOcriterio)
				{
					HOcriterionResults.insert({ station.getID(), HOcriterio });
				}
				itRIP++;
				itRSRP++;
			}

			auto max = std::max_element(HOcriterionResults.begin(), HOcriterionResults.end(), [](const auto& x, const auto& y) {
				return x.second < y.second;
				});
			int id = max->first;
			double finalChannel;
			double finalSINR;
			for (auto it = candidateChannelsContainer.begin(); it != candidateChannelsContainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalChannel = it->second;
				}
			}
			for (auto it = SINRcontainer.begin(); it != SINRcontainer.end(); ++it)
			{
				if (it->first == id)
				{
					finalSINR = it->second;
				}
			}
			basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(id) - 1).assignChannel(finalChannel, user.getUniqueID());
			user.setSINR(finalSINR);
		}
	}


/*
	if (realStationId == 1)
	{
		int baseStationID;
		double userChannel = user.getChannel();
		double channelFrom2 = basestations.at(1).provideService(user);
		double channelFrom3 = basestations.at(2).provideService(user);
		double channelFrom4 = basestations.at(3).provideService(user);
		std::cout << "From the current station " << user.getChannel() << std::endl;
		std::cout << "From station 2 " << channelFrom2 << std::endl;
		std::cout << "From station 3 " << channelFrom3 << std::endl;
		std::cout << "From station 4 " << channelFrom4 << std::endl;

		double max1 = std::max(userChannel, channelFrom2);
		double max2 = std::max(channelFrom3, channelFrom4);
		double bestChannel = std::max(max1, max2);

		if (bestChannel == userChannel)
			baseStationID = user.getStationId();
		else if(bestChannel == channelFrom2)
			baseStationID = basestations.at(1).getID();
		else if(bestChannel == channelFrom3)
			baseStationID = basestations.at(2).getID();
		else
			baseStationID = basestations.at(3).getID();
		user.setChannel(bestChannel);
		basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(baseStationID) - 1).assignChannel(bestChannel, user.getUniqueID());
		std::cout << "max " << bestChannel << std::endl;
		std::cout << "station id of max " << baseStationID << std::endl;
		std::cout << "temp" << std::endl;
	}

	if (realStationId == 2)
	{
		int baseStationID;
		double userChannel = user.getChannel();
		double channelFrom1 = basestations.at(0).provideService(user);
		double channelFrom4 = basestations.at(3).provideService(user);
		double channelFrom5 = basestations.at(4).provideService(user);
		std::cout << "From the current station " << user.getChannel() << std::endl;
		std::cout << "From station 1 " << channelFrom1 << std::endl;
		std::cout << "From station 4 " << channelFrom4 << std::endl;
		std::cout << "From station 5 " << channelFrom5 << std::endl;

		double max1 = std::max(userChannel, channelFrom1);
		double max2 = std::max(channelFrom4, channelFrom5);
		double bestChannel = std::max(max1, max2);

		if (bestChannel == userChannel)
			baseStationID = user.getStationId();
		else if (bestChannel == channelFrom1)
			baseStationID = basestations.at(0).getID();
		else if (bestChannel == channelFrom4)
			baseStationID = basestations.at(3).getID();
		else
			baseStationID = basestations.at(4).getID();
		user.setChannel(bestChannel);
		basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(baseStationID) - 1).assignChannel(bestChannel, user.getUniqueID());
		std::cout << "max " << bestChannel << std::endl;
		std::cout << "station id of max " << baseStationID << std::endl;
		std::cout << "temp" << std::endl;
	}

	if (realStationId == 3)
	{
		int baseStationID;
		double userChannel = user.getChannel();
		double channelFrom1 = basestations.at(0).provideService(user);
		double channelFrom4 = basestations.at(3).provideService(user);
		double channelFrom6 = basestations.at(5).provideService(user);
		std::cout << "From the current station " << user.getChannel() << std::endl;
		std::cout << "From station 1 " << channelFrom1 << std::endl;
		std::cout << "From station 4 " << channelFrom4 << std::endl;
		std::cout << "From station 6 " << channelFrom6 << std::endl;

		double max1 = std::max(userChannel, channelFrom1);
		double max2 = std::max(channelFrom4, channelFrom6);
		double bestChannel = std::max(max1, max2);

		if (bestChannel == userChannel)
			baseStationID = user.getStationId();
		else if (bestChannel == channelFrom1)
			baseStationID = basestations.at(0).getID();
		else if (bestChannel == channelFrom4)
			baseStationID = basestations.at(3).getID();
		else
			baseStationID = basestations.at(5).getID();
		user.setChannel(bestChannel);
		basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(baseStationID) - 1).assignChannel(bestChannel, user.getUniqueID());
		std::cout << "max " << bestChannel << std::endl;
		std::cout << "station id of max " << baseStationID << std::endl;
		std::cout << "temp" << std::endl;
	}
	
	if (realStationId == 4)
	{
		int baseStationID;
		double userChannel = user.getChannel();
		double channelFrom1 = basestations.at(0).provideService(user);
		double channelFrom2 = basestations.at(1).provideService(user);
		double channelFrom3 = basestations.at(2).provideService(user);
		double channelFrom5 = basestations.at(4).provideService(user);
		double channelFrom6 = basestations.at(5).provideService(user);
		double channelFrom7 = basestations.at(6).provideService(user);
		std::cout << "From the current station " << user.getChannel() << std::endl;
		std::cout << "From station 1 " << channelFrom1 << std::endl;
		std::cout << "From station 2 " << channelFrom2 << std::endl;
		std::cout << "From station 3 " << channelFrom3 << std::endl;
		std::cout << "From station 5 " << channelFrom5 << std::endl;
		std::cout << "From station 6 " << channelFrom6 << std::endl;
		std::cout << "From station 7 " << channelFrom7 << std::endl;

		double max1 = std::max(userChannel, channelFrom1);
		double max2 = std::max(channelFrom2, channelFrom3);
		double max3 = std::max(max1, max2);
		double max4 = std::max(max3, channelFrom5);
		double max5 = std::max(channelFrom6, channelFrom7);

		double bestChannel = std::max(max4, max5);

		if (bestChannel == userChannel)
			baseStationID = user.getStationId();
		else if (bestChannel == channelFrom1)
			baseStationID = basestations.at(0).getID();
		else if (bestChannel == channelFrom2)
			baseStationID = basestations.at(1).getID();
		else if (bestChannel == channelFrom3)
			baseStationID = basestations.at(2).getID();
		else if (bestChannel == channelFrom5)
			baseStationID = basestations.at(4).getID();
		else if (bestChannel == channelFrom6)
			baseStationID = basestations.at(5).getID();
		else
			baseStationID = basestations.at(6).getID();
		user.setChannel(bestChannel);
		basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(baseStationID) - 1).assignChannel(bestChannel, user.getUniqueID());
		std::cout << "max " << bestChannel << std::endl;
		std::cout << "station id of max " << baseStationID << std::endl;
		std::cout << "temp" << std::endl;
	}
	
	if (realStationId == 5)
	{
		int baseStationID;
		double userChannel = user.getChannel();
		double channelFrom2 = basestations.at(1).provideService(user);
		double channelFrom4 = basestations.at(3).provideService(user);
		double channelFrom7 = basestations.at(6).provideService(user);
		double channelFrom8 = basestations.at(7).provideService(user);
		std::cout << "From the current station " << user.getChannel() << std::endl;
		std::cout << "From station 2 " << channelFrom2 << std::endl;
		std::cout << "From station 4 " << channelFrom4 << std::endl;
		std::cout << "From station 7 " << channelFrom7 << std::endl;
		std::cout << "From station 8 " << channelFrom8 << std::endl;

		double max1 = std::max(userChannel, channelFrom2);
		double max2 = std::max(channelFrom4, channelFrom7);
		double max3 = std::max(max1, max2);
		double bestChannel = std::max(max3, channelFrom8);

		if (bestChannel == userChannel)
			baseStationID = user.getStationId();
		else if (bestChannel == channelFrom2)
			baseStationID = basestations.at(1).getID();
		else if (bestChannel == channelFrom4)
			baseStationID = basestations.at(3).getID();
		else if (bestChannel == channelFrom7)
			baseStationID = basestations.at(6).getID();
		else
			baseStationID = basestations.at(7).getID();
		user.setChannel(bestChannel);
		basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(baseStationID) - 1).assignChannel(bestChannel, user.getUniqueID());
		std::cout << "max " << bestChannel << std::endl;
		std::cout << "station id of max " << baseStationID << std::endl;
		std::cout << "temp" << std::endl;
	}
	
	if (realStationId == 6)
	{
		int baseStationID;
		double userChannel = user.getChannel();
		double channelFrom3 = basestations.at(2).provideService(user);
		double channelFrom4 = basestations.at(3).provideService(user);
		double channelFrom7 = basestations.at(6).provideService(user);
		double channelFrom10 = basestations.at(9).provideService(user);
		std::cout << "From the current station " << user.getChannel() << std::endl;
		std::cout << "From station 3 " << channelFrom3 << std::endl;
		std::cout << "From station 4 " << channelFrom4 << std::endl;
		std::cout << "From station 7 " << channelFrom7 << std::endl;
		std::cout << "From station 10 " << channelFrom10 << std::endl;

		double max1 = std::max(userChannel, channelFrom3);
		double max2 = std::max(channelFrom4, channelFrom7);
		double max3 = std::max(max1, max2);
		double bestChannel = std::max(max3, channelFrom10);

		if (bestChannel == userChannel)
			baseStationID = user.getStationId();
		else if (bestChannel == channelFrom3)
			baseStationID = basestations.at(2).getID();
		else if (bestChannel == channelFrom4)
			baseStationID = basestations.at(3).getID();
		else if (bestChannel == channelFrom7)
			baseStationID = basestations.at(6).getID();
		else
			baseStationID = basestations.at(9).getID();
		user.setChannel(bestChannel);
		basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(baseStationID) - 1).assignChannel(bestChannel, user.getUniqueID());
		std::cout << "max " << bestChannel << std::endl;
		std::cout << "station id of max " << baseStationID << std::endl;
		std::cout << "temp" << std::endl;
	}
	
	if (realStationId == 7)
	{
		int baseStationID;
		double userChannel = user.getChannel();
		double channelFrom4 = basestations.at(3).provideService(user);
		double channelFrom5 = basestations.at(4).provideService(user);
		double channelFrom6 = basestations.at(5).provideService(user);
		double channelFrom8 = basestations.at(7).provideService(user);
		double channelFrom9 = basestations.at(8).provideService(user);
		double channelFrom10 = basestations.at(9).provideService(user);
		std::cout << "From the current station " << user.getChannel() << std::endl;
		std::cout << "From station 4 " << channelFrom4 << std::endl;
		std::cout << "From station 5 " << channelFrom5 << std::endl;
		std::cout << "From station 6 " << channelFrom6 << std::endl;
		std::cout << "From station 8 " << channelFrom8 << std::endl;
		std::cout << "From station 9 " << channelFrom9 << std::endl;
		std::cout << "From station 10 " << channelFrom10 << std::endl;

		double max1 = std::max(userChannel, channelFrom4);
		double max2 = std::max(channelFrom5, channelFrom6);
		double max3 = std::max(max1, max2);
		double max4 = std::max(max3, channelFrom8);
		double max5 = std::max(channelFrom9, channelFrom10);

		double bestChannel = std::max(max4, max5);

		if (bestChannel == userChannel)
			baseStationID = user.getStationId();
		else if (bestChannel == channelFrom4)
			baseStationID = basestations.at(3).getID();
		else if (bestChannel == channelFrom5)
			baseStationID = basestations.at(4).getID();
		else if (bestChannel == channelFrom6)
			baseStationID = basestations.at(5).getID();
		else if (bestChannel == channelFrom8)
			baseStationID = basestations.at(7).getID();
		else if (bestChannel == channelFrom9)
			baseStationID = basestations.at(8).getID();
		else
			baseStationID = basestations.at(9).getID();
		user.setChannel(bestChannel);
		basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(baseStationID) - 1).assignChannel(bestChannel, user.getUniqueID());
		std::cout << "max " << bestChannel << std::endl;
		std::cout << "station id of max " << baseStationID << std::endl;
		std::cout << "temp" << std::endl;
	}
	
	if (realStationId == 8)
	{
		int baseStationID;
		double userChannel = user.getChannel();
		double channelFrom5 = basestations.at(4).provideService(user);
		double channelFrom7 = basestations.at(6).provideService(user);
		double channelFrom9 = basestations.at(8).provideService(user);
		std::cout << "From the current station " << user.getChannel() << std::endl;
		std::cout << "From station 5 " << channelFrom5 << std::endl;
		std::cout << "From station 7 " << channelFrom7 << std::endl;
		std::cout << "From station 9 " << channelFrom9 << std::endl;

		double max1 = std::max(userChannel, channelFrom5);
		double max2 = std::max(channelFrom7, channelFrom9);
		double bestChannel = std::max(max1, max2);

		if (bestChannel == userChannel)
			baseStationID = user.getStationId();
		else if (bestChannel == channelFrom5)
			baseStationID = basestations.at(4).getID();
		else if (bestChannel == channelFrom7)
			baseStationID = basestations.at(6).getID();
		else
			baseStationID = basestations.at(8).getID();
		user.setChannel(bestChannel);
		basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(baseStationID) - 1).assignChannel(bestChannel, user.getUniqueID());
		std::cout << "max " << bestChannel << std::endl;
		std::cout << "station id of max " << baseStationID << std::endl;
		std::cout << "temp" << std::endl;
	}
	
	if (realStationId == 9)
	{
		int baseStationID;
		double userChannel = user.getChannel();
		double channelFrom7 = basestations.at(6).provideService(user);
		double channelFrom8 = basestations.at(7).provideService(user);
		double channelFrom10 = basestations.at(9).provideService(user);
		std::cout << "From the current station " << user.getChannel() << std::endl;
		std::cout << "From station 7 " << channelFrom7 << std::endl;
		std::cout << "From station 8 " << channelFrom8 << std::endl;
		std::cout << "From station 10 " << channelFrom10 << std::endl;

		double max1 = std::max(userChannel, channelFrom7);
		double max2 = std::max(channelFrom8, channelFrom10);
		double bestChannel = std::max(max1, max2);

		if (bestChannel == userChannel)
			baseStationID = user.getStationId();
		else if (bestChannel == channelFrom7)
			baseStationID = basestations.at(6).getID();
		else if (bestChannel == channelFrom8)
			baseStationID = basestations.at(7).getID();
		else
			baseStationID = basestations.at(9).getID();
		user.setChannel(bestChannel);
		basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(baseStationID) - 1).assignChannel(bestChannel, user.getUniqueID());
		std::cout << "max " << bestChannel << std::endl;
		std::cout << "station id of max " << baseStationID << std::endl;
		std::cout << "temp" << std::endl;
	}

	if (realStationId == 10)
	{
		int baseStationID;
		double userChannel = user.getChannel();
		double channelFrom6 = basestations.at(5).provideService(user);
		double channelFrom7 = basestations.at(6).provideService(user);
		double channelFrom9 = basestations.at(8).provideService(user);
		std::cout << "From the current station " << user.getChannel() << std::endl;
		std::cout << "From station 6 " << channelFrom6 << std::endl;
		std::cout << "From station 7 " << channelFrom7 << std::endl;
		std::cout << "From station 9 " << channelFrom9 << std::endl;

		double max1 = std::max(userChannel, channelFrom6);
		double max2 = std::max(channelFrom7, channelFrom9);
		double bestChannel = std::max(max1, max2);

		if (bestChannel == userChannel)
			baseStationID = user.getStationId();
		else if (bestChannel == channelFrom6)
			baseStationID = basestations.at(5).getID();
		else if (bestChannel == channelFrom7)
			baseStationID = basestations.at(6).getID();
		else
			baseStationID = basestations.at(8).getID();
		user.setChannel(bestChannel);
		basestations.at(static_cast<std::vector<BaseStation, std::allocator<BaseStation>>::size_type>(baseStationID) - 1).assignChannel(bestChannel, user.getUniqueID());
		std::cout << "max " << bestChannel << std::endl;
		std::cout << "station id of max " << baseStationID << std::endl;
		std::cout << "temp" << std::endl;
	}
	*/
}

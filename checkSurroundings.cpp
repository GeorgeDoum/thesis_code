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

}
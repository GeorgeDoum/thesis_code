#include "Optimal_points.h"

std::vector<Drone> calculateOptimalPoints(std::vector<BaseStation>& basestations, std::vector<User>& users, SDL_Renderer* renderer)
{
	std::vector<Drone> drones;
	for(auto& station : basestations)
	{
		int numberOfUsersPerHexagon = 0;
		for (auto& usr : users)
		{
			if (station.getID() == usr.getStationId())
				numberOfUsersPerHexagon++;
		}

		if (numberOfUsersPerHexagon >= 20)
		{

			int stationId = station.getID();
			std::vector<int> xCoordinateOfUsers;
			std::vector<int> yCoordinateOfUsers;
			std::vector<int> usersUniqueIDs;
			int stationY = station.getY();

			for (auto& user : users)
			{
				if (stationId == user.getStationId())
				{
					xCoordinateOfUsers.push_back(user.getX());
					yCoordinateOfUsers.push_back(user.getY());
					usersUniqueIDs.push_back(user.getUniqueID());
				}
			}

			std::vector<int> xUpperSideOfHexagon;
			std::vector<int> yUpperSideOfHexagon;
			std::vector<int> xLowerSideOfHexagon;
			std::vector<int> yLowerSideOfHexagon;
			std::vector<int> upperUsersUniqueIDs;
			std::vector<int> lowerUsersUniqueIDs;
			int counter = 0;

			for (auto& temp : yCoordinateOfUsers)
			{
				if (temp >= stationY)
				{
					xUpperSideOfHexagon.push_back(xCoordinateOfUsers.at(counter));
					upperUsersUniqueIDs.push_back(usersUniqueIDs.at(counter));
					yUpperSideOfHexagon.push_back(temp);
				}
				else
				{
					xLowerSideOfHexagon.push_back(xCoordinateOfUsers.at(counter));
					lowerUsersUniqueIDs.push_back(usersUniqueIDs.at(counter));
					yLowerSideOfHexagon.push_back(temp);
				}
				counter++;
			}
			std::vector<User> drone1Users;
			std::vector<User> drone2Users;

			for (auto& usrT : users)
			{
				if (usrT.getStationId() == stationId)
				{
					for (auto& us1 : upperUsersUniqueIDs)
					{
						if (usrT.getUniqueID() == us1)
							drone1Users.push_back(usrT);
					}

					for (auto& us2 : lowerUsersUniqueIDs)
					{
						if (usrT.getUniqueID() == us2)
							drone2Users.push_back(usrT);
					}
				}		
			}

			int drone1X = (std::accumulate(xUpperSideOfHexagon.begin(), xUpperSideOfHexagon.end(), 0) ) /xUpperSideOfHexagon.size();
			int drone1Y = (std::accumulate(yUpperSideOfHexagon.begin(), yUpperSideOfHexagon.end(), 0)) / yUpperSideOfHexagon.size();

			int drone2X = (std::accumulate(xLowerSideOfHexagon.begin(), xLowerSideOfHexagon.end(), 0)) / xLowerSideOfHexagon.size();
			int drone2Y = (std::accumulate(yLowerSideOfHexagon.begin(), yLowerSideOfHexagon.end(), 0)) / yLowerSideOfHexagon.size();

			Drone drone1(drone1X, drone1Y, renderer, stationId);
			Drone drone2(drone2X, drone2Y, renderer, stationId);
			std::vector<int> drone1usersUniqueIDs = drone1.provideService(drone1Users);
			std::vector<int> drone2usersUniqueIDs = drone2.provideService(drone2Users);

			for (auto& st : basestations)
			{
				st.eraseChannels(drone1usersUniqueIDs);
				st.eraseChannels(drone2usersUniqueIDs);
			}

			drones.push_back(drone1);
			drones.push_back(drone2);
		}
	}
	return drones;
}
#include "Optimal_points.h"

struct Cluster {
	int initialMendoidX, initialMendoidY;
	int clusterNum; 
	std::map<int,User> initialNearUsers;
	double minDist;

	Cluster() :
		initialMendoidX(0.0),
		initialMendoidY(0.0),
		clusterNum(-1),
		minDist(std::numeric_limits<double>::max()) {}

	Cluster(int x, int y, int k) :
		initialMendoidX(x),
		initialMendoidY(y),
		clusterNum(k),
		minDist(std::numeric_limits<double>::max()) {}

	void addInitialUserToCluster(User& user)
	{
		initialNearUsers.insert({ user.getUniqueID(),user });
	}
	std::map<int,User> getClusterUsers()
	{
		return initialNearUsers;
	}
	double distance(Cluster c) {
		return (c.initialMendoidX - initialMendoidX) * (c.initialMendoidX - initialMendoidX) + (c.initialMendoidY - initialMendoidY) * (c.initialMendoidY - initialMendoidY);
	}

};


int findCloser(User& user, std::vector<Cluster>& clusters)
{
	std::map<int, double> distances;
	
	for (Cluster& clr : clusters)
	{
		int distX = abs(clr.initialMendoidX - user.getX());
		int distY = abs(clr.initialMendoidY - user.getY());
		double dist = sqrt((distX ^ 2) + (distY ^ 2));
		distances.insert({ clr.clusterNum ,  dist});
	}

	auto it = std::min_element(distances.begin(), distances.end(),
		[](const auto& l, const auto& r) { return l.second < r.second; });
	int clusterid = it->first;
	return clusterid;
}

void reallocateUserToCluster(User& user, std::vector<Cluster>& clusters, int idToGO)
{
	for (Cluster& c : clusters)
	{
		std::map<int, User> temp = c.getClusterUsers();
		std::map<int, User>::iterator it = temp.find(user.getUniqueID());
		if (it != temp.end())
		{
			c.getClusterUsers().erase(it->first);
		}
	}

	for (Cluster& clust : clusters)
	{
		if (clust.clusterNum == idToGO)
		{
			clust.addInitialUserToCluster(user);
		}
	}
}

std::vector<Drone> calculateOptimalPoints(std::vector<BaseStation>& basestations, std::vector<User>& users, SDL_Renderer* renderer)
{
	/*  https://www.geeksforgeeks.org/ml-k-medoids-clustering-with-example/?fbclid=IwAR2_onM03shuKGWYKgvG7awWo5TA4Ma25QQd8BQtrEzagNiJnsjLR_u-95w
	*											&&						
				An Unsupervised Machine Learning Approach for UAV-Aided Offloading of 5G Cellular Networks
						Lefteris Tsipi , Michail Karavolos and Demosthenes Vouyioukas
			Telecom 2022, 3, 86–102. https://doi.org/10.3390/telecom3010005 | https://www.mdpi.com/journal/telecom    */
	std::cout << "UDP ALGORITHM" << std::endl;
	std::vector<Drone> drones;
	std::vector<Cluster> clusters;

	int k = ceil(users.size() / static_cast<double>(10));
	std::cout << "out of  " << users.size() << " and 10 users per drone we have K  =  " << k << std::endl;

	/* Randomly choose initial mendoids for each cluster that we created*/
	for (int i=0; i<k; i++)
	{
		int x = rand() % 440 +420;
		int y = rand() % 487 + 150;
		SDL_SetRenderDrawColor(renderer, 250, 10,205, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(renderer, x, y);
		Cluster s = Cluster(x, y, i + 1);
		clusters.emplace_back(s);
	}
	//find the closest medoid to each user as an initial calculation
	for (User& usr : users)
	{
		int clusterid = findCloser(usr, clusters);
		clusters.at(clusterid - 1).addInitialUserToCluster(usr);
	}

	//calculate the initial within-cluser disimilarity & cost (A[1])
	std::vector<int> calculatedDissims;
	int A1 = 0;
	int A0 = 0;
	for (User& user : users)
	{
		std::map<int,int> dissimilarities;

		for (Cluster& c : clusters)
		{
			int dissiX = abs(user.getX() - c.initialMendoidX);
			int dissiY = abs(user.getY() - c.initialMendoidY);
			int totalDissim = dissiX + dissiY;
			dissimilarities.insert({ c.clusterNum,totalDissim });
		}

		auto it = std::min_element(dissimilarities.begin(), dissimilarities.end(),
			[](const auto& l, const auto& r) { return l.second < r.second; });

		reallocateUserToCluster(user, clusters, it->first);
		calculatedDissims.push_back(it->second);
	}
	for (auto& d : calculatedDissims)
	{
		A1 += d;
	}

	do
	{
		A0 = A1;
		for (Cluster& clr : clusters)
		{
			for (User& usr : users)
			{

			}
		}
		std::cout << "hello" <<std::endl;
	} while (A0 - A1 < 0.000001);

	return drones;










	/*
	std::cout << "KDP ALGORITHM" << std::endl;
	*/
	/*
	std::cout << "DRONE PLACEMENT GEORGE" << std::endl;

	std::vector<Drone> drones;
	for (auto& station : basestations)
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

				int drone1X = (std::accumulate(xUpperSideOfHexagon.begin(), xUpperSideOfHexagon.end(), 0)) / xUpperSideOfHexagon.size();
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

	}*/

}
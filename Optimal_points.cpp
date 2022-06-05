#include "Optimal_points.h"

struct Cluster {
	int mendoidX, mendoidY;
	int oldX, oldY;
	int clusterNum; 
	std::map<int, User> initialNearUsers;
	std::map<int, User> currentNearUsers;
	double minDist;

	Cluster() :
		mendoidX(0.0),
		mendoidY(0.0),
		oldX(0.0),
		oldY(0.0),
		clusterNum(-1),
		minDist(std::numeric_limits<double>::max()) {}

	Cluster(int x, int y, int k) :
		mendoidX(x),
		mendoidY(y),
		oldX(0.0),
		oldY(0.0),
		clusterNum(k),
		minDist(std::numeric_limits<double>::max()) {}

	void addInitialUserToCluster(User& user)
	{
		initialNearUsers.insert({ user.getUniqueID(),user });
	}

	void setX(int x)
	{
		mendoidX = x;
	}

	void setY(int y)
	{
		mendoidY = y;
	}

	void setOldX(int x)
	{
		oldX = x;
	}

	void setOldY(int y)
	{
		oldY = y;
	}

	void addCurrentUserToCluster(User& user)
	{
		currentNearUsers.insert({ user.getUniqueID(),user });
	}
	std::map<int, User> getInitialClusterUsers()
	{
		return initialNearUsers;
	}

	std::map<int, User> getCurrentClusterUsers()
	{
		return currentNearUsers;
	}

	double distance(Cluster c) {
		return (c.mendoidX - mendoidX) * (c.mendoidX - mendoidX) + (c.mendoidY - mendoidY) * (c.mendoidY - mendoidY);
	}

};


int findCloser(User& user, std::vector<Cluster>& clusters)
{
	std::map<int, double> distances;
	
	for (Cluster& clr : clusters)
	{
		int distX = abs(clr.mendoidX - user.getX());
		int distY = abs(clr.mendoidY - user.getY());
		double dist = sqrt((distX ^ 2) + (distY ^ 2));
		distances.insert({ clr.clusterNum ,  dist});
	}

	auto it = std::min_element(distances.begin(), distances.end(),
		[](const auto& l, const auto& r) { return l.second < r.second; });
	int clusterid = it->first;
	return clusterid;
}

std::map<int,double> findCloserBaseStation(Cluster& cluster, std::vector<BaseStation>& basestations)
{
	std::map<int, double> distances;
	std::map<int, double> ret;
	for (BaseStation& station : basestations)
	{
		int distX = abs(station.getX() - cluster.mendoidX);
		int distY = abs(station.getY() - cluster.mendoidY);
		double dist = sqrt((distX ^ 2) + (distY ^ 2));
		distances.insert({ station.getID() ,  dist});
	}
	auto it = std::min_element(distances.begin(), distances.end(),
		[](const auto& l, const auto& r) { return l.second < r.second; });

	ret.insert({ it->first, it->second });
	return ret;
}

void reallocateUserToCluster(User& user, std::vector<Cluster>& clusters, int idToGO, int choice)
{
	switch (choice)
	{
	case 0:
		for (Cluster& c : clusters)
		{
			std::map<int, User> temp = c.getInitialClusterUsers();
			std::map<int, User>::iterator it = temp.find(user.getUniqueID());
			if (it != temp.end())
			{
				c.getInitialClusterUsers().erase(it->first);
			}
		}

		for (Cluster& clust : clusters)
		{
			if (clust.clusterNum == idToGO)
			{
				clust.addInitialUserToCluster(user);
			}
		}

		break;
	case 1:
		for (Cluster& c : clusters)
		{
			std::map<int, User> temp = c.getCurrentClusterUsers();
			std::map<int, User>::iterator it = temp.find(user.getUniqueID());
			if (it != temp.end())
			{
				c.getCurrentClusterUsers().erase(it->first);
				c.getInitialClusterUsers().erase(it->first);
			}
		}

		for (Cluster& clust : clusters)
		{
			if (clust.clusterNum == idToGO)
			{
				clust.addCurrentUserToCluster(user);
			}
		}
		break;
	}
}

bool cmp(std::pair<int, double>& a,
	std::pair<int, double>& b)
{
	return a.second > b.second;
}

std::vector<std::pair<int,double>> sort(std::map<int, double>&M)
{

	// Declare vector of pairs
	std::vector<std::pair<int, double> > A;

	// Copy key-value pair from Map
	// to vector of pairs
	for (auto& it : M) {
		A.push_back(it);
	}

	// Sort using comparator function
	sort(A.begin(), A.end(), cmp);

	// Print the sorted value
	for (auto& it : A) {

		std::cout << it.first << ' '
			<< it.second << std::endl;
	}
	return A;
}

bool checkIfAlreadyIntoCluster(User& user, std::vector<Cluster>& clusters)
{
	for (auto& cl : clusters)
	{
		std::map<int, User>::iterator it = cl.initialNearUsers.find(user.getUniqueID());
		std::map<int, User>::iterator it1 = cl.currentNearUsers.find(user.getUniqueID());
		if ((it != cl.initialNearUsers.end()) && (it1 != cl.currentNearUsers.end()))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

std::vector<Drone> calculateOptimalPoints(std::vector<BaseStation>& basestations, std::vector<User>& users, SDL_Renderer* renderer)
{
	/*  https://www.geeksforgeeks.org/ml-k-medoids-clustering-with-example/?fbclid=IwAR2_onM03shuKGWYKgvG7awWo5TA4Ma25QQd8BQtrEzagNiJnsjLR_u-95w
	*											&&						
				An Unsupervised Machine Learning Approach for UAV-Aided Offloading of 5G Cellular Networks
						Lefteris Tsipi , Michail Karavolos and Demosthenes Vouyioukas
			Telecom 2022, 3, 86–102. https://doi.org/10.3390/telecom3010005 | https://www.mdpi.com/journal/telecom    

												*/

	std::cout << "UDP ALGORITHM" << std::endl;
	std::vector<Drone> drones;
	std::vector<Cluster> clusters;

	int k = ceil(users.size() / static_cast<double>(10));
	std::cout << "out of  " << users.size() << " and 10 users per drone we have K  =  " << k << std::endl;

	// Randomly choose initial mendoids for each cluster that we created
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
			int dissiX = abs(user.getX() - c.mendoidX);
			int dissiY = abs(user.getY() - c.mendoidY);
			int totalDissim = dissiX + dissiY;
			dissimilarities.insert({ c.clusterNum,totalDissim });
		}

		auto it = std::min_element(dissimilarities.begin(), dissimilarities.end(),
			[](const auto& l, const auto& r) { return l.second < r.second; });

		reallocateUserToCluster(user, clusters, it->first, 0);
		calculatedDissims.push_back(it->second);
	}
	for (auto& d : calculatedDissims)
	{
		A1 += d;
	}
	//USER CLUSTERING PROCESS
	do
	{
		A0 = A1;
		for (Cluster& clr : clusters)
		{
			int i = rand() % users.size();
			int Xbackup = clr.mendoidX;
			int Ybackup = clr.mendoidY;
			clr.setX(users.at(i).getX());
			clr.setY(users.at(i).getY());
			
			std::vector<int> calculatedDissims;
			for (User& usr : users)
			{
				std::map<int, int> dissimilarities;
				int clusterid = findCloser(usr, clusters);
				clusters.at(clusterid - 1).addCurrentUserToCluster(usr);

				for (Cluster& c : clusters)
				{
					int dissiX = abs(usr.getX() - c.mendoidX);
					int dissiY = abs(usr.getY() - c.mendoidY);
					int totalDissim = dissiX + dissiY;
					dissimilarities.insert({ c.clusterNum,totalDissim });
				}
				auto it = std::min_element(dissimilarities.begin(), dissimilarities.end(),
					[](const auto& l, const auto& r) { return l.second < r.second; });

				reallocateUserToCluster(usr, clusters, it->first, 1);
				calculatedDissims.push_back(it->second);
			}
			int B = 0;
			for (auto& d : calculatedDissims)
			{
				B += d;
			}
			if (B < A1)
			{
				A1 = B;
				for (Cluster& c : clusters)
				{
					c.initialNearUsers.clear();
					for (auto it = c.currentNearUsers.begin(); it != c.currentNearUsers.end(); ++it)
					{
						c.initialNearUsers.insert({ it->first, it->second });
					}
				}
			}
			else
			{
				clr.setX(Xbackup);
				clr.setY(Ybackup);
				for (Cluster& c : clusters)
				{
					c.currentNearUsers.clear();
				}
			}
		}
	} while (A0 - A1 > 0.000001);
	std::cout << "USER CLUSTERING PROCESS DONE" << std::endl;

	double a = 0.5; //weightening factor
	//calculate weightScores
	std::map<int, double> weightScores;
	for (Cluster& cl : clusters)
	{
		std::map<int,double> closerBSID = findCloserBaseStation(cl, basestations);
		double weight = 0.0;
		weight = (a * cl.getCurrentClusterUsers().size()) + ((1 - a) * closerBSID.begin()->second);
		weightScores.insert({ cl.clusterNum, weight });
	}
	//Sort in descending order and get the D = 10 with the biggest scores
	std::vector<std::pair<int,double>> sortedWeightScores = sort(weightScores);
	std::cout << std::endl;
	std::vector<int> idsOfFinalClusters;
	int i = 0;
	for (auto& scores : sortedWeightScores)
	{
		if (i < 10)
		{
			std::cout << scores.first << "   " << scores.second << std::endl;
			idsOfFinalClusters.push_back(scores.first);
		}
		i++;
	}
	//The optimal final clusters with the coordinates that the drones should be deployed & cover
	for (Cluster& cl : clusters)
	{
		for (auto& id : idsOfFinalClusters)
		{
			if (id == cl.clusterNum)
			{
				Drone drone(cl.mendoidX, cl.mendoidY, renderer);
				std::map<int,User> users = cl.initialNearUsers;
				std::vector<int> droneusersUniqueIDs = drone.provideService(users);

				for (auto& st : basestations)
				{
					st.eraseChannels(droneusersUniqueIDs);
				}
				drones.push_back(drone);
			}
		}
	}
		/*			
	*  THIS IS THE END OF UDP ALGORITHM
	* 
	*     BELOW IS THE K-MEAN MENDOID 
	         CLUSTERING ALGORITHM  
	*               */
	/*
	std::cout << "KDP ALGORITHM" << std::endl;
	std::vector<Drone> drones;
	std::vector<Cluster> clusters;

	int k = ceil(users.size() / static_cast<double>(10));
	std::cout << "out of  " << users.size() << " and 10 users per drone we have K  =  " << k << std::endl;

	// Randomly choose initial mendoids for each cluster that we created
	for (int i = 0; i < k; i++)
	{
		int x = rand() % 440 + 420;
		int y = rand() % 487 + 150;
		SDL_SetRenderDrawColor(renderer, 250, 10, 205, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(renderer, x, y);
		Cluster s = Cluster(x, y, i + 1);
		clusters.emplace_back(s);
	}

	bool convergence = false;
	do
	{
		int convergencecounter = 0;
		//find the closest medoid to each user as an initial calculation
		for (User& usr : users)
		{
			int clusterid = findCloser(usr, clusters);
			bool check = checkIfAlreadyIntoCluster(usr, clusters);
			if (check)
			{
				clusters.at(static_cast<std::vector<Cluster, std::allocator<Cluster>>::size_type>(clusterid) - 1).addInitialUserToCluster(usr);

			}
		}
		//find the mean of all assigned users and calculate  the new mendoids
		for (Cluster& clu : clusters)
		{
			double meanX = 0.0;
			double meanY = 0.0;
			int newX = 0;
			int newY = 0;
			for (auto& clusterUsr : clu.initialNearUsers)
			{
				meanX = meanX + clusterUsr.second.getX();
				meanY = meanY + clusterUsr.second.getY();
			}
			newX = meanX / clu.initialNearUsers.size();
			newY = meanY / clu.initialNearUsers.size();
			clu.setOldX(clu.mendoidX);
			clu.setOldY(clu.mendoidY);
			clu.setX(newX);
			clu.setY(newY);
		}
		//find the closest medoid to each user as an new calculation
		for (User& usr : users)
		{
			int clusterid = findCloser(usr, clusters);
			bool check = checkIfAlreadyIntoCluster(usr, clusters);
			if (check)
			{
				clusters.at(static_cast<std::vector<Cluster, std::allocator<Cluster>>::size_type>(clusterid) - 1).addCurrentUserToCluster(usr);
			}
		}
		//check if none cluster assignments change
		for (Cluster& clu : clusters)
		{
			if ((clu.oldX == clu.mendoidX) && (clu.oldY == clu.mendoidY))
			{
				convergencecounter++;
			}
		}
		//check if every cluster reached convergence. If true ,stop the calculations, the clusters are final
		if (convergencecounter == clusters.size())
		{
			convergence = true;
			for (Cluster& clu : clusters)
			{
				clu.initialNearUsers.clear();
			}
		}
		else
		{
			convergence = false;
			for (Cluster& clu : clusters)
			{
				clu.initialNearUsers.clear();
				clu.currentNearUsers.clear();
			}
		}

	} while (!convergence);

	for (Cluster& clu : clusters)
	{
		for (auto& usr : clu.currentNearUsers)
		{
			clu.initialNearUsers.insert({ usr.first, usr.second });
		}
	}
	std::cout << "K-MEAN CLUSTERING PROCESS DONE" << std::endl;

	double a = 0.5; //weightening factor
	//calculate the weightScores
	std::map<int, double> weightScores;
	for (Cluster& cl : clusters)
	{
		std::map<int, double> closerBSID = findCloserBaseStation(cl, basestations);
		double weight = 0.0;
		weight = (a * cl.getCurrentClusterUsers().size()) + ((1 - a) * closerBSID.begin()->second);
		weightScores.insert({ cl.clusterNum, weight });
	}

	//Sort in descending order and get the D = 10 with the biggest scores
	std::vector<std::pair<int, double>> sortedWeightScores = sort(weightScores);
	std::cout << std::endl;
	std::vector<int> idsOfFinalClusters;
	int i = 0;
	for (auto& scores : sortedWeightScores)
	{
		if (i < 10)
		{
			std::cout << scores.first << "   " << scores.second << std::endl;
			idsOfFinalClusters.push_back(scores.first);
		}
		i++;
	}
	//The optimal final clusters with the coordinates that the drones should be deployed & cover
	for (Cluster& cl : clusters)
	{
		for (auto& id : idsOfFinalClusters)
		{
			if (id == cl.clusterNum)
			{
				Drone drone(cl.mendoidX, cl.mendoidY, renderer);
				std::map<int, User> users = cl.initialNearUsers;
				std::vector<int> droneusersUniqueIDs = drone.provideService(users);

				for (auto& st : basestations)
				{
					st.eraseChannels(droneusersUniqueIDs);
				}
				drones.push_back(drone);
			}
		}
	}

	// END OF KDP ALGORITHM
	*/
	return drones;

}
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

	double distance(Cluster c, int x, int y) {
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

double calculateInterference(User& user, std::vector<BaseStation>& basestations, int currentStationID, std::vector<Drone>& drones)
{
	double sum = 0;
	for (BaseStation& station : basestations)
	{
		double channel = station.provideService(user, 1);
		double Pr = (station.getPrs() * channel * (-10)) / user.getTemporaryPathLoss(); //Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
		sum = sum + Pr;
		
	}

	for (Drone& drone : drones)
	{
		if (drone.getId() != currentStationID)
		{
			double Pr = (drone.getPrs() * user.getChannel() / user.getPathLoss()); 
			sum = sum + Pr;
		}
	}
	return sum;
}


std::vector<Drone> calculateOptimalPoints(std::vector<BaseStation>& basestations, std::vector<User>& users, SDL_Renderer* renderer)
{
	/*  https://www.geeksforgeeks.org/ml-k-medoids-clustering-with-example/?fbclid=IwAR2_onM03shuKGWYKgvG7awWo5TA4Ma25QQd8BQtrEzagNiJnsjLR_u-95w
	*											&&						
				An Unsupervised Machine Learning Approach for UAV-Aided Offloading of 5G Cellular Networks
						Lefteris Tsipi , Michail Karavolos and Demosthenes Vouyioukas
			Telecom 2022, 3, 86–102. https://doi.org/10.3390/telecom3010005 | https://www.mdpi.com/journal/telecom    

												*/
	
		
	//calculate total mean RSs before UAV deployment
	double generalSum = 0.0;
	for (auto& s : basestations)
	{
		double sumPr = 0;
		for (auto& ss : s.getChannels())
		{
			double pathloss = 0.0;
			for (auto& pa : s.getPathlosses())
			{
				if (pa.first == ss.first)
				{
					pathloss = pa.second;
				}
			}
			double Pr = (-6.9 * (ss.second * ss.second)) / pathloss;
			sumPr = sumPr + Pr;
		}
		double something = sumPr/s.getChannels().size();
		generalSum = (generalSum + something);
		std::cout << "RSS => " << generalSum << std::endl;
	}
	/*
	std::cout << "Mean-Shift ALGORITHM" << std::endl;
	std::cout << "out of  " << users.size() << std::endl;
	std::vector<Drone> drones;
	std::vector<Cluster> clusters;
	int windowSize = 50;
	bool convergence = false;
	int i = 0;

	do{
		int convergencecounter = 0;
		int x = rand() % 440 + 420;
		int y = rand() % 487 + 150;
		Cluster cl = Cluster(x, y, i + 1);
		i++;
		do
		{
			for (auto& usr : users)
			{
				if ((abs(x - usr.getX()) <= windowSize) && (abs(y - usr.getY()) <= windowSize))
				{
					cl.addInitialUserToCluster(usr);
				}
			}
			double meanX = 0.0;
			double meanY = 0.0;
			int newX = 0;
			int newY = 0;
			for (auto& clusterUsr : cl.getInitialClusterUsers())
			{
				meanX = meanX + clusterUsr.second.getX();
				meanY = meanY + clusterUsr.second.getY();
			}
			newX = meanX / cl.initialNearUsers.size();
			newY = meanY / cl.initialNearUsers.size();
			cl.setOldX(cl.mendoidX);
			cl.setOldY(cl.mendoidY);
			cl.setX(newX);
			cl.setY(newY);

			for (auto& usr : users)
			{
				if ((abs(cl.mendoidX - usr.getX()) <= windowSize) && (abs(cl.mendoidY - usr.getY()) <= windowSize))
				{
					cl.addCurrentUserToCluster(usr);
				}
			}
			meanX = 0.0;
			meanY = 0.0;
			newX = 0;
			newY = 0;
			for (auto& clusterUsr : cl.getCurrentClusterUsers())
			{
				meanX = meanX + clusterUsr.second.getX();
				meanY = meanY + clusterUsr.second.getY();
			}
			newX = meanX / cl.currentNearUsers.size();
			newY = meanY / cl.currentNearUsers.size();
			cl.setX(newX);
			cl.setY(newY);

			if ((cl.oldX == newX) && (cl.oldY == newY))
			{
				convergencecounter++;
			}
			cl.initialNearUsers.clear();
			for (auto it = cl.currentNearUsers.begin(); it != cl.currentNearUsers.end(); ++it)
			{
				cl.initialNearUsers.insert({ it->first, it->second });
			}
			cl.currentNearUsers.clear();
		} while (convergencecounter == 0);

		bool accepted = true;
		for (auto& c : clusters)
		{
			if ((c.mendoidX == cl.mendoidX) && (c.mendoidY == cl.mendoidY) || ( ( abs(c.mendoidX - cl.mendoidX) <70 ) && ( abs(c.mendoidY - cl.mendoidY) < 70 ) ) )
			{
				accepted = false;
			}
		}
		if ((cl.initialNearUsers.size() > 7) && accepted)
		{
			//check for close mendoids
			clusters.emplace_back(cl);
		}

	} while (clusters.size() < 10);

	//make sure there are no duplicates
	for (auto& usr : users)
	{
		bool found = false;
		int idToFind = usr.getUniqueID();
		for (auto& cl : clusters)
		{
			auto it = cl.initialNearUsers.find(idToFind);
			if (it != cl.initialNearUsers.end())
			{
				if (found)
				{
					cl.initialNearUsers.erase(it);
				}
				else
				{
					found = true;
				}
			}
		}
	}

	int droneid = 1;
	for (Cluster& cl : clusters)
	{
		Drone drone(cl.mendoidX, cl.mendoidY, droneid, renderer);
		std::map<int, User> users = cl.initialNearUsers;
		std::vector<int> droneusersUniqueIDs = drone.provideService(users);
		for (auto& st : basestations)
		{
			st.eraseChannels(droneusersUniqueIDs);
		}
		drones.push_back(drone);
		droneid++;
	}

	//update channel ,pathloss and station id for each user (after the UAV deployment)
	for (auto& dro : drones)
	{
		auto mapChannels = dro.getChannels();
		auto mapPathlosses = dro.getPathlosses();
		for (auto& user : users)
		{
			auto it = mapChannels.find(user.getUniqueID());
			if (it != mapChannels.end())
			{
				user.setChannel(it->second);
				user.setStationId(-1);
				user.setDroneId(dro.getId());
				user.setNumU(dro.getChannels().size());
			}
			auto it1 = mapPathlosses.find(user.getUniqueID());
			if (it1 != mapPathlosses.end())
			{
				user.setPathLoss(it1->second);
				user.setStationId(-1);
				user.setDroneId(dro.getId());
				user.setNumU(dro.getChannels().size());
			}
		}
	}
	*/
	/*
*  THIS IS THE END OF MEAN-SHIFT ALGORITHM
*
*     BELOW IS THE MINI BATCH K-MEAN CLUSTERING
			  ALGORITHM
*               */

	/*
	std::cout << "MiniBACH ALGORITHM" << std::endl;
	std::cout << "out of  " << users.size() << std::endl;
	std::vector<Drone> drones;
	std::vector<Cluster> clusters;
	int k = 10;
	int iterations = 20;
	int batchSize = ceil(users.size() / static_cast<double>(10));

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

	//find the closest medoid to each user as an initial calculation
	for (User& usr : users)
	{
		int clusterid = findCloser(usr, clusters);
		clusters.at(clusterid - 1).addInitialUserToCluster(usr);
	}

	for (int i = 1; i <= iterations; i++)
	{
		//select random users for the batch set
		std::vector<User> batchUsers;
		for (int j = 1; j <= batchSize; j++)
		{
			int randomUserElement = rand() % users.size();
			batchUsers.push_back(users.at(randomUserElement));
		}
		//for each batch user find the closer cluster
		for (auto& usr : batchUsers)
		{
			int clusterid = findCloser(usr, clusters);
			clusters.at(clusterid - 1).addCurrentUserToCluster(usr);
		}
		//calculate the new cluster center
		for (auto& cl : clusters)
		{
			double meanX = 0.0;
			double meanY = 0.0;
			int newX = 0;
			int newY = 0;
			for (auto& clusterUsr : cl.getCurrentClusterUsers())
			{
				meanX = meanX + clusterUsr.second.getX();
				meanY = meanY + clusterUsr.second.getY();
			}
			if (meanX > 0 && meanY > 0)
			{
			newX = meanX / cl.currentNearUsers.size();
			newY = meanY / cl.currentNearUsers.size();
			cl.setOldX(cl.mendoidX);
			cl.setOldY(cl.mendoidY);
			cl.setX(newX);
			cl.setY(newY);
			}
			cl.initialNearUsers.clear();
			for (auto it = cl.currentNearUsers.begin(); it != cl.currentNearUsers.end(); ++it)
			{
				cl.initialNearUsers.insert({ it->first, it->second });
			}
		}
	}
	//make sure there are no duplicates
	for (auto& usr : users)
	{
		bool found = false;
		int idToFind = usr.getUniqueID();
		for (auto& cl : clusters)
		{
			auto it = cl.initialNearUsers.find(idToFind);
			if (it != cl.initialNearUsers.end())
			{
				if (found)
				{
					cl.initialNearUsers.erase(it);
				}
				else
				{
					found = true;
				}
			}
		}
	}


	int droneid = 1;
	for (Cluster& cl : clusters)
	{
		Drone drone(cl.mendoidX, cl.mendoidY, droneid, renderer);
		std::map<int, User> users = cl.initialNearUsers;
		std::vector<int> droneusersUniqueIDs = drone.provideService(users);
		for (auto& st : basestations)
		{
			st.eraseChannels(droneusersUniqueIDs);
		}
		drones.push_back(drone);
		droneid++;
	}

	//update channel ,pathloss and station id for each user (after the UAV deployment)
	for (auto& dro : drones)
	{
		auto mapChannels = dro.getChannels();
		auto mapPathlosses = dro.getPathlosses();
		for (auto& user : users)
		{
			auto it = mapChannels.find(user.getUniqueID());
			if (it != mapChannels.end())
			{
				user.setChannel(it->second);
				user.setStationId(-1);
				user.setDroneId(dro.getId());
				user.setNumU(dro.getChannels().size());
			}
			auto it1 = mapPathlosses.find(user.getUniqueID());
			if (it1 != mapPathlosses.end())
			{
				user.setPathLoss(it1->second);
				user.setStationId(-1);
				user.setDroneId(dro.getId());
				user.setNumU(dro.getChannels().size());
			}
		}
	}
	*/
	/*
*  THIS IS THE END OF BATCH ALGORITHM
*
*     BELOW IS THE UDP CLUSTERING 
              ALGORITHM
*               */

/*
	
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
	int droneid = 1;
	for (Cluster& cl : clusters)
	{
		for (auto& id : idsOfFinalClusters)
		{
			if (id == cl.clusterNum)
			{
				if (cl.initialNearUsers.size() > 5)
				{
					Drone drone(cl.mendoidX, cl.mendoidY, droneid, renderer);
					std::map<int, User> users = cl.initialNearUsers;
					std::vector<int> droneusersUniqueIDs = drone.provideService(users);
					for (auto& st : basestations)
					{
						st.eraseChannels(droneusersUniqueIDs);
					}
					drones.push_back(drone);
					droneid++;
				}

			}
		}
	}

	//update channel and pathloss for each user (after the UAV deployment)
	for (auto& dro : drones)
	{
		auto mapChannels = dro.getChannels();
		auto mapPathlosses = dro.getPathlosses();
		for (auto& user : users)
		{
			auto it = mapChannels.find(user.getUniqueID());
			if (it != mapChannels.end())
			{
				user.setChannel(it->second);
				user.setStationId(-1);
				user.setDroneId(dro.getId());
				user.setNumU(dro.getChannels().size());
			}
			auto it1 = mapPathlosses.find(user.getUniqueID());
			if (it1 != mapPathlosses.end())
			{
				user.setPathLoss(it1->second);
				user.setStationId(-1);
				user.setDroneId(dro.getId());
				user.setNumU(dro.getChannels().size());
			}
		}
	}
	
	double spectral = 0.0;
	for (auto& dr : drones)
	{
		for (auto& usr : users)
		{
			if (usr.getStationId() == -1)
			{
				double Pr = (-6.9999 * usr.getChannel() * (-10)) / usr.getPathLoss();//Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
				double I = calculateInterference(usr, basestations, usr.getDroneId(), drones);
				double SINR = (Pr / (4 + I)) ;
				usr.setSINR(abs(SINR));
				double rate = ((5e+6 / usr.getNumU()) / (5e+6 + 5e+6)) * (log2(1 + abs(SINR)));
				spectral = spectral + rate;
			}
		}
		std::cout << "Spectral " << spectral << std::endl;
	}


	/*
	//to calculate the Rss after UAV deployement
	double generalSum1 = 0.0;
	int i1 = 1;
	
	for (auto& d : drones)
	{
		double sumPr = 0;
		for (auto& ch : d.getChannels())
		{
			double pathloss = 0.0;
			for (auto& pa : d.getPathlosses())
			{
				if (pa.first == ch.first)
				{
					pathloss = pa.second;
				}
			}
			double Pr = (-6.9 * (ch.second *ch.second) ) / pathloss;
			sumPr = sumPr + Pr;
		}
		generalSum1 = (generalSum1 + sumPr) / i1;
		double output = generalSum - generalSum1;
		std::cout << "RSS => " << output << std::endl;
		i++;
	}*/

	/*
//to calculate the Sum Rate
double sumRate = 0.0;
for (auto& dr : drones)
{
	for (auto& usr : users)
	{
		if (usr.getStationId() == -1)
		{
			double Pr = (-6.9999 * usr.getChannel() * (-10)) / usr.getPathLoss();
			double I = calculateInterference(usr, basestations, usr.getDroneId(), drones);
			double SINR = (Pr / (4 + I));
			usr.setSINR(abs(SINR));
			double rate = (5e+6 / usr.getNumU()) * (log2(1 + abs(SINR)));
			sumRate = sumRate + rate;
		}
	}
	std::cout << "SUM RATE " << sumRate << std::endl;
}

std::cout<< "SUM RATE " << sumRate << std::endl;
*/
		
		/*			
	*  THIS IS THE END OF UDP ALGORITHM
	* 
	*     BELOW IS THE K-MEAN MENDOID 
	         CLUSTERING ALGORITHM  
	*               */
	
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

	//make sure there are no duplicates
	for (auto& usr : users)
	{
		bool found = false;
		int idToFind = usr.getUniqueID();
		for (auto& cl : clusters)
		{
			auto it = cl.initialNearUsers.find(idToFind);
			if (it != cl.initialNearUsers.end())
			{
				if (found)
				{
					cl.initialNearUsers.erase(it);
				}
				else
				{
					found = true;
				}
			}
		}
	}



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
	int droneid = 1;
	for (Cluster& cl : clusters)
	{
		for (auto& id : idsOfFinalClusters)
		{
			if (id == cl.clusterNum)
			{
				if (cl.initialNearUsers.size() > 3)
				{
					Drone drone(cl.mendoidX, cl.mendoidY, droneid, renderer);
					std::map<int, User> users = cl.initialNearUsers;
					std::vector<int> droneusersUniqueIDs = drone.provideService(users);

					for (auto& st : basestations)
					{
						st.eraseChannels(droneusersUniqueIDs);
					}
					drones.push_back(drone);
					droneid++;
				}

			}
		}
	}
	//update channel ,pathloss and station id for each user (after the UAV deployment)
	for (auto& dro : drones)
	{
		auto mapChannels = dro.getChannels();
		auto mapPathlosses = dro.getPathlosses();
		for (auto& user : users)
		{
			auto it = mapChannels.find(user.getUniqueID());
			if (it != mapChannels.end())
			{
				user.setChannel(it->second);
				user.setStationId(-1);
				user.setDroneId(dro.getId());
				user.setNumU(dro.getChannels().size());
			}
			auto it1 = mapPathlosses.find(user.getUniqueID());
			if (it1 != mapPathlosses.end())
			{
				user.setPathLoss(it1->second);
				user.setStationId(-1);
				user.setDroneId(dro.getId());
				user.setNumU(dro.getChannels().size());
			}
		}
	}

	//to calculate spectral efficiency
	double spectral = 0.0;
	for (auto& dr : drones)
	{
		for (auto& usr : users)
		{
			if (usr.getStationId() == -1)
			{
				double Pr = (-6.9999 * usr.getChannel() * (-10)) / usr.getPathLoss();//Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
				double I = calculateInterference(usr, basestations, usr.getDroneId(), drones);
				double SINR = (Pr / (4 + I));
				usr.setSINR(abs(SINR));
				double rate = ((5e+6 / usr.getNumU()) / (5e+6 + 5e+6)) * (log2(1 + abs(SINR)));
				spectral = spectral + rate;
			}
		}
		std::cout << "SUM RATE " << spectral << std::endl;
	}

	std::cout << "SUM RATE " << spectral << std::endl;
	
	// END OF KDP ALGORITHM

	return drones;

}
/*
//To calculate the offloading
double percentage = 0.0;
for (auto& d : drones)
{
	std::multimap<int, double> get = d.getChannels();
	int s = get.size();
	int s1 = users.size();
	double percentage1 = ((static_cast<double>(s) / s1) * 100);
	percentage = percentage + percentage1;
	std::cout << percentage << "  %" << std::endl;
}
*/


//to calculate the Rss after UAV deployement
/*
double generalSum1 = 0.0;
int i1 = 1;

for (auto& d : drones)
{
	double sumPr = 0;
	for (auto& ch : d.getChannels())
	{
		double pathloss = 0.0;
		for (auto& pa : d.getPathlosses())
		{
			if (pa.first == ch.first)
			{
				pathloss = pa.second;
			}
		}
		double Pr = (-6.9 * (ch.second *ch.second) ) / pathloss;
		sumPr = sumPr + Pr;
	}
	generalSum1 = (generalSum1 + sumPr)/i1;
	double output = generalSum - generalSum1;
	std::cout << "RSS => " << output << std::endl;
	i++;
}*/











/*
//to calculate the Sum Rate
double sumRate = 0.0;
for (auto& dr : drones)
{
	for (auto& usr : users)
	{
		if (usr.getStationId() == -1)
		{
			double Pr = (-6.9999 * usr.getChannel() * (-10)) / usr.getPathLoss();
			double I = calculateInterference(usr, basestations, usr.getDroneId(), drones);
			double SINR = (Pr / (4 + I));
			usr.setSINR(abs(SINR));
			double rate = (5e+6 / usr.getNumU()) * (log2(1 + abs(SINR)));
			sumRate = sumRate + rate;
		}
	}
	std::cout << "SUM RATE " << sumRate << std::endl;
}

std::cout<< "SUM RATE " << sumRate << std::endl;
*/



/*
//to calculate spectral efficiency
double spectral = 0.0;
for (auto& dr : drones)
{
	for (auto& usr : users)
	{
		if (usr.getStationId() == -1)
		{
			double Pr = (-6.9999 * usr.getChannel() * (-10)) / usr.getPathLoss();//Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
			double I = calculateInterference(usr, basestations, usr.getDroneId(), drones);
			double SINR = (Pr / (4 + I));
			usr.setSINR(abs(SINR));
			double rate = ((5e+6/usr.getNumU())/(5e+6 + 5e+6)) * (log2(1 + abs(SINR)));
			spectral = spectral + rate;
		}
	}
	std::cout << "SUM RATE " << spectral << std::endl;
}

std::cout << "SUM RATE " << spectral << std::endl;
*/


/*
//to calculate spectral efficiency
double spectral = 0.0;
for (auto& dr : drones)
{
	for (auto& usr : users)
	{
		if (usr.getStationId() == -1)
		{
			double Pr = (-6.9999 * usr.getChannel() * (-10)) / usr.getPathLoss();//Gt = 15dbi == dbm = dbi +2.15 thus Gt ==0.1W == -10 dB
			double I = calculateInterference(usr, basestations, usr.getDroneId(), drones);
			double SINR = (Pr / (4 + I));
			usr.setSINR(abs(SINR));
			double rate = ((5e+6 / usr.getNumU()) / (5e+6 + 5e+6)) * (log2(1 + abs(SINR)));
			spectral = spectral + rate;
		}
	}
	std::cout << "SPEC EFFIC " << spectral << std::endl;
}

std::cout << "SPEC EFFIC " << spectral << std::endl;

*/
#include "checkSurroundings.h"

void checkSurroundingBasestations(User& user, std::vector<BaseStation>& basestations, int i)
{
	std::cout << user.getUniqueID() << std::endl;
	std::cout << basestations.at(i).getID() << std::endl;
	int realStationId = i + 1;

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

}

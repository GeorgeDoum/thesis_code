#pragma once
#include <iostream>
#include <vector>
#include "Basestation.h"
#include "User.h"
void doMeasurements(std::vector<BaseStation>& basestations, std::vector<User>& users);
double calculateInterference(User& user, std::vector<BaseStation>& basestations, int currentStationID);

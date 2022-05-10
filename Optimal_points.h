#pragma once
#include <iostream>
#include "BaseStation.h"
#include "User.h"
#include <functional>
#include <numeric>
#include <SDL.h>
#include <SDL_ttf.h>
#include "SDL_render.h"
#include "Drone.h"
std::vector<Drone> calculateOptimalPoints(std::vector<BaseStation>& basestations, std::vector<User>& users, SDL_Renderer* renderer);
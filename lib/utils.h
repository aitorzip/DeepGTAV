/*
	THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
				http://dev-c.com
			(C) Alexander Blade 2015
*/

#pragma once

#include <string>
#include "script.h"

// returns module load path with trailing slash
std::string GetCurrentModulePath();
void update_status_text();
void set_status_text(std::string str, DWORD time = 2500, bool isGxtEntry = false);

float getFloatValue(Vehicle vehicle, int offset);
void setFloatValue(Vehicle vehicle, int offset, float value);
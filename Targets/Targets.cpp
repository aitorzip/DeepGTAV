#include "Targets.h"
#include "../lib/script.h"
#include <vector>

//TODO:
// -Objects
// -Return also PED type (animal/human) and maybe Vehicle type (discard boats, planes, trains and helicopters)
// -Set max distance and return ordered by distance??
std::vector<Vehicle> getVehiclesInScreen(Vehicle vehicle) {
	const int ARR_SIZE = 1024;
	Vehicle vehicles[ARR_SIZE];
	std::vector<Vehicle> _vehicles;
	int count = worldGetAllVehicles(vehicles, ARR_SIZE);
	for (int i = 0; i < count; i++) {
		if (ENTITY::IS_ENTITY_ON_SCREEN(vehicles[i])) {
			if (!ENTITY::IS_ENTITY_IN_AIR(vehicles[i]) && !ENTITY::IS_ENTITY_IN_WATER(vehicles[i])) {
				if (ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(vehicle, vehicles[i], 19)){
					Vector3 coord = ENTITY::GET_ENTITY_COORDS(vehicles[i], FALSE);
					float x, y;
					if (GRAPHICS::_WORLD3D_TO_SCREEN2D(coord.x, coord.y, coord.z, &x, &y))
					{
						char text[256];
						sprintf_s(text, "^\nVehicle\n");
						UI::SET_TEXT_FONT(0);
						UI::SET_TEXT_SCALE(0.2, 0.2);
						UI::SET_TEXT_COLOUR(255, 255, 255, 255);
						UI::SET_TEXT_WRAP(0.0, 1.0);
						UI::SET_TEXT_CENTRE(0);
						UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
						UI::SET_TEXT_EDGE(1, 0, 0, 0, 205);
						UI::_SET_TEXT_ENTRY("STRING");
						UI::_ADD_TEXT_COMPONENT_STRING(text);
						UI::_DRAW_TEXT(x, y);
						// box
						GRAPHICS::DRAW_RECT(x + 0.027f, y + 0.043f, 0.058f, 0.056f, 75, 75, 75, 75);
					}
					//_vehicles.push_back(vehicles[i]);
				}
			}
		}
	}

	return _vehicles;
}

std::vector<Ped> getPedsInScreen(Vehicle vehicle) {
	const int ARR_SIZE = 1024;
	Ped peds[ARR_SIZE];
	std::vector<Ped> _peds;
	int count = worldGetAllPeds(peds, ARR_SIZE);
	for (int i = 0; i < count; i++) {
		if (ENTITY::IS_ENTITY_ON_SCREEN(peds[i])) {
			if (!PED::IS_PED_IN_ANY_VEHICLE(peds[i], FALSE)) {
				if (PED::GET_PED_TYPE(peds[i]) != 28) { //ANIMAL
					if (ENTITY::HAS_ENTITY_CLEAR_LOS_TO_ENTITY(vehicle, peds[i], 19)) {
						Vector3 coord = ENTITY::GET_ENTITY_COORDS(peds[i], TRUE);
						float x, y;
						if (GRAPHICS::_WORLD3D_TO_SCREEN2D(coord.x, coord.y, coord.z, &x, &y))
						{
							char text[256];
							sprintf_s(text, "^\nPed\n");
							UI::SET_TEXT_FONT(0);
							UI::SET_TEXT_SCALE(0.2, 0.2);
							UI::SET_TEXT_COLOUR(255, 255, 255, 255);
							UI::SET_TEXT_WRAP(0.0, 1.0);
							UI::SET_TEXT_CENTRE(0);
							UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
							UI::SET_TEXT_EDGE(1, 0, 0, 0, 205);
							UI::_SET_TEXT_ENTRY("STRING");
							UI::_ADD_TEXT_COMPONENT_STRING(text);
							UI::_DRAW_TEXT(x, y);
							// box
							GRAPHICS::DRAW_RECT(x + 0.027f, y + 0.043f, 0.058f, 0.056f, 75, 75, 75, 75);
						}
						//_peds.push_back(peds[i]);
					}
				}
			}		
		}
	}
}
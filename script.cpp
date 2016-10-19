/*
	THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
				http://dev-c.com			
			(C) Alexander Blade 2015
*/

#include "script.h"
#include <sstream>
#include "Environment.h"
#include "Targets.h"

void test(Environment environment)
{
	Vehicle vehicle = environment.getScenario().getVehicle();
	float reward = environment.getRewarder().computeReward(vehicle);

	//set_status_text("reward: " + std::to_string(reward));
	//update_status_text();

	/*Vector3 currentPosition;
	int nodeID;
	tNode node;
	std::vector<tLink> links;
	
	currentPosition = ENTITY::GET_ENTITY_COORDS(vehicle, FALSE);
	nodeID = PATHFIND::GET_NTH_CLOSEST_VEHICLE_NODE_ID(currentPosition.x, currentPosition.y, currentPosition.z, 1, 1, 300, 300);
	node = nodes[nodeID];
	links = node.links;

	if (GRAPHICS::_WORLD3D_TO_SCREEN2D(node.coord.x, node.coord.y, node.coord.z, &x, &y))
	{
		char text[256];
		sprintf_s(text, "^\nNode\n|D: %d, V %d, S %d\n|DS %d, H %d, SL %d\n", node.attr.disabled, node.attr.speed, node.attr.special, node.attr.density, node.attr.highway, node.attr.slipLane);
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

	for (int i = 0; i < node.links.size(); i++) {
		tLink link = node.links.at(i);
		if (GRAPHICS::_WORLD3D_TO_SCREEN2D(node.links.at(i).coord.x, node.links.at(i).coord.y, node.links.at(i).coord.z, &x, &y))
		{
			char text[256];
			sprintf_s(text, "^\nLink\n|W %d, LI %d, LO %d, N %d\n|B %d, SH %d\n", node.links.at(i).attr.width, node.links.at(i).attr.lanesIn, node.links.at(i).attr.lanesOut, node.links.at(i).attr.narrowRoad, node.links.at(i).attr.blockIfNoLanes, node.links.at(i).attr.shortcut);
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

		for (int j = 0; j < link.linePoints.size(); j++) {
			if (GRAPHICS::_WORLD3D_TO_SCREEN2D(link.linePoints.at(j).coord.x, link.linePoints.at(j).coord.y, link.linePoints.at(j).coord.z, &x, &y))
			{
				char text[256];
				sprintf_s(text, "^\nLine %d\n", link.linePoints.at(j).laneIn);
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
		}
	}*/

	

}


void main()
{	
	Environment environment;
	while (true)
	{
		environment.step();
		//test
		getVehiclesInScreen(environment.getScenario().getVehicle());
		getPedsInScreen(environment.getScenario().getVehicle());
		//end test
		WAIT(0);		
	}
}

void ScriptMain()
{
	srand(GetTickCount());
	main();
}

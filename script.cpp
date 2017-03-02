/*
	THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
				http://dev-c.com			
			(C) Alexander Blade 2015
*/

#include "lib/script.h"
#include "Server.h"

void ScriptMain()
{
	Server server(8000);
	while (true) {
		while (!server.clientConnected) {
			server.checkClient();
			scriptWait(0);
		}
		while (server.clientConnected) {
			server.checkRecvMessage();
			server.checkSendMessage();
			server.scenario.run();
		}
	}
}

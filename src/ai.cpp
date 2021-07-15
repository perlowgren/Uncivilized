
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <amanita/aRandom.h>

#include "main.h"
#include "city.h"
#include "unit.h"
#include "path.h"


void Game::doAI(Player *pl) {
debug_output("Game::doAI(CMD_END_TURN CMD_END_TURN CMD_END_TURN CMD_END_TURN CMD_END_TURN CMD_END_TURN)\n");
	sendServerCommand(0,CMD_END_TURN,pl->id);
}




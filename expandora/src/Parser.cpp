#include "Parser.h"

void Parser::setTerrain(Property * ter) {
	
	activeTerrain = (terrains.find((*ter)[0]))->second; // the first character has to be the terrain id
}

void Parser::event(ParseEvent * ev) {
	switch (ev->type) {
		case ROOM:
			mudEvents.push(ev);
			break;
		case MOVE_FAIL:
			mudEvents.push(ev);
			break;
		case MOVE:
			playerEvents.push(ev);
			break;
		case UNIQUE:
			playerEvents.push(ev);
			break;
		case NOTE:
			dropNote(ev);
			return;
	}
	checkQueues();
}       

void Parser::checkQueues() {
	if (mudEvents.empty()) return;
	if (playerEvents.empty() && state != SYNCING) state = SYNCING;	
	else while (mudEvents.front()->timestamp < playerEvents.front()->timestamp && !(mudEvents.empty())) {
		mudEvents.pop();
		state = SYNCING;
	}
	if (mudEvents.empty()) return;
	
	//now we are sure we have a user event that happened before the mud event
	
	
	switch (state) {
		case APPROVED:
			approved();
			break;
		case EXPERIMENTING:
			experimenting();
			break;
		case SYNCING:
			syncing();
			break;
	}
}	

void Parser::approved() {
	if (mudEvents.front()->type == MOVE_FAIL) {
		mudEvents.pop();
		playerEvents.pop();
		return;
	}
	if (playerEvents.front()->type == UNIQUE) {
		mostLikelyRoom->setUnique() = true;
		playerEvents.pop();
		return;
	}
	
	// now we have a move and a room on the event queues;
	Room * perhaps = 0;
	RoomCollection * possible = mostLikelyRoom->go(playerEvents.front());
	
	if ((perhaps = possible->matchOne(mudEvents.front())) != 0) { // narrows possible by the event and return a Room if exactly one is left
		mostLikelyRoom = perhaps;
		mudEvents.pop();
		playerEvents.pop();
		return;
	}
	else {
		state = EXPERIMENTING;
       		buildPaths(possible);
	}
}


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
	if (playerEvents.front()->tpye == unify) {
		mostLikelyRoom.unique = true;
		return;
	}
	RoomCollection * possible =

#include "Parser.h"
#include "RoomAdmin.h"

void Parser::setTerrain(Property * ter) {
	
	activeTerrain = (terrains.find((*ter)[0]))->second; // the first character has to be the terrain id
}

void Parser::event(BaseEvent * ev) {
	if (ev->type >= 0) {
		// a move event
		playerEvents.push(ev);
	}
	
	else {
		
		switch (ev->type) {
			case ROOM:
			case MOVE_FAIL:
				mudEvents.push(ev);
				break;
			case UNIQUE:
				playerEvents.push(ev);
				break;
			case NOTE:
				// goes into mostLikelyRoom
				dropNote((ParseEvent *)ev);
				return;
		}
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
	if (playerEvents.front()->type == UNIQUE) {
		mostLikelyRoom->setUnique();
		playerEvents.pop();
		return;
	}
	if (mudEvents.front()->type == MOVE_FAIL) {
		mudEvents.pop();
		playerEvents.pop();
		return;
	}
	
	// now we have a move and a room on the event queues;
	Room * perhaps = 0;
	RoomCollection * possible = mostLikelyRoom->go(playerEvents.front());
	if (possible == 0) {
		Coordinate * c = getExpectedCoordinate();
		perhaps = roomAdmin.getRoom(c);
		if ((perhaps == 0) || (!perhaps->fastCompare((ParseEvent *)mudEvents.front()))) {
			perhaps = roomAdmin.insertRoom((ParseEvent *)mudEvents.front(), c);
			if (perhaps == 0) {
				// can't insert it for some reason - for example skipped props
				state = SYNCING;
				playerEvents.pop();
				mudEvents.pop();
				cmm.deactivate(c);
				return;
			}
			else {	
				possible = rcmm.activate()->merge(perhaps->getHome());
				state = EXPERIMENTING;
				cmm.deactivate(c);
			}
		}
		else {
			mostLikelyRoom->addExit(playerEvents.front()->type, perhaps);
			mostLikelyRoom = perhaps;
			playerEvents.pop();
			mudEvents.pop();
			cmm.deactivate(c);
			return;
		}
	}
	else {
		possible = rcmm.activate();
		possible->merge(possible);
	
		if ((perhaps = possible->matchOne((ParseEvent *)mudEvents.front())) != 0) { // narrows possible by the event and return a Room if exactly one is left
			mostLikelyRoom = perhaps;
			mudEvents.pop();
			playerEvents.pop();
			rcmm.deactivate(possible);
			return;
		}
		else {
			Coordinate * c = getExpectedCoordinate();
			possible->merge(roomAdmin.insertRoom((ParseEvent *)mudEvents.front(), c)->getHome());
			state = EXPERIMENTING;
			cmm.deactivate(c);
		}
	}
       	buildPaths(possible);
	rcmm.deactivate(possible);
}

void Parser::syncing() {
	if (playerEvents.front()->type == UNIQUE) {
		// unique doesn't work when syncing ...
		playerEvents.pop();
		return;
	}
	if (mudEvents.front()->type == MOVE_FAIL) {
		mudEvents.pop();
		playerEvents.pop();
		return;
	}
	
	// now we have a move and a room on the event queues;
	
	RoomCollection * possible = roomAdmin.getRoom(mudEvents.front());
	if (possible->numRooms > 0) {
		state = EXPERIMENTING;
		buildPaths(possible);
		rcmm.deactivate(possible);
	}
	else if (possible->numRooms == 0) rcmm.deactivate(possible);

}	

Coordinate * Parser::getExpectedCoordinate() {
	Coordinate * c = cmm.activate();
	c->add(stdMoves[playerEvents.front()->type]);
	c->add(mostLikelyRoom->getCoordinate());
	return c;
}


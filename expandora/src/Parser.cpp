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
				// goes into mostLikelyRoom and isn't pushed on the queue, so that the queuePop won't get confused
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
		mudPop();
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
		playerPop();
		return;
	}
	if (mudEvents.front()->type == MOVE_FAIL) {
		mudPop();
		playerPop();
		return;
	}
	
	// now we have a move and a room on the event queues;
	Room * perhaps = 0;
	RoomCollection * possible = mostLikelyRoom->go(playerEvents.front());
	if (possible->numRooms() == 0) {
		Coordinate * c = getExpectedCoordinate();
		perhaps = roomAdmin.getRoom(c);
		if ((perhaps == 0) || (!perhaps->fastCompare((ParseEvent *)mudEvents.front()))) {
			perhaps = roomAdmin.insertRoom((ParseEvent *)mudEvents.front(), c);
			if (perhaps == 0) {
				// can't insert it for some reason - for example skipped props
				state = SYNCING;
				playerPop();
				mudPop();
				cmm.deactivate(c);
				rcmm->deactivate(possible);
				return;
			}
			else {	
				possible->merge(perhaps->getHome());
				state = EXPERIMENTING;
				cmm.deactivate(c);
			}
		}
		else {
			mostLikelyRoom->addExit(playerEvents.front()->type, perhaps);
			mostLikelyRoom = perhaps;
			playerPop();
			mudPop();
			cmm.deactivate(c);
			rcmm->deactivate(possible);
			return;
		}
	}
	else {
	
		if ((perhaps = possible->matchOne((ParseEvent *)mudEvents.front())) != 0) { // narrows possible by the event and return a Room if exactly one is left
			mostLikelyRoom = perhaps;
			mudPop();
			playerPop();
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

void playerPop() {
	if (playerEvents.front() >= 0) pemm.deactivate(playerEvents.front());
	else bemm.deactivate(playerEvents.front());
	playerEvents.pop();
}
			
void mudPop() {
	if (mudEvents.front() >= 0) pemm.deactivate(mudEvents.front());
	else bemm.deactivate(mudEvents.front());
	mudEvents.pop();
}

void Parser::syncing() {
	if (playerEvents.front()->type == UNIQUE) {
		// unique doesn't work when syncing ...
		playerPop();
		return;
	}
	if (mudEvents.front()->type == MOVE_FAIL) {
		mudPop();
		playerPop();
		return;
	}
	
	// now we have a move and a room on the event queues;
	
	RoomCollection * possible = roomAdmin.getRooms(mudEvents.front());
	if (possible->numRooms > 0) {
		state = EXPERIMENTING;
		buildPaths(possible);
		rcmm.deactivate(possible);
	}
	else if (possible->numRooms == 0) rcmm.deactivate(possible);
	playerPop();
	mudPop();

}	

Coordinate * Parser::getExpectedCoordinate() {
	Coordinate * c = cmm.activate();
	c->add(stdMoves[playerEvents.front()->type]);
	c->add(mostLikelyRoom->getCoordinate());
	return c;
}


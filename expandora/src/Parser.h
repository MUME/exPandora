
#include <queue.h>

class Parser {
	public:
		void setTerrain(char terrain);
		void event(ParseEvent * ev);
	private:
		char state;
		queue<ParseEvent *> playerEvents;
		queue<ParseEvent *> mudEvents;
};	

extern Parser parser;

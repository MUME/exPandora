#include <stdio.h>

class parent {
	public:
		parent();
		void pr();
	private:
		char p;
};

class child : public parent {
	public:
		child();
	private:
		char p;
};

parent::parent() {
	p = 'p';
}

child::child() {
	p = 'c';
}

void parent::pr() {
	printf("character: %c\n", p);
}

int main(int argc, char **argv) {
	parent par;
	child ch;
	par.pr();
	ch.pr();
}

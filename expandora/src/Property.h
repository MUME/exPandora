#define SKIPPED_ONE 0
#define SKIPPED_MANY -1

class Property {
	public:	
		Property();
		void clear();
		void skip();
		void skipMany();
		int size();
		int operator==(Property other);
		void operator+=(char c);
		void operator+=(char * string);
		char & operator[](int offset);
	private:
		char * begin;
		int length;
		int used;
		void enlarge(int neededSpace);
};


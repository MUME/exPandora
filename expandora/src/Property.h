#ifndef PROPERTY
#define PROPERTY

#define SKIPPED_ONE 0
#define SKIPPED_MANY -1


class Property {
	public:	
		Property(); // builds a NO_PROPERTY
		Property(char *);

		void clear();
		void skip();
		void skipMany();
		int size();
		
		char current();
		char next();
		char prev();
		
		char * rest();
		
		int operator==(Property & other);
		void operator+=(char c);
		void operator+=(char * string);
		char & operator[](int offset);
		int comp(Property & other);
	private:
		int currentOffset;
		char * begin;
		int length;
		int used;
		void enlarge(int neededSpace);
};


extern Property NO_PROPERTY;
#endif

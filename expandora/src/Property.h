#ifndef PROPERTY
#define PROPERTY

#define SKIPPED_ONE 0
#define SKIPPED_MANY -1

#include "ObjectRecycler.h"

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
		void add(char c);
		void add(char * string);
		void add(const char * other, const char * end);
		char operator[](int offset);
		int comp(Property & other);
		void copy(Property & other);
		int getOffset(){return currentOffset;};
		
	private:
		int currentOffset;
		char * begin;
		int length;
		int used;
		void enlarge(int neededSpace);
};


extern ObjectRecycler<Property> pmm;
#endif

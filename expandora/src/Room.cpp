#include "Room.h"
#include "string.h"

Room::Room(char ** _properties, int _numProperties) {
	properties = _properties;
	numProperties = _numProperties;
	int i = 0;
	for (i = 0; i < 6; i++) {
		doors[i] = properties[numProperties - (5-i)];
	}
}

/** compare the optional properties that are not present in the search tree
 * perhaps we should allow a tolerance, too?
 */
int Room::containsOptionals(char ** optionals, int num) {
	if (num > numOptionals) return 0;
	int j = 0;
	int i = 0;
	int matched = 0;
	for (i = 0; i < num; i++) {
	       	for (j = 0; j < numOptionals; j++) {
			if (!strcmp(optionals[i], optionalProperties[j])) {
				matched = 1;
				break;
			}
		}
		if (!matched) return 0;
		else matched = 0;
	}
	return 1;
}

/** we only compare the first num properties only to this room here
 * we allow a tolerance for typoes and such like pandora does
 */
int Room::fastCompare(char ** impProps, int num, int tolerance) {
	for (int i = 0; i < num; i++) {
		tolerance -= abs(strcmp(impProps[i], properties[i]));
		if (tolerance <= 0) return 0;
	}
	return 1;
}
				

#include "Room.h"
#include "Exit.h"

Room::Room(vector<char *> & _properties) {
	properties = _properties;
	int num = properties.size();
	int i = 0;
	for (i = 0; i < 6; i++) {
		doors[i] = properties[num - (6-i)];
	}
}

/** compare the optional properties that are not present in the search tree
 * perhaps we should allow a tolerance, too?
 */
int Room::containsOptionals(vector<char *> & optionals) {
	if (optionals.size() > optionalProperties.size()) return 0;
	int j = 0;
	int i = 0;
	int matched = 0;
	for (i = optionals.size(); i > 0; --i) {
	       	for (j = optionalProperties.size(); j > 0; --j) {
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
int Room::fastCompare(vector<char *> & imps, int tolerance) {
	for (int i = imps.size(); i > 0; --i) {
		tolerance -= abs(strcmp(imps[i], properties[i]));
		if (tolerance <= 0) return 0;
	}
	return 1;
}
				
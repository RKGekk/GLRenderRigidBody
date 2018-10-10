#ifndef COLLISIONDATA_H
#define COLLISIONDATA_H

#include "Contact.h"

struct CollisionData {

	Contact *contactArray;
	Contact *contacts;
	int contactsLeft;
	unsigned contactCount;
	float friction;
	float restitution;
	float tolerance;

	bool hasMoreContacts() {
		return contactsLeft > 0;
	}

	void reset(unsigned maxContacts) {
		contactsLeft = maxContacts;
		contactCount = 0;
		contacts = contactArray;
	}

	void addContacts(unsigned count) {

		contactsLeft -= count;
		contactCount += count;

		contacts += count;
	}
};

#endif // !COLLISIONDATA_H
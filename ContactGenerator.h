#ifndef CONTACTGENERATOR_H
#define CONTACTGENERATOR_H

#include "Contact.h"

class ContactGenerator {
public:

	virtual unsigned addContact(Contact *contact, unsigned limit) const = 0;
};

#endif // !CONTACTGENERATOR_H


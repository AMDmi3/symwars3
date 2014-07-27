#ifndef USTRING_H
#define USTRING_H

#include "platform.h"

//#define USTR_DEBUG

class UString
{
public:
	unsigned int m_length;	//length doesn't count null-terminator
	unsigned int* m_data;

	UString();
	~UString();
	UString(const UString& original);
	UString(const char* cstr);
	UString(unsigned int k);
	UString(unsigned int* k);
	UString& operator=(const UString &original);
	UString operator+(const UString &other);
	UString substr(int start, int len) const;
	int firstof(UString find) const;
	std::string rawstr() const;
};

#endif

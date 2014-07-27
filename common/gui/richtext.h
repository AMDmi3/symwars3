#ifndef RICHTEXT_H
#define RICHTEXT_H

#include "../platform.h"
#include "../ustring.h"

#define RICHTEXT_TEXT		0
#define RICHTEXT_ICON		1

// Rich text part
class RichTextP
{
public:
	int m_type;
	int m_icon;
	UString m_text;
	int texlen() const;	//each icon counts as 1
	int rawlen() const;	//icon tags are counted
	std::string texval() const;
	RichTextP substr(int start, int length) const;
	RichTextP();
	RichTextP(const RichTextP& original);
	RichTextP(const char* cstr);
	RichTextP(UString ustr);
	RichTextP& operator=(const RichTextP &original);
	RichTextP(int type, int subtype);
};

class RichText
{
public:
	std::list<RichTextP> m_part;
	std::string rawstr() const;
	int texlen() const;	//each icon counts as 1
	int rawlen() const;	//icon tags are counted
	RichText pwver() const;	//asterisk-mask password std::string
	RichText();
	RichText(const RichTextP& part);
	RichText(const RichText& original);
	RichText(const char* cstr);
	RichText& operator=(const RichText &original);
	RichText operator+(const RichText &other);
	RichText substr(int start, int length) const;
	//RichText parsetags(int* caret) const;
};

RichText ParseTags(RichText original, int* caret);

#endif

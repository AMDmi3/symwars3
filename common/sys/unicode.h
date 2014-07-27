#include "../platform.h"
#include "../ustring.h"

int ValidateUTF8(char unsigned *buff, int len);
void dump_unicode(char unsigned *buff, int len);
unsigned int *ToUTF32(const unsigned char *utf8, int len);
unsigned char *ToUTF8(const unsigned int *unicode);
void print_char(int pos, int len, unsigned int ch);
void dump_unicode_string(unsigned int *str);
int UTF8Len(unsigned char ch);

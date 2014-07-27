#include "unicode.h"
#include "../utils.h"


#define ERR(x) {g_log<<x<<std::endl; g_log.flush(); exit(1);}

// http://foolab.org/node/773
#if 0
int main(int argc, char *argv[])
{
	FILE *fp;
	struct stat buf;
	int x;
	unsigned char *buff;
	unsigned int *unicode = NULL;

	if (argc != 2)
	{
		ERR("Usage unicode <file>");
	}

	x = stat(argv[1], &buf);
	if (x != 0)
	{
		perror("stat");
		exit(1);
	}
	buff = (unsigned char *)malloc(buf.st_size +1);
	if (!buff)
		ERR("Not enough memory!");

	fp = fopen(argv[1], "r");
	if (!fp)
	{
		perror("fopen");
		exit(1);
	}
	x = fread(buff, 1, buf.st_size, fp);
	if (x != buf.st_size)
	{
		perror("fread");
		exit(1);
	}
	buff[x] = '\0';
	fclose(fp);
	if (ValidateUTF8(buff, x))
	{
		unicode = ToUTF32(buff, x);
		dump_unicode_string(unicode);
		//      dump_unicode(buff, x);
	}
	else
	{
		ERR("Invalid UTF-8 strings.");
	}
	//  printf("%s",buff);
	delete [] buff;
	buff = ToUTF8(unicode);
	if (unicode)
		free(unicode);
	printf("%s\n", buff);
	delete [] buff;
	return 0;
}
#endif

int ValidateUTF8(unsigned char *buff, int len)
{
	int x;
	for (x = 0; x < len; x++)
	{
		if ((unsigned char)(buff[x]) > 0xfd)
		{
			printf("Byte %i is invalid\n", x);
			return 0;
		}
	}
	return 1;
}

int UTF8Len(unsigned char ch)
{
	int l;
	unsigned char c = ch;
	c >>= 3;
	// 6 => 0x7e
	// 5 => 0x3e
	if (c == 0x1e)
	{
		l = 4;
	}
	else
	{
		c >>= 1;
		if (c == 0xe)
		{
			l = 3;
		}
		else
		{
			c >>= 1;
			if (c == 0x6)
			{
				l = 2;
			}
			else
			{
				l = 1;
			}
		}
	}
	return l;
}

unsigned int *ToUTF32(const unsigned char *utf8, int len)
{
	const unsigned char *p = utf8;
	unsigned int ch;
	int x = 0;
	int l;
	unsigned int *result = new unsigned int [len];
	unsigned int *r = result;
	if (!result)
	{
		OutOfMem(__FILE__, __LINE__);
	}
	while (*p)
	{
		l = UTF8Len(*p);

		switch (l)
		{
		case 4:
			ch = (*p ^ 0xf0);
			break;
		case 3:
			ch = (*p ^ 0xe0);
			break;
		case 2:
			ch = (*p ^ 0xc0);
			break;
		case 1:
			ch = *p;
			break;
		default:
			g_log<<"Len: "<<l<<std::endl;
		}
		++p;
		int y;
		for (y = l; y > 1; y--)
		{
			ch <<= 6;
			ch |= (*p ^ 0x80);
			++p;
		}
		print_char(x,l,ch);
		x += l;
		*r = ch;
		r++;
	}
	*r = 0x0;
	return result;
}

unsigned char *ToUTF8(const unsigned int *unicode)
{
	unsigned char *utf8 = NULL;
	const unsigned int *s = unicode;
	unsigned char *u;
	unsigned int ch;
	int x = 0;
	while (*s)
	{
		++s;
		++x;
	}
	if (x == 0)
	{
		return NULL;
	}
	utf8 = new unsigned char [x*4];
	if (!utf8)
		ERR("Out of memory");

	s = unicode;
	u = utf8;
	x = 0;

	while (*s)
	{
		ch = *s;
		if (*s < 0x80)
		{
			x = 1;
			*u = *s;
			u++;
		}
		else if (*s < 0x800)
		{
			x = 2;
			*u = 0xc0 | (ch >> 6);
			u++;
		}
		else if (*s < 0x10000)
		{
			x = 3;
			*u = 0xe0 | (ch >> 12);
			u++;
		}
		else if (*s < 0x200000)
		{
			x = 4;
			*u = 0xf0 | (ch >> 18);
			u++;
		}
		if (x > 1)
		{
			int y;
			for (y = x; y > 1; y--)
			{
				/*
				unsigned int mask = 0x3f << ((y-2)*6);
				*u = 0x80 | (ch & mask);
				*/
				*u = 0x80 | (ch & (0x3f << ((y-2)*6)));
				++u;
			}
		}
		++s;
	}
	return utf8;
}

void dump_unicode(unsigned char *buff, int len)
{
	unsigned int *result = ToUTF32(buff, len);
	dump_unicode_string(result);
	delete [] result;
}

void dump_unicode_string(unsigned int *str)
{
	unsigned int *s = str;
	while (*s)
	{
		printf("%li %lx\n", *s, *s);
		s++;
	}
	printf("\n");
}

void print_char(int pos, int len, unsigned int ch)
{
	char utf8[5];
	for(int i=0; i<4; i++)
		utf8[i] = ((char*)&ch)[i];
	utf8[4] = 0;
	//printf("Character: %i\tLength: %i\tUTF-32(hex): %lx\tUTF-32(dec): %li\n", pos, len, ch, ch);
	g_log<<"Character: "<<pos<<"\tLength: "<<len<<"\tUTF-8: "<<utf8<<"\tUTF-32(dec): "<<ch<<std::endl;
}

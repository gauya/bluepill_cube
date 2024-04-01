#include <stdint.h>
#include <string.h>
#include <ctype.h>

#ifndef __GSTR_H__
#define __GSTR_H__

void touppers(char *str);
void tolowers(char *str);
int strchri(const char *s,int ch);
const char* whitespace_skip(const char *b);
const char *space_skip(const char *b,const char *s);
int cmpstr(const char *s,const char *d);
int instrs(const char *s,const char **dest);
int stoi(const char *s);
double stof(const char *s);
int64_t stol(const char *s);
int str2arr(char *msg, char **dest, size_t sz, const char *dil);
int str2arri(const char *msg, int16_t *dest, size_t sz, const char *dil);
char *strnzcpy(char *s1, const char *s2, size_t n);
int chop_str(char *msg, char **dest, size_t num, const char *dil);

class gstr {
	char *_str;
	uint32_t _len;
	uint32_t _bsize;
public:
	gstr(const char*s=0);
	gstr(gstr &);
	virtual ~gstr();
	inline uint32_t len() const { return _len; };
	operator const char*() const;
	gstr& operator=(gstr &);
	gstr& operator=(const char*);
	bool operator==(gstr &) const;
	bool operator==(const char*) const;
	char& at(int);
	char& operator [](int);
	int comp (gstr&); // -1, 0, 1
	int comp (const char *); // -1, 0, 1
	gstr& append(gstr&);
	gstr& append(const char*);
	gstr& operator += (gstr&);
	gstr& operator += (const char*);
	friend gstr operator + (gstr&,gstr&);
	friend gstr operator + (gstr&,const char*);
	gstr& operator <<= (gstr&);
	gstr& operator <<= (const char*);
	void lstrip(const char* d=0);
	void rstrip(const char *d=0);
	gstr& cut(int,int); // from to
	gstr& remove(const char*);
	gstr& remove_all(const char*);
	gstr& replace(const char*, const char* to=0);
	gstr& replace_all(const char*, const char* to=0);
	gstr& lower();
	gstr& upper();
};

#endif // __GSTR_H__

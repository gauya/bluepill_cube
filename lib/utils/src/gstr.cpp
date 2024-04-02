/*
 * gstr.cpp
 *
 *  Created on: Dec 14, 2021
 *      Author: seu
 */

#include "gstr.h"
#include <ctype.h>
#include <stdexcept>
using namespace std;

const char __default_white_space[] = " \t\n\r";
const char __default_delimiter[] = " \t\n\r,.:/";

void touppers(char *str) {
	while(*str) {
		*str = toupper(*str);
		str++;
	}
}

void tolowers(char *str) {
	while(*str) {
		*str = tolower(*str);
		str++;
	}
}

const char* whitespace_skip(const char *str) {
	if( !str ) 
		return (const char*)0;
	
	while( *str && strchr(__default_white_space,*str) ) str++;
	
	return str;
}

const char *str_skip(const char *str,const char *skip) {
	if( !str ) 
		return (const char*)0;
	
	if(skip && *skip)
		while(strchr(skip,*str)) str++;
	
	return str;
}

// return index
int strchri(const char *s,int ch) {
	if(!s || !ch) return -1;
	for(int i=0;*s; i++,s++) {
		if(*s == ch) return i;
	}
	return -1;
}

int instrs(const char *s,const char **dest) {
	if(!s || !dest) return -1;

	for(int i=0;*dest;i++,dest++) {
		if( strcmp(s,*dest) == 0 )
			return i;
	}
	return -1;
}


#define isodigit(a) ((a) >= '0' && (a) <= '7')
#define isbdigit(a) ((a) == '0' || (a) == '1')

int stoi(const char *s) {
	s = whitespace_skip(s);
	if(!s) return 0;

	int minus = 1;
	uint32_t val = 0;

	if(*s == '-' || *s == '+')  {
		if(*s++ == '-') minus = -1;
	}

	for(;*s >= '0' && *s <= '9'; s++) {
		val = (val * 10) + (*s - '0');
	}
	//if( val & 0x80000000 ) // error

	return (minus ==-1)? -(int)val : (int)val;
}

int64_t stol(const char *s) {
	s = whitespace_skip(s);
	if(!s) return 0;

	int minus = 1;
	uint64_t val = 0;

	if(*s == '-' || *s == '+')  {
		if(*s++ == '-') minus = -1;
	}

	for(;*s >= '0' && *s <= '9'; s++) {
		val = (val * 10) + (*s - '0');
	}

	return (minus == -1)? -(int64_t)val : (uint64_t)val;
}

uint32_t stob(const char *s) {
	s = whitespace_skip(s);
	if(!s) return 0;

	if(*s == '\0') {
		s++;
		if(*s == 'b' || *s == 'B') s++;
	}

	uint32_t val = 0;
	const char *n = s;

	while(isbdigit(*n)) n++;
	n--;

	for(uint32_t u=0; n>=s; n--, u <<= 1) {
		if( *n == '1' ) val += u;
	}

	return val;
}

uint32_t stoo(const char *s) {
	s = whitespace_skip(s);
	if(!s) return 0;

	uint32_t val = 0;
	const char *n = s;

	while(isodigit(*n)) n++;
	n--;

	for(uint32_t u=0; n>=s; n--, u <<= 3) {
		val += (*n-'0') * u;
	}
	return val;
}

uint32_t stox(const char *s) {
	s = whitespace_skip(s);
	if(!s) return 0;

	if(*s == '\0') {
		s++;
		if(*s == 'x' || *s == 'X') s++;
	}

	uint32_t val = 0;
	const char *n = s;

	while(isxdigit(*n)) n++;
	n--;

	for(uint32_t u=0; n>=s; n--, u <<= 4) {
		val += (*n-'0') * u;
	}
	return val;
}

double stof(const char *s) {
	s = whitespace_skip(s);
	if(!s) return 0.;

	int minus = 1;
	double v = 0;

	if(*s == '-' || *s == '+')  {
		if(*s++ == '-') minus = -1;
	}

	for(;*s && (*s >= '0' && *s <= '9'); s++) {
		v = (v * 10) + (*s - '0');
	}

	if(*s++ == '.') {
		int c;
		double d=1.0;
		for(c = 0; *s >= '0' && *s <= '9'; s++,c++) {
			d *= 0.1;
			v += d * (*s - '0');
		}
	}

	return (minus == -1)? -v : v;
}

// return field number, -1 when error or not effective field
int chop_str(char *msg, char **dest, size_t num, const char *dil) {
	int cnt=0;

	while(*msg && cnt < (int)num) {
		msg = (char *)str_skip((const char*)msg, __default_white_space);
		if( *msg == '\0' ) break;

		dest[cnt++] = msg;
		while(*msg && strchri(dil,*msg) == -1 ) msg++; // skip context
		if( *msg == '\0' ) break; // end

		*msg++ = '\0'; // must be *msg is dil or white space
	}

	return cnt;
}

// s1 > (n+1)
char *strnzcpy(char *s1, const char *s2, size_t n)
{
	char *s = s1;
	while (n > 0 && *s2 != '\0') {
		*s++ = *s2++;
		--n;
	}
	while (n > 0) {
		*s++ = '\0';
		--n;
	}
	*s = '\0';
	return s1;
}


const char *get_token(const char *s, char *buf, size_t bsize) {
   size_t cp = 0;

   buf[0] = '\0';
   s = whitespace_skip(s);
   if( !s || !*s )
      return (const char *)0;

   while(*s) {
      if(isspace(*s)) {
         break;
      }
      buf[cp++] = *s++;
      if(bsize < (cp+2)) break;
   }
   buf[cp] = '\0';
   s = whitespace_skip(s);

   return (cp > 0)? s : 0;
}

/////////////////////////////////////////////////////////////////////////////////

gstr::gstr(const char *s) {
	// TODO Auto-generated constructor stub
	if(s) {
		_len = strlen(s);
		_str = new char[_len+1];
		if( !_str ) {
			// throw
		}
		strcpy(_str,s);
		_bsize = _len+1;
	} else {
		_str = 0;
		_len = 0;
		_bsize = 0;
	}
}

gstr::gstr(gstr &g) {
	// TODO Auto-generated constructor stub
	if(g.len() > 0) {
		_len = g.len();
		_str = new char[_len+1];
		if( !_str ) {
			// throw
		}
		strcpy(_str,g._str);
		_bsize = _len+1;
	} else {
		_str = 0;
		_len = 0;
		_bsize = 0;
	}
}

gstr::~gstr() {
	// TODO Auto-generated destructor stub
	if(_bsize > 0) {
		delete[] (_str);
	}
	_str = 0;
	_len = _bsize = 0;
}

gstr::operator const char*() const{
	return (const char*)_str;
}

gstr& gstr::operator=(gstr &g) {
	if( _bsize > 0 ) {
		delete[] _str;
	}
	if( g.len() > 0 ) {
		_str = new char[g.len()+1];
		_len = g.len();
		_bsize = g._len+1;
	} else {
		_str = 0;
		_len = 0;
		_bsize = 0;
	}
	return *this;
}
#include <string.h>

gstr& gstr::operator=(const char*s) {
	if( _bsize > 0 ) {
		delete[] _str;
		_bsize = _len = 0;
		_str = 0;
	}
	if( s ) {
		_len = strlen(s);
		_str = new char[_len+1];
		strcpy(_str,s);
		_bsize = _len + 1;
	} else {
		_str = 0;
		_len = 0;
		_bsize = 0;
	}
	return *this;
}
bool gstr::operator==(gstr &g) const {
	return (g.len() == len() && strcmp(g._str, _str) == 0)? true : false;
}

bool gstr::operator==(const char*s) const {
	return (strlen(s) == _len && strcmp(s, _str) == 0)? true : false;
}

char& gstr::at(int idx) {
	if( (uint32_t)idx < _len ) {
		return _str[idx];
	} else {
		//return _str[0];
		throw invalid_argument("MyFunc argument too large.");
		//		throw std::out_of_range("");
		//		throw std:: length_error(idx);
	}
}

char& gstr::operator [](int idx) {
	if( (uint32_t)idx < _len ) {
		return _str[idx];
	} else {
		throw std::out_of_range("");
	}
}

int gstr::comp (gstr& g) { // -1, 0, 1
	return strcmp(_str, g._str);
}

int gstr::comp (const char *s) { // -1, 0, 1
	return strcmp(_str,s);
}

gstr& gstr::append(gstr& g) {
	return append((const char*)g);
}

gstr& gstr::append(const char*s) {
	char *ns;
	uint32_t ln = strlen(s);
	if( ln == 0 ) return *this;

	int fb = _bsize - _len -1;
	if( fb < (int)ln ) {
		ns = new char[ln + len() + 1];
		strcpy(ns, _str);
		strcpy(ns+len(), s);

		if( _bsize > 0 )
			delete[] _str;
		_bsize = ln + len() + 1;
		_str = ns;
		_len += ln;
	} else {
		strcpy(_str + _len, s);
		_len += ln;
	}

	return *this;
}

gstr& gstr::operator += (gstr& g) {
	return append(g);

}

gstr& gstr::operator += (const char*s) {
	return append(s);
}

gstr& gstr::operator <<= (gstr& g) {
	return append(g);
}

gstr& gstr::operator <<= (const char* s) {
	return append(s);
}

gstr operator + (gstr& g1, gstr& g2) {
	gstr a = g1;
	a.append(g2);
	return a;
}

gstr operator + (gstr& g, const char* s) {
	gstr a(g);
	a.append(s);
	return a;
}

void gstr::lstrip(const char *d) {
	const char *s;
	if(!d) {
		s = whitespace_skip(_str);
	} else {
		s = str_skip(_str, d);
	}

	if( s != _str ) {
		// memcpy(
		uint32_t sz = (s - _str);
		for(int i=0; sz < _len; i++) {
			_str[i] = _str[sz++];
		}
		_str[sz] = '\0';
		_len -= (sz - 1);
		_bsize = 0;
	}
}

void gstr::rstrip(const char *d) {
	if( len() < 1 ) return;
	const char *p = (!d)? __default_white_space : d;
	for(; _len >= 0; _len--) {
		if( strchri(p,_str[_len]) == -1 ) {
			return;
		}
		_str[_len] = '\0';
		if(_bsize>=_len) _bsize = _len - 1;
	}
}

// strstr, strcasestr
gstr& gstr::remove(const char*ss) {
	if(!ss) return *this;
	char *s = strstr(_str,ss);
	if(!s) return *this;

	uint32_t ll=strlen(ss);

	strcpy(s, s+ll);
	_len -= ll;

	return *this;
}

gstr& gstr::remove_all(const char*ss) {
	if(!ss) return *this;
	char *s = strstr(_str,ss);
	if(!s) return *this;

	uint32_t ll=strlen(ss);

	do {
		strcpy(s,s+ll);
		s = strstr(s,ss);
	} while(s);

	_len = strlen(_str);

	return *this;
}

gstr& gstr::replace(const char*s1, const char*s2) {
	if(!s1) return *this;
	char *s = strstr(_str,s1);
	if(!s) return *this;

	if(!s2) return remove(s1);

	uint32_t l1=strlen(s1),l2=strlen(s2);

	auto scp = [](char*s,const char*s2){ while(*s2){ *s++ = *s2++; }; return s; };

	if( l1 == l2 ) {
		scp(s,s2);
		return *this;
	}

	if( l1 > l2 ) {
		const char *n = s + l1;
		s = scp(s,s2);
		s = scp(s,n);
		*s = '\0';

		_len -= (l1-l2);
		return *this;
	}

	char *nstr = new char[_len + l2 + 1];
	if(!nstr) return *this;

	char *np = nstr;
	strnzcpy(np,_str,(s - _str)); np += (s - _str);
	scp(np,s2); np += l2;
	strcpy(np,s+l1);

	if( _bsize > 0 ) delete _str;

	_bsize = _len + l2 + 1;
	_len = _len + (l2-l1);
	_str = nstr;

	return *this;
}

gstr& gstr::replace_all(const char*s1, const char*s2) {
	if(!s1) return *this;
	char *s = strstr(_str,s1);
	if(!s) return *this;

	if(!s2) return remove_all(s1);

	uint32_t l1=strlen(s1),l2=strlen(s2);

	auto scp = [](char*s,const char*s2){ while(*s2){ *s++ = *s2++; }; return s; };
	// l1 == l2
	if( l1 == l2 ) {
		scp(s,s2);
		return *this;
	}

	char *p1, *p2;
	// l1 > l2
	if( l1 > l2 ) {
		do {
			p2 = s + l2;
			p1 = s + l1;
			scp(s,s2);
			s = strstr(p1,s1);

		} while(s);
		strcpy(p2,p1);

		_len = strlen(_str);

		return *this;
	}

	char *nstr = new char[_len + l2 + 1];
	p2 = nstr;
	p1 = _str;
	int ln;
	do {
		ln = s-p1;
		strncpy(p2, p1, ln);
		s += l1;
		p1 = s;
		p2 += ln;
		strncpy(p2, s2, l2);
		p2 += l2;

		s = strstr(p1, s1);
	} while(s);
	if(!s) {
		strcpy(p2, p1);
	}

	if( _bsize > 0 ) delete _str;
	_bsize = _len + l2 + 1;
	_len = strlen(nstr);
	_str = nstr;

	return *this;
}

gstr& gstr::cut(int from, int to) {
	if( to < 0 ) to = _len - 1;
	if(from >= to || from < 0 || (uint32_t)to >= _len)
		return *this;

	strcpy(_str+from, _str+to+1);

	_len -= (to-from+1);

	return *this;
}

gstr& gstr::lower() {
	if( _len > 0 ) {
		for(uint32_t i=0;i<_len;i++) {
			_str[i] = tolower(_str[i]);
		}
	}
	return *this;
}

gstr& gstr::upper() {
	if( _len > 0 ) {
		for(uint32_t i=0;i<_len;i++) {
			_str[i] = toupper(_str[i]);
		}
	}
	return *this;
}

#ifdef GSTR_TEST

#include "gprintf.h"
void gstr_test() {
	gstr s0("I am a Boy");

}

#endif // GSTR_TEST
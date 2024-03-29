/*
 * gstr.cpp
 *
 *  Created on: Dec 14, 2021
 *      Author: seu
 */

#include "gstr.h"
#include <ctype.h>

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

// strcmp  (*s2 - *s1)
// cmpstr  0: equal, 1: s > d, 'd' is fully included in 's', -1: error or not include
int cmpstr(const char *s,const char *d) {
	if( !s || !d ) return -1;

	for(;*s == *d;s++,d++) {
		if( !*d ) return 0;
	}
	//	return ((uint8_t)(*s) < (uint8_t)(*d))? -1 : 1;
	return (*d)? -1 : 1;
}

int instrs(const char *s,const char **dest) {
	if(!s || !dest) return -1;

	for(int i=0;*dest;i++,dest++) {
		if( cmpstr(s,*dest) == 0 )
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

static const char __block_chars[] = "'\"(){}[]<>";
static const char __block_start_chars[] = "/'\"({[<";
static const char __block_end_chars[] = "'\")}]>*";

bool is_block_string(const char *str) {
	return (strchr(__block_chars, *str))? 1 : 0;
}

#if 0
typedef enum {
	e_block,
	e_op,
	e_num,
	e_name,
	e_comment,
	e_sepe,
	e_space,
	e_end = -1,
	e_err = -2
} parse_type_t;

typedef struct {
	parse_type_t	type;
	const char *str;
} fam_strs_t;

const fam_strs_t __fam_strs[] = {
	{ e_block, "\"\"\"" }, // python
	{ e_block, "\'\'\'" }, // python
	{ e_comment, "/*" },
	{ e_comment, "*/" },
	{ e_comment, "//" },
	{ e_op, "<<=" },
	{ e_op, ">>=" },
	{ e_op, ">>" },
	{ e_op, "<<" },
	{ e_op, "==" },
	{ e_op, "<=" },
	{ e_op, ">=" },
	{ e_op, "!=" },
	{ e_op, "&&" },
	{ e_op, "||" },
	{ e_op, "&=" },
	{ e_op, "|=" },
	{ e_op, "^=" },
	{ e_op, "!=" },
	{ e_op, "*=" },
	{ e_op, "/=" },
	{ e_op, "+=" },
	{ e_op, "-=" },
	{ e_op, "->" },
	{ e_op, "%=" },
	{ e_op, "~=" },
	{ e_op, "++" },
	{ e_op, "--" },
	{ e_block, "\"" },
	{ e_block, "\'" },
	{ e_block, "\n" },
//	{ e_comment, "#" },
	{ e_op, "!" },
	{ e_op, "~" },
	{ e_op, "!" },
	{ e_op, "=" },
	{ e_op, "+" },
	{ e_op, "-" },
	{ e_op, "*" },
	{ e_op, "/" },
	{ e_op, "%" },
	{ e_op, "^" },
	{ e_op, "|" },
	{ e_op, "&" },
	{ e_block, "(" },
	{ e_block, ")" },
	{ e_block, "[" },
	{ e_block, "]" },
	{ e_block, "{" },
	{ e_block, "}" },
	{ e_block, "<" },
	{ e_block, ">" },
	{ e_sepe, "," },
	{ e_op, "." },
	{ e_sepe, "@" },
	{ e_sepe, "$" },
	{ e_sepe, ";" },
	{ e_sepe, "#" },
	{ e_op, "?" },
	{ e_op, ":" },
	{ e_end, 0 }
};
#endif

int get_toktype(const char *s) {
	if(!s) return eERR;
	if(!*s) return eEND;
	if(isalpha(*s) || *s == '_') return eALPA;
	if(isspace(*s)) return eSPACE;
	if(isdigit(*s)) return eNUM;
	if(*s == '\\') {
		return eESC;
	}
	if(strchr("+-*/%|&^=<>!~",*s)) {
		switch(*s) {
		case '/':
			if( s[1] == '*' ) return eBLK;
			if( s[1] == '/' ) return eBLK;
			break;
		case '*':
			if( s[1] == '/' ) return eBLK;
			break;
		case '<': // <, <<, <<=, <=,
			if( s[1] != '=' && s[1] != '<' ) return eBLK;
			break;
		case '>': // >, >>, >>=, >=
			if( s[1] != '=' && s[1] != '>' ) return eBLK;
			// ref. prev and next token 
			break;
		case '-':
			if( s[1] != '>' ) return eDIL;
			break;
		default:;
		}
		return eOP;
	}
	if(is_block_string(s)) return eBLK;
	if(strchr("`:;.,?@#$_",*s)) return eDIL;
	return eERR;
}

block_type_t __block_types[] = {
	{ eWIDE_CONSTANT, "\"\"\"", "\"\"\"" },
	{ eWIDE_CONSTANT, "'''", "'''" },
	{ eCOMMENT, "/*", "*/" },
	{ eLINE_COMMENT, "//", "\n" },
//	{ eLINE_COMMENT, "#", "\n" },
//	{ eLINE_COMMENT, ";", "\n" },
	{ eCHAR_CONSTANT, "'", "'" },
	{ eCHAR_CONSTANT, "\"", "\"" },
	{ eBLOCK, "(", ")" },
	{ eBLOCK, "{", "}" },
	{ eBLOCK, "[", "]" },
	{ eBLOCK, "<", ">" },
	{ eNOTBLOCK,0,0 }

};

block_type_t *is_start_block(const char *str) {
	block_type_t *b = __block_types;

	if( !str || !*str ) return (block_type_t*)0;

	for( int i=0;b->etype != eNOTBLOCK; i++,b++) {
		if(cmpstr(str,b->start) >= 0) {
			return b;
		}
	}

	return (block_type_t*)0;
}

block_type_t *is_end_block(const char *str) {
	block_type_t *b = __block_types;

	if( !str || !*str ) return (block_type_t*)0;

	for( int i=0;b->etype != eNOTBLOCK; i++,b++) {
		if(cmpstr(str,b->end) >= 0) {
			return b;
		}
	}

	return (block_type_t*)0;
}

block_type_t *get_blktype(int e) {
	int i=0;
	while( __block_types[i].etype != eNOTBLOCK ) {
		if( __block_types[i].etype == e ) 
			return &__block_types[i];
		i++;
	}
	return (block_type_t*)0;
}

block_type_t *get_blktype(const char *s) {
	if( get_toktype(s) != eBLK ) return (block_type_t *)0;
	
	block_type_t *e = is_start_block(s);
	if(e) return e;

	return is_end_block(s);
}
#if 0
// return nextp, fill buffer except block string, 
const char *get_block(const char *str, char *buf, size_t bsize, block_info_t *info) {
	if(!str || !buf || bsize < 2)
		return str;

	buf[0] = '\0';

	const char *s = lookup_rblock(str, info->bt);

	info->len = (s - str);
	if( (size_t)info->len < bsize ) {
		strncpy(buf, str, info->len);
		buf[info->len] = '\0';
	}

	return s;
}
#endif
block_info_t get_block(const char **str, char *buf, size_t bsize, int step) {
	block_info_t bi = { 0,0 };

	if(!str || !buf || bsize < 2)
		return bi;

	if((bi.bt = is_start_block(*str)) == 0) 
		return bi;

	*str += strlen(bi.bt->start);

	buf[0] = '\0';
	const char *bs,*n;
	bs = *str;
//printf("begin step=%d bt(%d|%s)\n",step,bi.bt->etype,bi.bt->start);

	if(bi.bt->etype == eCHAR_CONSTANT) {
		const char *s = strchr(*str, '\n');
		n = (const char*)strstr(*str,bi.bt->end);
		if( s < n && s ) { n = s; }
		if( !n ) {
			n = *str + strlen(*str);
		} 
		*str = n + 1;
		bi.len = (n-bs);
		strncpy(buf,bs,bi.len);
		buf[bi.len] = '\0';

		return bi;
	} 

	if(bi.bt->etype == eLINE_COMMENT) {
		const char *s = strchr(*str, '\n');
		if( !s ) 
			bi.len = strlen(bs);
		else { 
			*str = s + strlen(bi.bt->end);
			bi.len = (s - bs);
		}
		strncpy(buf,bs,bi.len);
		buf[bi.len] = '\0';
		
		return bi;
	}

	if(bi.bt->etype == eWIDE_CONSTANT || bi.bt->etype == eCOMMENT) {
		const char *s = strstr(*str, bi.bt->end);
		if( !s ) 
			bi.len = strlen(bs);
		else {
			bi.len = (s - bs);
			*str = s + strlen(bi.bt->end);
		}
		strncpy(buf,bs,bi.len);
		buf[bi.len] = '\0';
		
		return bi;
	}

	block_info_t bis={0,0},bie={0,0};
// printf("start step=%d bt(%s) [%s]\n",step,bi.bt->start,*str);
	do {
#if 1		
		while(**str) {
			block_type_t *e = get_blktype(*str);
			if( !e ) {
//				printf("not block (%c)\n",**str);
			} else {
//				printf("ok block (%c-%d)\n",**str,e->etype);
			}
			if( e && e->etype == eBLOCK) break; 
			(*str)++; 
			}
#else
		while(**str) {
			if( get_toktype(*str) != eBLK ) break;
			(*str)++; 
			}
#endif
		if(!**str) { break; }

//printf("++++[%s]\n",*str);
		bis.bt=is_start_block(*str);
		if(bis.bt) {
			bis = get_block(str, buf, bsize, step+1);
			if( bis.len >= 0 ) {
			} else {
				// error
			}
			continue;
		} 
		bie.bt=is_end_block(*str);
		if(bie.bt) {
//printf("^^^^[%p/%p (%s/%s)] (%s)",bi.bt, bie.bt, bi.bt->end, bie.bt->end,*str);				
			if( bi.bt == bie.bt ) {
				bi.len = (*str - bs); 
				*str += strlen(bie.bt->end);
//printf("^^[ (%d/%d - <%s|%s>)]",bi.len,bsize, *str,bs);				
				if( (size_t)bi.len >= bsize ) { 
					bi.len = -bi.len;
					return bi; 
				}
				strncpy(buf, bs, bi.len);
				buf[bi.len] = '\0';

//				printf("[B%d<%s>]\n",step+1,buf);
				parse(buf, step+1);

				return bi; // return valid
			} else {
//printf("^^^^BAD conext ^^^^^^^^[%p/%p (%s/%s)] (%s)",bi.bt, bie.bt, bi.bt->end, bie.bt->end,*str);				
				// \n /* */
			}
		} else {
		}

		(*str)++;
	} while(**str);
	
	bi.len = (*str - bs);
	strncpy(buf, bs, bi.len);
	buf[bi.len] = '\0';

	return bi;
}

// return next strp, throw away useless garbage.
const char *get_parse_token(const char *s, char *buf, size_t bsize, ptoken_t *ptoken) {
	if( !s || !buf || bsize < 2) return 0;

	ptoken_t pt, *p;
	p = (ptoken)? ptoken : &pt;
	
	size_t blen = bsize;
	int nt=eEND;
	p->etype = 0; 
	p->subtype = 0;
	p->len = 0;

#define CHK_LEN do {\
		buf[p->len++] = *s++;\
		if(bsize < (size_t)(p->len+1)) { buf[p->len] = 0; return s; }\
} while(0)

	s = whitespace_skip(s);
	if( !s ) {
		return 0;
	}

	p->etype = get_toktype(s);
	if(p->etype == eEND || p->etype == eERR) return 0;
	buf[p->len++] = *s++; buf[p->len] = '\0'; blen--;

//printf("(%d)get_parse p->etype=%d(%c)\n",step,p->etype,buf[0]);	
	if(p->etype == eOP) {
		if(buf[0] == '+' || buf[0] == '-') { // effective decimal only, not hexa, bin, octal
			if(get_toktype(s) == eNUM) { // +-number
				p->etype = eNUM;
				return s;
//				return get_parse(s, buf, bsize, p);
			} else { // ++,+=, --, -=, ->
				if((buf[0] == '+' && (*s == '+' || *s == '=')) ||
						(buf[0] == '-' && (*s == '-' || *s == '=' || *s == '>'))) {
					CHK_LEN;
				}
				buf[p->len] = '\0';
				return s;
			}
		} else
		if(buf[0] == '<') { // <<, <=, <<=
			if( *s == '<' ) {
				CHK_LEN;
				if( *s == '=' ) {
					CHK_LEN;
				}
			} else
			if( *s == '=' ) {
					CHK_LEN;
			}
		} else
		if(buf[0] == '>') { // >>, >=, >>=
			if( *s == '>' ) {
				CHK_LEN;
				if( *s == '=' ) { // >>=
					CHK_LEN;
				}
			} else
			if( *s == '=' ) { // >=
				CHK_LEN;
			}
		} else
		if(buf[0] == '=') { // ==
			if( *s == '=' ) {
				CHK_LEN;
			}
		} else
		if(buf[0] == '!') { // !=
			if( *s == '=' ) {
				CHK_LEN;
			}
		} else
		if(buf[0] == '|') { // ||, |=
			if( *s == '=' || *s == '|' ) {
				CHK_LEN;
			}
		} else
		if(buf[0] == '&') { // &&, &=
			if( *s == '=' || *s == '&' ) {
				CHK_LEN;
			}
		} else
		if(buf[0] == '/') { // /*
			if( *s == '*' ) { // 
				CHK_LEN;
			}
		} else
		if(buf[0] == '*') { // */
			if( *s == '/' ) { // 
				CHK_LEN;
			}
		} else
		if(buf[0] == '~') { //
			if( *s == '=' ) { // ~=
				CHK_LEN;
			}
		} else
		if(buf[0] == '^') { // ^^,^=
			if( *s == '=' || *s == '^' ) {
				CHK_LEN;
			}
		}
		buf[p->len] = 0;
		return s;
	}

	if(p->etype == eESC) {
		if(*s == 'n' || *s == 'r' || *s == 't') {
			buf[p->len-1] = (*s=='n')? '\n' : (*s=='r')? '\r' : '\t';
		} else {
			buf[p->len-1] = *s;
		}
		buf[p->len] = '\0';
		return (s+1);
	}

	if(*(s-1) == '0') { // check next char, 0x, 0b, 0
		if(*s == 'b') { // bin 0b1023424
			CHK_LEN;
			while(*s == '0' || *s == '1') {
				CHK_LEN;
			}
			buf[p->len] = '\0';
			return s;
		}
		if(*s == 'x') { // hexa 0xf23a
			CHK_LEN;
			while(isxdigit(*s)) {
				CHK_LEN;
			}
			buf[p->len] = '\0';
			return s;
		}
	}

	if(p->etype == eNUM) {
		while(isdigit(*s)) {
			CHK_LEN;
		}
		if(*s == '.') {
			CHK_LEN;
			while(isdigit(*s)) {
				CHK_LEN;
			}
		}
		buf[p->len] = '\0';
		return s;
	}

	if(p->etype == eBLK) {
		block_type_t *bt = get_blktype(s-1);
		p->subtype = bt->etype;

		const char *tmps = lookup_rblock(s, bt);
		if( !tmps ) {
			return 0;  // context invalid
		}
		p->len = tmps - s;
		if(p->len != 0) { 
			strncpy(buf, s, p->len);
		}
		buf[p->len] = '\0';

		return tmps;
	}

	if(p->etype == eDIL) {
		buf[p->len] = '\0';
		return s;
	}

	while((nt=get_toktype(s)) == p->etype) {
		CHK_LEN;
	}

	// C name
	if( nt == eNUM && p->etype == eALPA ) {
		while(isdigit(*s) || get_toktype(s) == eALPA) {
			CHK_LEN;
		}
	}
	buf[p->len] = '\0';

	return whitespace_skip(s);
}
#undef CHK_LEN

const char *next_token(const char *s, ptoken_t *p) {
	if( !s || !*s || !p || p->bsize < 2) return 0;

	size_t blen = p->bsize;
	int nt=eEND;
	p->buf[0] = '\0';
	p->etype = 0; 
	p->subtype = 0;
	p->len = 0;

#define CHK_LEN do {\
		p->buf[p->len++] = *s++;\
		if(p->bsize < (size_t)(p->len+1)) { p->buf[p->len] = 0; return s; }\
} while(0)

	s = whitespace_skip(s);
	if( !s ) {
		return 0;
	}

	p->etype = get_toktype(s);
	if(p->etype == eEND || p->etype == eERR) return 0;
	p->buf[p->len++] = *s++; p->buf[p->len] = '\0'; blen--;

//printf("(%d)get_parse p->etype=%d(%c)\n",step,p->etype,buf[0]);	
	if(p->etype == eOP) {
		if(p->buf[0] == '+' || p->buf[0] == '-') { // effective decimal only, not hexa, bin, octal
			if(get_toktype(s) == eNUM) { // +-number
				p->etype = eNUM;
				return next_token(s, p);
			} else { // ++,+=, --, -=, ->
				if((p->buf[0] == '+' && (*s == '+' || *s == '=')) ||
						(p->buf[0] == '-' && (*s == '-' || *s == '=' || *s == '>'))) {
					CHK_LEN;
				}
				p->buf[p->len] = '\0';
				return s;
			}
		} else
		if(p->buf[0] == '<') { // <<, <=, <<=
			if( *s == '<' ) {
				CHK_LEN;
				if( *s == '=' ) {
					CHK_LEN;
				}
			} else
			if( *s == '=' ) {
					CHK_LEN;
			}
		} else
		if(p->buf[0] == '>') { // >>, >=, >>=
			if( *s == '>' ) {
				CHK_LEN;
				if( *s == '=' ) { // >>=
					CHK_LEN;
				}
			} else
			if( *s == '=' ) { // >=
				CHK_LEN;
			}
		} else
		if(p->buf[0] == '=') { // ==
			if( *s == '=' ) {
				CHK_LEN;
			}
		} else
		if(p->buf[0] == '!') { // !=
			if( *s == '=' ) {
				CHK_LEN;
			}
		} else
		if(p->buf[0] == '|') { // ||, |=
			if( *s == '=' || *s == '|' ) {
				CHK_LEN;
			}
		} else
		if(p->buf[0] == '&') { // &&, &=
			if( *s == '=' || *s == '&' ) {
				CHK_LEN;
			}
		} else
		if(p->buf[0] == '/') { // /*
			if( *s == '*' ) { // 
				CHK_LEN;
			}
		} else
		if(p->buf[0] == '*') { // */
			if( *s == '/' ) { // 
				CHK_LEN;
			}
		} else
		if(p->buf[0] == '~') { //
			if( *s == '=' ) { // ~=
				CHK_LEN;
			}
		} else
		if(p->buf[0] == '^') { // ^^,^=
			if( *s == '=' || *s == '^' ) {
				CHK_LEN;
			}
		}
		p->buf[p->len] = 0;
		return s;
	}

	if(p->etype == eESC) {
		if(*s == 'n' || *s == 'r' || *s == 't') {
			p->buf[p->len-1] = (*s=='n')? '\n' : (*s=='r')? '\r' : '\t';
		} else {
			p->buf[p->len-1] = *s;
		}
		p->buf[p->len] = '\0';
		return (s+1);
	}

	if(*(s-1) == '0') { // check next char, 0x, 0b, 0
		if(*s == 'b') { // bin 0b1023424
			CHK_LEN;
			while(*s == '0' || *s == '1') {
				CHK_LEN;
			}
			p->buf[p->len] = '\0';
			return s;
		}
		if(*s == 'x') { // hexa 0xf23a
			CHK_LEN;
			while(isxdigit(*s)) {
				CHK_LEN;
			}
			p->buf[p->len] = '\0';
			return s;
		}
	}

	if(p->etype == eNUM) {
		while(isdigit(*s)) {
			CHK_LEN;
		}
		if(*s == '.') {
			CHK_LEN;
			while(isdigit(*s)) {
				CHK_LEN;
			}
		}
		p->buf[p->len] = '\0';
		return s;
	}

	if(p->etype == eBLK) {
		block_type_t *bt = get_blktype(s-1);
		p->subtype = bt->etype;

		const char *tmps = lookup_rblock(s, bt);
		if( !tmps ) {
			return 0;  // context invalid
		}
		p->len = tmps - s;
		if(p->len != 0) { 
			strncpy(p->buf, s, p->len);
		}
		p->buf[p->len] = '\0';

		return (s + p->len + strlen(bt->end));
	}

	if(p->etype == eDIL) {
		p->buf[p->len] = '\0';
		return s;
	}

	while((nt=get_toktype(s)) == p->etype) {
		CHK_LEN;
	}

	// C name
	if( nt == eNUM && p->etype == eALPA ) {
		while(isdigit(*s) || get_toktype(s) == eALPA) {
			CHK_LEN;
		}
	}
	p->buf[p->len] = '\0';

	return s; //whitespace_skip(s);
}
#undef CHK_LEN

const char *lookup_rblock(const char *str, block_type_t *et) {
	//block_type_t *is_end_block(const char *str)
	// skip space
	// get_toktype -- ! BLK skip
	// start_block() --> lookup_rblock()
	// end_block() --> return

	if( !str || !et ) return 0;

	int ty;
	block_type_t *t;
	
	while(str && *str) {
		str = whitespace_skip(str);
		if(!str) break;
		ty = get_toktype(str);
		if( ty == eBLK ) {
			if(is_end_block(str) == et) {
				return str+strlen(et->end);
			}
			
			t = is_start_block(str);
			if( t ) {
				str = lookup_rblock(str+strlen(t->start), t);
			} else {
			}
		} else {
			str++;
		}
	}
	return str;
}

ptoken_t *get_ptoken(const char *str, ptoken_t *pt) {
	pt->bsize = strlen(str)+1;
	pt->buf = (char*)new char[pt->bsize];
	if( !pt->buf ) return 0;
	pt->etype = -1;
	pt->subtype = -1;
	pt->len = 0;

	return pt;
}

int parse(const char *s,int step) {
	if(!s) return -1;
	uint32_t cnt=0;

//	block_info_t bi;
	ptoken_t pt;
	get_ptoken(s, &pt);
	pt.step = step;

//printf("(%d)start parse len=%u\n",step,len+1);
	while(*s) {
		s = next_token(s, &pt);
		if(!s) break;
#if 0		
		if(pt.etype == eBLK && pt.subtype == eBLOCK) {
			bi = get_block(&s,pt.buf, pt.bsize, pt.step+1);
			pt.subtype = bi.bt->etype;
			pt.len = bi.len;
//			parse(s, pt.step+1);
		}
#endif			
		cnt++;
	}

	if(pt.bsize > 0) 
		delete[](pt.buf);

	return (int)cnt;
}

/////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus

#include <stdexcept>
using namespace std;

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

#endif // __cplusplus

#ifdef TEST
#include <malloc.h>
int ld_file(const char *filename, char**buf)
{
	int size = 0;
	FILE *f = fopen(filename, "rb");
	if (f == NULL)
	{
		*buf = NULL;
		return -1; // -1 means file opening fail
	}
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	*buf = (char *)malloc(size+1);
	if(!*buf) {
		fclose(f);
		return -3;
	}
	if (size != fread(*buf, sizeof(char), size, f))
	{
		free(*buf);
		*buf = 0;
		return -2; // -2 means file reading fail
	}
	fclose(f);
	return size;
}

int main(int argc,char **argv) {
	if(argc < 1) return 1;

	char *bf=0;
	int sz = ld_file(argv[1], &bf);
	printf("[%s] fsize=%d\n", argv[1],sz);
	if(sz < 0 || !bf) {
		return 1;
	}
	parse(bf,0);

	free(bf);
}

#endif // TEST

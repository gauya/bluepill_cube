#ifndef __GPARSE_H
#define __GPARSE_H

#include "gstr.h"

typedef enum {
	eEND = 0,
	eALPA,     // constant, name
	eNUM,      // constant, 			
	eOP,       // 
	eDIL,
	eSPACE,
	eESC,
	eBLK,
	eERR = -1
} etoken_type;

typedef enum {
	eBLOCK=0,
	eLINE_COMMENT,
	eCOMMENT,
	eWIDE_CONSTANT,
	eCHAR_CONSTANT,
	eNOTBLOCK = -1
} eblock_type;

typedef enum {
	eARITHMETIC, //arithmetic  - +, -, *, /, %, ++, --
	eRELETIONAL, //relational  - ==, !=, >, <, >=, <=
	eLOGICAL,    //logical     - &&, ||, !
	eASSIGNMENT, //Assignment  - =, +=, -=, *=, /=, %=, <<=, >>=, &=, ^=, |=
	eBITWISE,    //Bitwise     - |, &, ^, ~, <<, >>
	eSPEICIAL,   //Special     - &,*,sizeof
	eCONDITION  //Conditional - ?:
//	ePOSTFIX     //            - ., ->, :
} eop_type;

typedef struct {
	eblock_type etype;
	const char *start;
	const char *end;
} block_type_t;

typedef struct {
	block_type_t *bt;
	int len;
} block_info_t;

#if 0
/*
Postfix	() [] -> . ++ - -	Left to right
Unary	+ - ! ~ ++ - - (type)* & sizeof	Right to left
*/
typedef struct {
   eop_type etype;
   const char*opstr;
} eop_type_t;
#endif

typedef struct {
	char *buf;
	size_t bsize;
	int len;
	int etype; // token type
	int subtype;  // sub type
	int step;
} ptoken_t;


int get_toktype(const char *);
const char *get_parse(const char* s, char *buf, size_t blen,int step);
const char *next_token(const char* s, ptoken_t *p);
const char *lookup_rblock(const char *str, block_type_t *et);
block_type_t *is_start_block(const char *str);
int parse(const char *s,int step);
int parse_block(block_type_t *bt, const char **str, char *buf, size_t bsize);
block_info_t get_block(const char **s, char *buf, size_t bsize, int step);

block_type_t *get_blktype(const char *s);
block_type_t *get_blktype(int idx);
const char *get_token(const char *s, char *buf, size_t blen);
const char *get_parse_token(const char *s, char *buf, size_t bsize, ptoken_t *ptoken=0);

class gparse : public gstr {

};

#endif // __GPARSE_H
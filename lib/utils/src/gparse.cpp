#include "gparse.h"


static const char __block_chars[] = "'\"(){}[]<>";
static const char __block_start_chars[] = "/'\"({[<";
static const char __block_end_chars[] = "'\")}]>*";

bool is_block_string(const char *str) {
	return (strchr(__block_chars, *str))? 1 : 0;
}

#if 1
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

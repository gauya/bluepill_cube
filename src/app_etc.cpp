/*
 * app_etc.cpp
 *
 *  Created on: Mar 25, 2022
 *      Author: seu
 */
#include "app.h"
#include "gparse.h"

int add_proc(const char*pn, void (*f)(const char*), int timer, int act) { // when act = 0 and timer >= 0, the processor starts in a stopped state.
	int no=0;
	if( timer != -1 ) {
		no = add_pfn( timer, (void(*)())f, pn);
		if(act == 0) 
			pfn_stop(no);
	}
	if( pn && strlen(pn)>0 )
		set_tty_func(pn,f);

	return no;
}

void scadule_pre() {
	//
}

void scadule_post() {
	//
}

void command_list(const char*str) {
	// ls --> void tty_inner_cmd_list()
	// ls -f view_proc()
	char bf[32];
	str = get_token(str, bf, 32);
	if(!bf[0]) {
		tty_inner_cmd_list();
		return;
	}
	const char *cmds[] = { "-p","-a","-l",0 };
	switch( instrs(bf,cmds)) {
	case 1: case 2: // -a
		tty_inner_cmd_list();
	case 0: // -p
		view_proc_all();
		break;
	default:
		gdebug(2,0,"ls [-p | -a]\n");
	}
}

extern void view_proc_all();

void ps_ctrl(const char *s) {
  // ps 2
  // ps (all)
  // ps stop 2,4
  // ps start 0
  // ps frq 2.10   no 2 proc set frq = 10

  const char*cmds[] = { "stop","start","frq",0 };
  char buf[40],tb[10+1];
  int c,no,val;
  int dl=10;
  strcpy(buf,s);

  const char *p = get_token(buf,tb,10);
  if( p ) {
    c = instrs(tb,cmds);
    if( c >= 0 ) {
      p = get_token(p,tb,10);
      gdebug(dl,"1. c=%d tb=[%s]\n", c, tb);
      switch(c) {
        case 0: // stop
          if(tb[0]) {
            val = stol(tb);
            pfn_stop(val);
      gdebug(dl,"2. val=%d tb=[%s]\n", val, tb);
          } else { // error

          }
          break;
        case 1: // start
          if(tb[0]) {
            val = stol(tb);
            pfn_start(val);
      gdebug(dl,"3. val=%d tb=[%s]\n", val, tb);
          } else { // error

          }
          break;
        case 2: // frq
          if(tb[0]) {
            no = stol(tb);
            p = get_token(p,tb,10);
            if(p && tb[0]) {
              val = stol(tb);
              pfn_frq(no,val);
      gdebug(dl,"4. val=%d tb=[%s]\n", val, tb);
            } else {
              // error
      gdebug(dl,"4-1. tb=[%s]\n", tb);
            }

          }
          break;
        default:;
      }
      return;
    } else { // numeric or error
      // ps 2
    }
  } else {
    if(tb[0]) {
      view_proc(stol(tb));
      return;
    }
  }
  
  view_proc_all();
}

#if 0

uint32_t suspicious_looptime = 1000 * 10; // 10ms

void scadule_loop_check() {
	if(get_scadule_laptime() > suspicious_looptime) {
		// stop current proc
		gpfn_t *g = get_curpfn();
		if(g) {
			gdebug(0,0,"\n!!! (%d)[%s]\n",g->no,(g->pname)? g->pname : "");
		}
	}
}

void scadule_traffic(const char *str) {
	scadule_info_t *sit = get_scadule_inf();
	if( !sit ) return;

	int d,h,m,s;
	uint32_t u = get_stime();
	s = u % 60; u -= s;
	m = (u % 3600) / 60; u -= m*60;
	h = (u % (24 * 3600)) / 3600; u -= (h * 3600);
	d = u / (3600 * 24);

	gdebug(2,0,"load %.1lfus (%.1lf ~ %.1lf)us ps=%d elapsed=%dday %02d.%02d.%02d\n"
			, sit->avg_long.v
			, sit->avg_long.max_v,sit->avg_long.min_v
			,sit->_runs
			,d,h,m,s
			);
// clear data,get_scadule_laptime()
	sit->avg_long.max_v = DBL_MIN;
	sit->avg_long.min_v = DBL_MAX;
}
#endif

void system_reset(const char *s) {
	NVIC_SystemReset();
}

int tty_direct_func(int ch) {
	switch(ch) {
	case 'l':
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
		break;
	default:
		  break;
	}
	return ch;
}

int tty_prev_func(int c) {
//	tty_func_key(c);
	return 0;
}



static uint32_t factorial(uint32_t num) {
	uint32_t v=num;
	for(int i=num-1; i > 0 ; i--) {	v += i; }
	return v;
}

double tinycalc(const char *str) {
	ptoken_t pt;
	char buf[128];
	char ops[8];

	double vr=0., v=0.;

	auto help = [] () {
		gdebug(log_level(),"use operator, [+-*/%!^()]\n"); };

	str = get_parse_token( str, buf, sizeof(buf)-1, &pt);
	if(!str) return v;

	if(pt.etype == eBLK && pt.subtype == eBLOCK) {
		v = tinycalc(buf);
	} else if(pt.etype == eNUM) {
		v= stof(buf);
	} else {
		if( buf[0] == '-' && *str == 'h') help();
		return v;
	}

	while(*str) {
		str = get_parse_token( str, buf, sizeof(buf)-1, &pt);
		if(pt.etype != eOP) {
			if(pt.etype == eNUM) v += stof(buf);
//			help();
		}
		if(!str) return v;

		if(buf[0] == '!')
			return factorial(v);
		strcpy(ops,buf);

		str = get_parse_token( str, buf, sizeof(buf)-1, &pt);
		if(!str) break;

		if(pt.etype == eBLK && pt.subtype == eBLOCK) {
			vr = tinycalc(buf);
		} else if(pt.etype == eNUM) {
			vr = stof(buf);
		} else {
//			help();
			return v;
		}

		switch(ops[0]) {
		case '+':
			v += vr;
			break;
		case '-':
			v -= vr;
			break;
		case '*':
			v *= vr;
			break;
		case '/':
			v /= vr;
			break;
		case '^':
			v = pow(v,vr);
			break;
		case '%':
			v = (double)((uint32_t)v % (uint32_t)vr);
			break;
#if 0
		case '&':
			;
			break;
		case '|':
			op = 10;
			break;
#endif
		default:;
		break;
		}
	}

	return v;
}

#ifndef ISFLOAT
#define ISFLOAT(v) (((v)-((int64_t)(v))) != 0.)
#endif

void dis_tinycalc(const char *str) {
   double v = tinycalc(str);

   if( ISFLOAT(v) ) {
      gprintf("%s = %f\n", str, v);
   } else {
      gprintf("%s = %ld\n", str, (long)v);
   }
}


#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif


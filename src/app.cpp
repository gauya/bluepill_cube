#include <gproc.h>
#include <gtick.h>
#include <gtick_stm32_hal.h>
#include <glog.h>
#include <gadc.h>
#include <gtty.h>
#include <stdio.h>
#include "app_etc.h"
#include <giwdg.h>
#include <ggpio.h>
#include <gtimer.h>

ggpio gLED(GPIOC,13); 

void loop_led() {
  static int sw=0,cnt=0;
  gLED.toggle();

  //digitalWrite(ledPin, sw);

  gdebug(2,"[%d]\n",cnt++);
  sw = !sw;
}

void test1() {
  gdebug(3,".");
}

void test2() {
  static int c = 0;
  uint32_t d,t = get_stime();
  int h,m,s;
  d = t;
  s = d % 60; d /= 60;
  m = d % 60; d /= 60;
  h = d % 24; d /= 24;
  gdebug(2,"test2 cnt = %d : elapsed %ld sec %d.%d.%d.%d]\n",c++, t,d, h,m,s);
}

int timer_cnt=0;
void timer_func(TIM_HandleTypeDef *h) {
  timer_cnt++;
}

gcavg avg();  // <-----------
gtimer *gt=0;

// ADC_TEST=1 : dma, else : polling
#define ADC_TEST  0

#if 0 // blackpill
#define VREFINT 1.21
#define REFVOL 3.3  // typical voltage
#define ADCMAX 4095.0
#define V25 0.76        // Voltage at 25C
#define AVG_SLOPE 0.0025 // 2.5mV/C

#else // bluepill

#define VREFINT 1.20
#define REFVOL 3.3  // typical voltage
#define ADCMAX 4095.0
#define V25 1.43        // Voltage at 25C
#define AVG_SLOPE 0.0043 // 4.3mV/C
#endif

void adc_temp_vref(uint16_t t, uint16_t v) {
    double VrefInt = (VREFINT * ADCMAX) / v;
    //double Vtmpsens = (VREFINT * v) / ADCMAX;
    double Vtmpsens = REFVOL * (((double)t) / ADCMAX);
//    double Temperature = (Vtmpsens - V25) * 0.1 / AVG_SLOPE + 25.0; // 200/.76
    double Temperature = (V25 - Vtmpsens) * 0.1 / AVG_SLOPE + 25.0; // 200/.76

    float temperature = ((float)t * 3.3f) / 4095.0f * 110.0f;

    float v_measured = t * 3.3 / (float)(4095);
    float temperature2 = (v_measured - V25) / AVG_SLOPE + 25.0f;
    float temperature3 = (V25 - (float)v_measured) * AVG_SLOPE + 25.0f;

    //t = (float)adcVal[4] * 3.3 / 0xfff; // 읽은 센서값을 전압으로 변경
    //t = (1.43 - t) / 0.0043 + 25.0; // 슬로프오 오프셋을 계산    

    gdebug(2,"t/v[ %d,%d ]  Vref = [%.2fV]      Temp = [%.2fC] [%d][%d]\n",t,v,VrefInt, Temperature, gt->cnt(),timer_cnt);
}

#if (ADC_TEST==1) 

stm32adcdma adc;
int dma_completed;

void adc_cb() {
  dma_completed++;
}

void testadc() {
  uint16_t val[18] = {0,};

  if(dma_completed > 0) {
    adc.read(val,adc.channel_nr());
    //gdebug(2,"adc %8d %5d %5d %5d %5d %5d %5d %5d %5d\n", dma_completed, val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7]);
    gdebug(2,"adc %8d", dma_completed);
    for( int i=0; i < adc.channel_nr(); i++ ) {
      gdebug(2,"%6d", val[i]);

    }
    gdebug(2,"\n");
    adc_temp_vref(val[5], val[6]);
    dma_completed = 0;
  } else {
    gdebug(2,"adc not completed\n");
  }
}
#else

stm32adc adc;
//uint16_t val[6];
void testadc() {
  uint16_t val[20] = {0,};
  //for(int i=0;i<6;i++) val[i] = 0;

  adc.read(val);

  gdebug(2,"adc read(%d)   : ",adc.channel_nr());
  for( int i=0; i < adc.channel_nr(); i++ ) {
    gdebug(2,"%u ", (val[i] & 0xfff));
  }
  gdebug(2,"\n");
  adc_temp_vref(val[5], val[6]);
}

#endif

void test4() {
//  int val = Serial.read();
//  if( val == -1 ) return;
//  Serial.write(val); // echo
}

void test5(const char*s) {
  view_proc(2);
}

void test6() {
  gdebug(2,"-------------------\n");
}

#include <gstr.h>
extern void view_proc_all();

void ps(const char *s) {
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
            pfn_stop(val,1);
      gdebug(dl,"2. val=%d tb=[%s]\n", val, tb);
          } else { // error

          }
          break;
        case 1: // start
          if(tb[0]) {
            val = stol(tb);
            pfn_stop(val,0);
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

void strtest(const char*str, const char* ds, const char* ss) {
  if(!ds) ds = __default_delimiter;
  if(!ss) ss = __default_white_space;
  int dl=10;

  int token_cnt = 0;
  int ssz = strlen(str);

  char *buf = new char[ssz*2+1];
  char **toks = new char*[ssz];

  if( !buf || !toks ) {
    gdebug(10,"!!!!!!!!! error [new] %d\n",ssz);
    return;
  }
  char *tbp = buf;
  const char *tp=str;

  while(tp) {
    tp = get_token(tp,tbp, ssz*2);
    toks[token_cnt++] = tbp;
    if(!*tbp || !tp) break;

    gdebug(dl,"tc=%d tbp=[%s]\n", token_cnt,tbp);

    while(*tbp) tbp++;
    tbp++;

  }

  gdebug(dl,"msg = [%s]\n\ntotal tokens = %d\n", str, token_cnt);
  for(int i=0; i < token_cnt; i++ ) {
    gdebug(dl,"%2d : [%s]\n", i, toks[i]);
  }
  delete[] buf;
  delete[] toks;
}

void strtestf(const char*str) {
  strtest(str," .:",0);
}

void rtled() {
  static int f=0;
  f++;
  if(!(f / 10)) {
    gLED.toggle();

  }
}

void cli_test2(const char*s) {
  test2();
}

extern UART_HandleTypeDef huart1;

char tmpbuf[] = "Hello!! good morning\n";
void setup() {
  // set the digital pin as output:

  init_serial(&huart1);
  log_level = 2;
  
gdebug(2,"start\n");

  #if 0 
  //HAL_UART_Transmit(get_console()->Handle, (uint8_t*)tmpbuf,21,HAL_MAX_DELAY) ;
  //gputs(tmpbuf);
  gdebug(2,tmpbuf);

  int ch;
  while(1) {
    ch = ggetc();
    if( ch <= 0 ) continue;
  }
  //printf("Hello");
  #endif

  gLED.init();

gdebug(2,"led init\n");

  adc_channels ac[] = {
    { ADC_CHANNEL_4, ADC_SAMPLETIME_55CYCLES_5,GPIOA, 4, }, // ADC_SAMPLETIME_56CYCLES
    { ADC_CHANNEL_0, ADC_SAMPLETIME_55CYCLES_5,GPIOA, 0,},
    { ADC_CHANNEL_1, ADC_SAMPLETIME_55CYCLES_5,GPIOA, 1,},
    { ADC_CHANNEL_2, ADC_SAMPLETIME_55CYCLES_5,GPIOA, 2,},
    { ADC_CHANNEL_3, ADC_SAMPLETIME_55CYCLES_5,GPIOA, 3,},
    { ADC_CHANNEL_TEMPSENSOR, ADC_SAMPLETIME_55CYCLES_5,0, 0, },
    { ADC_CHANNEL_17, ADC_SAMPLETIME_55CYCLES_5,0, 0, },
    { -1,0 }
  };

#if (ADC_TEST==1)  
  adc.setup(ADC1,ac,0,0);
  adc.attach(adc_cb);
  adc.start();
#else
  //  adc.setup(ADC1,ac);
#endif
  //gt = new gtimer(TIM3,1,7000,0,timer_func);
  //gt->start();

  gputs("1. adc setup\n");

  init_ticks(eTICK_VOL_100us);
  init_pfn();

  gputs("2. init ticks\n");

  set_tty_func("ps",ps );
  set_tty_func("time",cli_test2);
  set_tty_func("str",strtestf);

  gputs("3. tty funcs\n");

  add_pfn(1000, loop_led, "led blink");
  add_pfn(100,test1,"N1");
  add_pfn(10*1000,test2);
  add_pfn(5200, testadc,"adc read");
  add_rtpfn(15,rtled);
  add_pfn(10000,view_proc_all);
  add_pfn(0,tty,"key in");
  add_pfn(10000, test6, "elapsed test");
  add_proc("check pr", test5, 2, 0);
  
  //timer.setPrescaleFactor(1);
  //timer.setCaptureCompare(1);
  //timer.attachInterrupt(timer_func);
  //timer.resume();

  scadule();
}

void loop() {}

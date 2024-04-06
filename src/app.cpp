
#include <app.h>
#include <gadc.h>
#include <giwdg.h>
#include <ggpio.h>
#include <gtimer.h>
#include "gflash.h"

void loop_led() {
  static ggpio gled(GPIOC,13); 
  static int cnt=0;
  gled.toggle();

  gdebug(5,"[%d]\n",cnt++);
}

void flash_test();

__IO int timer_cnt=0;
void timer_func(TIM_HandleTypeDef *h) {
  timer_cnt++;
}

// ADC_TEST=1 : dma, else : polling
#define ADC_TEST  0

#if defined STM32F4 // blackpill
#define VREFINT 1.21
#define REFVOL 3.3  // typical voltage
#define ADCMAX 4095.0
#define V25 0.76        // Voltage at 25C
#define AVG_SLOPE 0.0025 // 2.5mV/C

#elif defined STM32F1 // bluepill startup 4~10us, sampling 17~ us

#define VREFINT 1.20
#define REFVOL 3.3  // typical voltage
#define ADCMAX 4095.0
#define V25 1.43        // Voltage at 25C
#define AVG_SLOPE 0.0043 // 4.3mV/C

#endif // STM32F4, STM32F1

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern int adc_completed;
extern int dma_finish1;
extern int dma_finish2;
extern int adc_mode;

#if 0
#define DMA_BUFFER_SIZE 7
uint16_t adc_buffer[DMA_BUFFER_SIZE*2];

void test_adc() {
    if( adc_mode != 1 ) {
      if(HAL_ADC_Start(&hadc1) != HAL_OK) {
        error_log("adc start");
      };
      HAL_Delay(1);
    }
    if( adc_completed || dma_finish1 || dma_finish2 ) {
      for( int i=0;i < DMA_BUFFER_SIZE; i++ ) {
        gdebug(2,"%5d ",adc_buffer[i]);
      }
      float temperature = (adc_buffer[0] / 4095.0) * 3.3;
//      float temperature = (adc_buffer[0] * 4095.0) / 3.3;
            temperature = (temperature - V25) / AVG_SLOPE + 25.0;
      gdebug(2, " (%d,%d,%d  (%d) %.3fC)\n",adc_completed,dma_finish1,dma_finish2, timer_cnt, temperature);
      adc_completed = dma_finish1 = dma_finish2 = timer_cnt = 0;
  }
}
#endif

#ifdef GSTR_TEST
  extern void gstr_test();
#endif

void test0(const char *msg) 
{
  flash_test();
#ifdef GSTR_TEST
  gstr_test();
#endif
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

gcavg avg();  // <-----------
gtimer *gt=0, *gt3=0;

void adc_temp_vref(uint16_t t, uint16_t v) {
    if(v==0) v=1;
    double VrefInt = (VREFINT * ADCMAX) / v;

    double Vsense = (VrefInt * t) / ADCMAX;
    double Temperature = (Vsense - V25) / AVG_SLOPE + 25.0; // 

    gdebug(2,"t/v[ %d,%d ]  Vref = [%.2fV]      Temp = [%.2fC] [%d][%d] ",t,v,VrefInt, Temperature, gt->cnt(),timer_cnt);
    gdebug(2,"(%d,%d,%d)\n",adc_completed,dma_finish1,dma_finish2);
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
    //adc.read(val);
    //gdebug(2,"adc %8d %5d %5d %5d %5d %5d %5d %5d %5d\n", dma_completed, val[0], val[1], val[2], val[3], val[4], val[5], val[6], val[7]);
    gdebug(2,"adc %8d", dma_completed);
    for( int i=0; i < adc.channel_num(); i++ ) {
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

extern gadc __adc1;

//uint16_t val[6];
void testadc() {
  uint16_t val[20] = {0,};
  //for(int i=0;i<6;i++) val[i] = 0;

  __adc1.read(val);

  gdebug(2,"adc read(%d)   : ",__adc1.channel_num());
  for( int i=0; i < __adc1.channel_num(); i++ ) {
    gdebug(2,"%u ", (val[i] & 0xfff));
  }
  gdebug(2,"  ");
  adc_temp_vref(val[5], val[6]);

  // test
  adc_completed = dma_finish1 = dma_finish2 = 0;
}

#endif // ADC_TEST

void test4() {
}

void test5(const char*s) {
  view_proc(2);
}

void test6() {
  gdebug(2,"-------------------\n");
}


void rtled() {
  static int f=0;
  f++;
  if(f > 200) {
    f=0;
  //  gled.toggle();
  }
}

void cli_test2(const char*s) {
  test2();
}

extern void pendmain(void);

ggpio et;
uint32_t _uptime, _etime, _rise=0,_fall=0;
void etfunc(uint16_t pin) 
{
  int st = et.read();
  if( st == 1 ) { // high, it is rised
    _uptime = get_utime();
    _rise++;
    _fall = 0;
//    gdebug(2,"rise\n");
  } else 
  if( st == 0 ) { // its falled
    if( _rise > 0 ) {
      _etime = elapsed_us(_uptime);
      gdebug(2,"fall %d rise=%d _fall=%d\n", _etime,_rise,_fall);
      _rise=0;
    } else {
      _fall++;
    }
  }
}

void setup() {
  init_serial(115200);

  init_ticks(eTICK_VOL_100us);
  init_proc();

// ---------------------------------------------------------
  log_level(2);
  
  error_log("start");

  gt = new gtimer(TIM2,625,127,timer_func);
  gt->start();
  gpio_t pwmg1 = {GPIOA,3};
  gt->pwm(4,300,TIM_OCMODE_PWM1,&pwmg1);
  gt->pwm_start(4);

  gt3 = new gtimer(TIM3,625,127,0);
  gt3->start();
  gpio_t pwmg2 = {GPIOB,1};
  gt3->pwm(4,300,TIM_OCMODE_PWM1,&pwmg2);
  gt3->pwm_start(4);

  //pendmain();

  et.init(GPIOB,5,eGPIO_EXTI_RISING_FALLING); //eGPIO_EXTI_FALLING); //eGPIO_EXTI_RISING_FALLING);
  et.attach(etfunc,0);

#if ADC_TEST
  adc_cube_start();
  __disable_irq();
  if( HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK ) {
    error_log("adc calibration fail");
  }
  __enable_irq();
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, DMA_BUFFER_SIZE);
#else
  adc_channels ac[] = {
    { ADC_CHANNEL_0, ADC_SAMPLETIME_55CYCLES_5,GPIOA, 0,},
    { ADC_CHANNEL_1, ADC_SAMPLETIME_55CYCLES_5,GPIOA, 1,},
    { ADC_CHANNEL_2, ADC_SAMPLETIME_55CYCLES_5,GPIOA, 2,},
    { ADC_CHANNEL_4, ADC_SAMPLETIME_55CYCLES_5,GPIOA, 4, }, // ADC_SAMPLETIME_56CYCLES
    { ADC_CHANNEL_TEMPSENSOR, ADC_SAMPLETIME_239CYCLES_5,0, 0, },
    { ADC_CHANNEL_17, ADC_SAMPLETIME_239CYCLES_5,0, 0, },
    { -1,0 }
  };

  __adc1.setup(ADC1,ac);
//  gadc adc(ADC1, ac);
  __adc1.start();
#endif

  set_tty_func("ps",ps_ctrl );
  set_tty_func("time",cli_test2);
  set_tty_func("ls",command_list);
  set_tty_func("test",test0);
  set_tty_func("ca",dis_tinycalc);

  add_pfn(1000, loop_led, "led blink");
  add_pfn(100,test1,"N1");
  add_pfn(10*1000,test2);
  add_pfn(1200, testadc,"adc read");
  add_rtpfn(15,rtled);
  add_pfn(0,tty,"key in");
  add_pfn(10000, test6, "elapsed test");
  
  add_proc("viewall", (void (*)(const char *))view_proc_all, 60000, 0);
  add_proc("check", test5, 20000, 0);
  
  scadule();
}

void loop() {}

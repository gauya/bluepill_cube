#include "app.h" 
#include "gtimer.h"
#include "gproc.h"
#include <math.h>
#include <stdlib.h>

/*
sg90
period : 20msec(50 hz)
-90 : 1.0 msec
  0 : 1.5 msec pulse 
+90 : 2.0 msec

-------------------------
injs025, injs035
0 pos : 1500us, 330hz, 20msec
4.8V ~ 6V
60g metal
90 - 500us ~ 2500us
dead band 4us
*/

// timer 50hz
class servo : gtimer {
    void igo(float a);
public:    
    float _cur_angle;
    float _margin,_minl,_maxr; // 
    int _channel;
    float _per_angle;
public:
    // constructor
    servo(TIM_TypeDef *tim, int channel, gpio_t *g);
    ~servo();

    void angle(float a);// -90 ~ +90
    void iangle(float i);  // +- increment
};

servo :: servo(TIM_TypeDef *tim, int channel, gpio_t *g) 
    : gtimer(tim,1270,1250), _channel(channel) {
    _margin = 0.3;
    _minl = -90.0;
    _maxr = 90.0;
    _per_angle = 90. / 500.;

    this->pwm(channel,0,0,g);

    //this->pwm_start(_channel)

}

void servo::angle(float a) 
{
    if( a < _minl ) a = _minl;
    else if( a > _maxr) a = _maxr;
    if( fabs(a - _cur_angle) < _margin ) return;

    _cur_angle = a;

    float val = 1500 + (_per_angle * a);
    this->ccr(_channel,val);
    this->pwm_start(_channel);
}

void servo::iangle(float i) 
{
    if( fabs(i) < _margin ) return;

    float n = i + _cur_angle;
    if( n < _minl ) n = _minl;
    else if( n > _maxr) n = _maxr;
    
    igo(n);
}

void servo::igo(float n) 
{
    _cur_angle = n;
    float val = 1500 + (_per_angle * n);
    this->ccr(_channel,val);
    this->pwm_start(_channel);
}

// no timer

class servo2 {
    timed_t _mpd, _spd;
  
public:
    servo2();
    ~servo2();
};

servo2::servo2() {
    _mpd = {20,};
}
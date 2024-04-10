
/*
 * gutils.h
 *
 *  Created on: 2021. 10. 15.
 *      Author: seu
 */

#ifndef GUTILS_H_
#define GUTILS_H_

#ifndef ABS
#define ABS(x)        ( ((x)<0)?-(x):(x) )
#endif

#ifndef MAX 
#define MAX(x,y)      ( (x)>(y)?(x):(y) )
#endif

#ifndef MIN
#define MIN(x,y)      ( (x)<(y)?(x):(y) )
#endif

#ifndef BIT
#define BIT(n) (1 << (n))
#endif

#ifndef BIT_ISTRUE
#define BIT_ISTRUE(b,mask)  ((b) & (mask))
#endif

#ifndef BIT_SET
#define BIT_SET(b,mask)     (b) |= (mask)
#endif

#ifndef BIT_RESET
#define BIT_RESET(b,mask)   (b) &= ~(mask)
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
	uint16_t wD;
	struct {
		uint16_t	BT0:1;
		uint16_t	BT1:1;
		uint16_t	BT2:1;
		uint16_t	BT3:1;
		uint16_t	BT4:1;
		uint16_t	BT5:1;
		uint16_t	BT6:1;
		uint16_t	BT7:1;
		uint16_t	BT8:1;
		uint16_t	BT9:1;
		uint16_t	BT10:1;
		uint16_t	BT11:1;
		uint16_t	BT12:1;
		uint16_t	BT13:1;
		uint16_t	BT14:1;
		uint16_t	BT15:1;
	};
} u16bits_t;


typedef union {
	uint32_t wD;
	struct {
		uint32_t	BT0:1;
		uint32_t	BT1:1;
		uint32_t	BT2:1;
		uint32_t	BT3:1;
		uint32_t	BT4:1;
		uint32_t	BT5:1;
		uint32_t	BT6:1;
		uint32_t	BT7:1;
		uint32_t	BT8:1;
		uint32_t	BT9:1;
		uint32_t	BT10:1;
		uint32_t	BT11:1;
		uint32_t	BT12:1;
		uint32_t	BT13:1;
		uint32_t	BT14:1;
		uint32_t	BT15:1;
		uint32_t	BT16:1;
		uint32_t	BT17:1;
		uint32_t	BT18:1;
		uint32_t	BT19:1;
		uint32_t	BT20:1;
		uint32_t	BT21:1;
		uint32_t	BT22:1;
		uint32_t	BT23:1;
		uint32_t	BT24:1;
		uint32_t	BT25:1;
		uint32_t	BT26:1;
		uint32_t	BT27:1;
		uint32_t	BT28:1;
		uint32_t	BT29:1;
		uint32_t	BT30:1;
		uint32_t	BT31:1;
	};
} u32bits_t;

int bit_count(uint32_t word);

// max > min
typedef struct {
	double	max; //
	double	min; //
} pg_t;

typedef struct {
	uint32_t cb,n;
	double  v;
} gavg_t;


double topg( pg_t *p, double v );
int toph( pg_t *p, double v );

uint32_t dif_u32(uint32_t s,uint32_t e);
uint32_t dif_u32_limit(uint32_t s,uint32_t e,uint32_t max);

double gavg(gavg_t *k, double nv );
void set_gavg(gavg_t *k, uint8_t no);
double last_gavg(gavg_t *a);

#ifdef __cplusplus
}
#define DEF_GAVG_CALIBRATION 10

template <typename T,typename N>
struct gavg_tt{
	N cb,n;
	T v; 
};
typedef gavg_tt<double,uint32_t> gavgdu_t;

template<typename T, typename N>
class gtg : public gavg_tt<T,N> {
	//gavg_tt<T,N> _d;
protected:
	using gavg_tt<T,N>::v;
	using gavg_tt<T,N>::cb;
	using gavg_tt<T,N>::n;
public:
	gtg(N cs = DEF_GAVG_CALIBRATION);
	~gtg(){};

	inline T get() const { return this->v; }
	T put( T val );
	void width(N nb);
	T operator=(T val) { return put(val); }
	inline operator T() const { return get(); }
};

template<typename T, typename N>
T gtg<T,N>::put( T val ) {
    v = ( (v * n) + val) / (n + 1);
    if( n < cb ) n++;
}

template<typename T,typename N>
void gtg<T,N>::width( N b ) {
	cb = b;
	if( cb < n ) n = cb;
}

typedef gtg<double,uint8_t> gcavg;
typedef gtg<double,uint32_t> glavg;

class gam {
private:
	gavg_t *_gs = 0;
	int _len = 0;
public:
	gam(int no, int cs[]);
	virtual ~gam();

	double get(int id = 0);
	double put( double v );
	double set(int id,uint32_t n);
};

class gpid {
private:
    double _kp;  // Proportional Gain
    double _ki;  // Integral Gain
    double _kd;  // Derivative Gain

    double _target;  // 목표치
    double _integral; // 적분
	double _derivative; 
    double _previous_error; // 이전 오차

public:
    gpid(double kp, double ki, double kd) 
		: _kp(kp), _ki(ki), _kd(kd), _target(0.0), _integral(0.0), _previous_error(0.0) {}
	~gpid() {}

    double update(double current_value,double setpoint);

	inline double& goal() { return _target; }
	inline double& kp() { return _kp; }
	inline double& ki() { return _ki; }
	inline double& kd() { return _kd; }
};

#if 0 // example

int main() {
  // PID 제어기 생성
  gpid controller(1.0, 0.5, 0.1);

  // 목표값 설정
  float setpoint = 100.0;

  // 시뮬레이션 시간 설정
  float dt = 0.01;

  // 시뮬레이션 루프
  for (int i = 0; i < 1000; i++) {
    // 현재 측정값
    float measurement = 80.0 + 5.0 * sin(0.1 * i);

    // 오차 계산
    float error = setpoint - measurement;

    // PID 제어 출력 계산
    float output = controller.Update(error, dt);

    // 제어 출력 적용 (예시: 모터 제어)
    // ...

    // 시뮬레이션 출력
    std::cout << "Time: " << i * dt << "s, Error: " << error << ", Output: " << output << std::endl;
  }

  return 0;
}
#endif

#else // c

#ifndef bool
typedef enum {
	false = 0,
	true = 1
} bool;
#endif

#endif // cplusplus

#endif /* GUTILS_H_ */

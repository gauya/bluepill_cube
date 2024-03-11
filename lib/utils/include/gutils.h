
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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif


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
} w32BT_t;

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
    double kp;  // Proportional Gain
    double ki;  // Integral Gain
    double kd;  // Derivative Gain

    double target;  // 목표치
    double integral; // 적분
	double derivative; 
    double previous_error; // 이전 오차

public:
    gpid(double kp, double ki, double kd) 
		: kp(kp), ki(ki), kd(kd), target(0.0), integral(0.0), previous_error(0.0) {}

    // 목표치 설정
    void setpoint(double setpoint) {
        target = setpoint;
    }

    // PID 제어 계산
    double calculate(double current_value) {
        double error = target - current_value;
        integral += error;
        double derivative = error - previous_error;

        // PID 제어 계산
        double output = kp * error + ki * integral + kd * derivative;

        // 이전 오차 업데이트
        previous_error = error;

        return output;
    }

	double Update(double error, double dt) {
		// 누적 오차 계산
		integral += error * dt;

		// 미분 오차 계산
		derivative = (error - previous_error) / dt;

		// PID 제어 출력 계산
		double output = kp * error + ki * integral + kd * derivative;

	    // 출력 제한 ( -1.0f ~ 1.0f )
    	output = fmax(fmin(output, 1.0f), -1.0f);
	
		// 이전 오차 저장
		previous_error = error;

		return output;
	}
    double compute(double current_value,double setpoint) {
        double error = setpoint - current_value;
        integral += error;
        double derivative = error - previous_error;
        previous_error = error;

        double output = kp * error + ki * integral + kd * derivative;
        return output;
    }
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

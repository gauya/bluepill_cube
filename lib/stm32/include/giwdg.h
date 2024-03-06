#ifndef __IWDG_H__
#define __IWDG_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void iwdg_init(int ms=300); // msec
void iwdg_clear();
void iwdg_set(uint16_t ms);

#ifdef __cplusplus
}
#endif

#endif // __IWDG_H__

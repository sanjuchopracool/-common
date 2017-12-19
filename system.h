#ifndef SYSTEM_H
#define SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
/**
 * @brief systemInit will start the system timer (sysTick)
 *        so that we can use delay related fuctions.
 */
void systemInit(void);

/**
 * @brief millis
 * @return no of milli seconds passed since start
 */
uint64_t millis(void);

/**
 * @brief micros
 * @return no of microseconds passed since start
 */
uint64_t micros(void);

void delayMicroseconds(uint32_t us);
void delay(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif // SYSTEM_H

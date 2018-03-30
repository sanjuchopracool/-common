#ifndef SERIAL_H
#define SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "printf.h"

void initSerial();
void sendChar(void* p, char ch);
char getChar();

#ifdef __cplusplus
}
#endif
#endif // SERIAL_H

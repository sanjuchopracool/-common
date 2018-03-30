#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include "common.h"

class SPIInterface
{
public:
    enum SPIName
    {
        SPI_1,
        SPI_2
    };
    explicit SPIInterface( SPIName inName );
    void Init();
    void Deinit();
    uint8_t WriteRead(uint8_t inByte );

private:
    SPIName mSPI;
    DISALLOW_COPY_AND_ASSIGN(SPIInterface)
};

#endif // SPI_H

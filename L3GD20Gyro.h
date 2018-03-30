#ifndef L3GD20GYRO_H
#define L3GD20GYRO_H

#include <SPI.h>
///
/// \brief The L3GD20Gyro class
/// 250 dps/d
class L3GD20Gyro : public SPIInterface
{
public:
    L3GD20Gyro( SPIInterface::SPIName inName );
    ~L3GD20Gyro();

    void Init();
    void Deinit();

    uint8_t GetDeviceID();

    void readRawRate(int16_t (&outValues)[3] );

private:
    void ReadRegister(uint8_t* outBuffer, uint8_t inReadAddr, uint16_t inNumByteToRead);
    void WriteRegister(uint8_t inWriteAddr, uint8_t inData);
};

#endif // L3GD20GYRO_H

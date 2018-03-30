#include "L3GD20Gyro.h"
#include <system.h>
#include <printf.h>

#ifdef STM32F3DISCOVERY
#include <stm32f30x.h>
#define L3GD20_CS_PIN               GPIO_Pin_3
#define L3GD20_CS_GPIO_PORT         GPIOE
#define L3GD20_CS_CLK_ENABLE()      RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOE, ENABLE )
#endif

/* Read/Write command */
#define READWRITE_CMD              ((uint8_t)0x80)
/* Multiple byte read/write command */
#define MULTIPLEBYTE_CMD           ((uint8_t)0x40)
/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device */
#define DUMMY_BYTE                 ((uint8_t)0x00)

/******************************************************************************/
/*************************** START REGISTER MAPPING  **************************/
/******************************************************************************/
#define L3GD20_WHO_AM_I_ADDR          0x0F  /* device identification register */
#define L3GD20_CTRL_REG1_ADDR         0x20  /* Control register 1 */
#define L3GD20_CTRL_REG2_ADDR         0x21  /* Control register 2 */
#define L3GD20_CTRL_REG3_ADDR         0x22  /* Control register 3 */
#define L3GD20_CTRL_REG4_ADDR         0x23  /* Control register 4 */
#define L3GD20_CTRL_REG5_ADDR         0x24  /* Control register 5 */
#define L3GD20_REFERENCE_REG_ADDR     0x25  /* Reference register */
#define L3GD20_OUT_TEMP_ADDR          0x26  /* Out temp register */
#define L3GD20_STATUS_REG_ADDR        0x27  /* Status register */
#define L3GD20_OUT_X_L_ADDR           0x28  /* Output Register X */
#define L3GD20_OUT_X_H_ADDR           0x29  /* Output Register X */
#define L3GD20_OUT_Y_L_ADDR           0x2A  /* Output Register Y */
#define L3GD20_OUT_Y_H_ADDR           0x2B  /* Output Register Y */
#define L3GD20_OUT_Z_L_ADDR           0x2C  /* Output Register Z */
#define L3GD20_OUT_Z_H_ADDR           0x2D  /* Output Register Z */
#define L3GD20_FIFO_CTRL_REG_ADDR     0x2E  /* Fifo control Register */
#define L3GD20_FIFO_SRC_REG_ADDR      0x2F  /* Fifo src Register */

#define L3GD20_INT1_CFG_ADDR          0x30  /* Interrupt 1 configuration Register */
#define L3GD20_INT1_SRC_ADDR          0x31  /* Interrupt 1 source Register */
#define L3GD20_INT1_TSH_XH_ADDR       0x32  /* Interrupt 1 Threshold X register */
#define L3GD20_INT1_TSH_XL_ADDR       0x33  /* Interrupt 1 Threshold X register */
#define L3GD20_INT1_TSH_YH_ADDR       0x34  /* Interrupt 1 Threshold Y register */
#define L3GD20_INT1_TSH_YL_ADDR       0x35  /* Interrupt 1 Threshold Y register */
#define L3GD20_INT1_TSH_ZH_ADDR       0x36  /* Interrupt 1 Threshold Z register */
#define L3GD20_INT1_TSH_ZL_ADDR       0x37  /* Interrupt 1 Threshold Z register */
#define L3GD20_INT1_DURATION_ADDR     0x38  /* Interrupt 1 DURATION register */

/******************************************************************************/
/**************************** END REGISTER MAPPING  ***************************/
/******************************************************************************/

#define I_AM_L3GD20                 ((uint8_t)0xD4)
#define I_AM_L3GD20_TR              ((uint8_t)0xD5)

/** @defgroup Power_Mode_selection
  * @{
  */
#define L3GD20_MODE_POWERDOWN       ((uint8_t)0x00)
#define L3GD20_MODE_ACTIVE          ((uint8_t)0x08)
/**
  * @}
  */

/** @defgroup OutPut_DataRate_Selection
  * @{
  */
#define L3GD20_OUTPUT_DATARATE_1    ((uint8_t)0x00)
#define L3GD20_OUTPUT_DATARATE_2    ((uint8_t)0x40)
#define L3GD20_OUTPUT_DATARATE_3    ((uint8_t)0x80)
#define L3GD20_OUTPUT_DATARATE_4    ((uint8_t)0xC0)
/**
  * @}
  */

/** @defgroup Axes_Selection
  * @{
  */
#define L3GD20_X_ENABLE            ((uint8_t)0x02)
#define L3GD20_Y_ENABLE            ((uint8_t)0x01)
#define L3GD20_Z_ENABLE            ((uint8_t)0x04)
#define L3GD20_AXES_ENABLE         ((uint8_t)0x07)
#define L3GD20_AXES_DISABLE        ((uint8_t)0x00)
/**
  * @}
  */

/** @defgroup Bandwidth_Selection
  * @{
  */
#define L3GD20_BANDWIDTH_1         ((uint8_t)0x00)
#define L3GD20_BANDWIDTH_2         ((uint8_t)0x10)
#define L3GD20_BANDWIDTH_3         ((uint8_t)0x20)
#define L3GD20_BANDWIDTH_4         ((uint8_t)0x30)
/**
  * @}
  */

/** @defgroup Full_Scale_Selection
  * @{
  */
#define L3GD20_FULLSCALE_250       ((uint8_t)0x00)
#define L3GD20_FULLSCALE_500       ((uint8_t)0x10)
#define L3GD20_FULLSCALE_2000      ((uint8_t)0x20)
#define L3GD20_FULLSCALE_SELECTION ((uint8_t)0x30)
/**
  * @}
  */

/** @defgroup Full_Scale_Sensitivity
  * @{
  */
#define L3GD20_SENSITIVITY_250DPS  ((float)8.75f)         /*!< gyroscope sensitivity with 250 dps full scale [DPS/LSB]  */
#define L3GD20_SENSITIVITY_500DPS  ((float)17.50f)        /*!< gyroscope sensitivity with 500 dps full scale [DPS/LSB]  */
#define L3GD20_SENSITIVITY_2000DPS ((float)70.00f)        /*!< gyroscope sensitivity with 2000 dps full scale [DPS/LSB] */
/**
  * @}
  */


/** @defgroup Block_Data_Update
  * @{
  */
#define L3GD20_BlockDataUpdate_Continous   ((uint8_t)0x00)
#define L3GD20_BlockDataUpdate_Single      ((uint8_t)0x80)
/**
  * @}
  */

/** @defgroup Endian_Data_selection
  * @{
  */
#define L3GD20_BLE_LSB                     ((uint8_t)0x00)
#define L3GD20_BLE_MSB	                   ((uint8_t)0x40)
/**
  * @}
  */

/** @defgroup High_Pass_Filter_status
  * @{
  */
#define L3GD20_HIGHPASSFILTER_DISABLE      ((uint8_t)0x00)
#define L3GD20_HIGHPASSFILTER_ENABLE	   ((uint8_t)0x10)
/**
  * @}
  */

/** @defgroup INT1_INT2_selection
  * @{
  */
#define L3GD20_INT1                        ((uint8_t)0x00)
#define L3GD20_INT2                        ((uint8_t)0x01)
/**
  * @}
  */

/** @defgroup INT1_Interrupt_status
  * @{
  */
#define L3GD20_INT1INTERRUPT_DISABLE       ((uint8_t)0x00)
#define L3GD20_INT1INTERRUPT_ENABLE        ((uint8_t)0x80)
/**
  * @}
  */

/** @defgroup INT2_Interrupt_status
  * @{
  */
#define L3GD20_INT2INTERRUPT_DISABLE       ((uint8_t)0x00)
#define L3GD20_INT2INTERRUPT_ENABLE        ((uint8_t)0x08)
/**
  * @}
  */

/** @defgroup INT1_Interrupt_ActiveEdge
  * @{
  */
#define L3GD20_INT1INTERRUPT_LOW_EDGE      ((uint8_t)0x20)
#define L3GD20_INT1INTERRUPT_HIGH_EDGE     ((uint8_t)0x00)
/**
  * @}
  */

/** @defgroup Boot_Mode_selection
  * @{
  */
#define L3GD20_BOOT_NORMALMODE             ((uint8_t)0x00)
#define L3GD20_BOOT_REBOOTMEMORY           ((uint8_t)0x80)
/**
  * @}
  */

/** @defgroup High_Pass_Filter_Mode
  * @{
  */
#define L3GD20_HPM_NORMAL_MODE_RES         ((uint8_t)0x00)
#define L3GD20_HPM_REF_SIGNAL              ((uint8_t)0x10)
#define L3GD20_HPM_NORMAL_MODE             ((uint8_t)0x20)
#define L3GD20_HPM_AUTORESET_INT           ((uint8_t)0x30)
/**
  * @}
  */

/** @defgroup High_Pass_CUT OFF_Frequency
  * @{
  */
#define L3GD20_HPFCF_0              0x00
#define L3GD20_HPFCF_1              0x01
#define L3GD20_HPFCF_2              0x02
#define L3GD20_HPFCF_3              0x03
#define L3GD20_HPFCF_4              0x04
#define L3GD20_HPFCF_5              0x05
#define L3GD20_HPFCF_6              0x06
#define L3GD20_HPFCF_7              0x07
#define L3GD20_HPFCF_8              0x08
#define L3GD20_HPFCF_9              0x09

#define GYRO_SENSITIVITY            -0.00875 // mdps/digit, negative because of my board orientation
namespace
{
uint8_t tempBuffer[6] = {0};

inline void EnableSlave()
{
    GPIO_ResetBits( L3GD20_CS_GPIO_PORT, L3GD20_CS_PIN );
}

inline void DisableSlave()
{
    GPIO_SetBits( L3GD20_CS_GPIO_PORT, L3GD20_CS_PIN );
}

}

L3GD20Gyro::L3GD20Gyro(SPIInterface::SPIName inName)
    : SPIInterface( inName )
{
    L3GD20_CS_CLK_ENABLE();
    // init pin
    GPIO_InitTypeDef theGPIOInit;
    // configure Slave Select
    theGPIOInit.GPIO_Pin = L3GD20_CS_PIN;
    theGPIOInit.GPIO_Mode = GPIO_Mode_OUT;
    theGPIOInit.GPIO_OType = GPIO_OType_PP;
    theGPIOInit.GPIO_Speed = GPIO_Speed_50MHz;
    theGPIOInit.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init( L3GD20_CS_GPIO_PORT, &theGPIOInit );
    DisableSlave();
    Init();
}

L3GD20Gyro::~L3GD20Gyro()
{

}

void L3GD20Gyro::Init()
{
    SPIInterface::Init();
    WriteRegister(L3GD20_CTRL_REG1_ADDR,
                  L3GD20_AXES_ENABLE | L3GD20_MODE_ACTIVE );
    WriteRegister(L3GD20_CTRL_REG4_ADDR,
                  L3GD20_BlockDataUpdate_Single | L3GD20_FULLSCALE_500);
}

void L3GD20Gyro::Deinit()
{
    Deinit();
}

uint8_t L3GD20Gyro::GetDeviceID()
{
    uint8_t temp;
    ReadRegister(&temp, L3GD20_WHO_AM_I_ADDR, 1);
    return temp;
}

void L3GD20Gyro::readRawRate(int16_t (&outValues)[3] )
{
    ReadRegister(tempBuffer, L3GD20_OUT_X_L_ADDR, 6);
    outValues[0] = (tempBuffer[1] << 8)| tempBuffer[0];
    outValues[1] = (tempBuffer[3] << 8)| tempBuffer[2];
    outValues[2] = (tempBuffer[5] << 8)| tempBuffer[4];
}

void L3GD20Gyro::ReadRegister(uint8_t* outBuffer, uint8_t inReadAddr, uint16_t inNumByteToRead)
{
    inReadAddr |= READWRITE_CMD;
    if(inNumByteToRead > 0x01)
        inReadAddr |= MULTIPLEBYTE_CMD;

    EnableSlave();
    WriteRead(inReadAddr);
    while(inNumByteToRead > 0)
    {
        *outBuffer = WriteRead(DUMMY_BYTE);
        inNumByteToRead--;
        outBuffer++;
    }
    DisableSlave();
}

void L3GD20Gyro::WriteRegister(uint8_t inWriteAddr, uint8_t inData)
{
    EnableSlave();
    WriteRead(inWriteAddr);
    WriteRead(inData);
    DisableSlave();
}

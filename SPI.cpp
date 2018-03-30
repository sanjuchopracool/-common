#include "SPI.h"

#ifdef USE_SPI2

#if defined(STM32F3DISCOVERY) || defined(STM32F429_439xx)

#ifdef STM32F3DISCOVERY
#include <stm32f30x.h>
#include <stm32f30x_spi.h>

#define SPI2_SCK_GPIO_CLK_ENABLE()       RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE)
#define SPI2_MISO_GPIO_CLK_ENABLE()      RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE)
#define SPI2_MOSI_GPIO_CLK_ENABLE()      RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE)
#define SPI2_MOSI_AF                     GPIO_AF_5
#define SPI2_MISO_AF                     GPIO_AF_5
#define SPI2_SCK_AF                      GPIO_AF_5

#define SPI_SEND_DATA                    SPI_SendData8
#define SPI_RECEIVE_DATA                 SPI_ReceiveData8
#define SPI2_BAUDRATE_PRESCALER          SPI_BaudRatePrescaler_4
#endif

#if  defined(STM32F429_439xx) || defined(STM32F40_41xxx)
#include <stm32f4xx.h>
#include <stm32f4xx_spi.h>

#define SPI2_SCK_GPIO_CLK_ENABLE()       RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE)
#define SPI2_MISO_GPIO_CLK_ENABLE()      RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE)
#define SPI2_MOSI_GPIO_CLK_ENABLE()      RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE)

#define SPI2_MOSI_AF                     GPIO_AF_SPI2
#define SPI2_MISO_AF                     GPIO_AF_SPI2
#define SPI2_SCK_AF                      GPIO_AF_SPI2

#define SPI_SEND_DATA                    SPI_SendData
#define SPI_RECEIVE_DATA                 SPI_ReceiveData
#define SPI2_BAUDRATE_PRESCALER          SPI_BaudRatePrescaler_16
#endif

/* Definition for SPIx clock resources */
#define SPI2_CLK_ENABLE()                RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE )
#define SPI2_CLK_DISABLE()               RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, DISABLE )

#define SPI2_FORCE_RESET()               RCC_APB1PeriphResetCmd( RCC_APB1Periph_SPI2, ENABLE )
#define SPI2_RELEASE_RESET()             RCC_APB1PeriphResetCmd( RCC_APB1Periph_SPI2, DISABLE )

// SPI2 Baud RATE

/* Definition for SPIx Pins */

#define SPI2_SCK_PIN                     GPIO_Pin_13
#define SPI2_SCK_PIN_SOURCE              GPIO_PinSource13
#define SPI2_SCK_GPIO_PORT               GPIOB

#define SPI2_MISO_PIN                    GPIO_Pin_14
#define SPI2_MISO_PIN_SOURCE             GPIO_PinSource14
#define SPI2_MISO_GPIO_PORT              GPIOB

#define SPI2_MOSI_PIN                    GPIO_Pin_15
#define SPI2_MOSI_PIN_SOURCE             GPIO_PinSource15
#define SPI2_MOSI_GPIO_PORT              GPIOB
#endif // USE_SPI2
#endif // STM32F3DISCOVERY

#ifdef USE_SPI2
void InitSPI2()
{
    // enable port clock for MOSI, MISO, SCK
    SPI2_MOSI_GPIO_CLK_ENABLE();
    SPI2_MISO_GPIO_CLK_ENABLE();
    SPI2_SCK_GPIO_CLK_ENABLE();

    // enable spi clock
    SPI2_CLK_ENABLE();

    GPIO_InitTypeDef theGPIOInit;

    // configure MOSI, MISO, SCK
    theGPIOInit.GPIO_Mode = GPIO_Mode_AF;
    theGPIOInit.GPIO_OType = GPIO_OType_PP;
    theGPIOInit.GPIO_Speed = GPIO_Speed_50MHz;
    theGPIOInit.GPIO_PuPd = GPIO_PuPd_NOPULL;

    // SCK
    theGPIOInit.GPIO_Pin = SPI2_SCK_PIN;
    GPIO_PinAFConfig( SPI2_SCK_GPIO_PORT, SPI2_SCK_PIN_SOURCE, SPI2_SCK_AF );
    GPIO_Init( SPI2_SCK_GPIO_PORT, &theGPIOInit );

    // MOSI
    theGPIOInit.GPIO_Pin = SPI2_MOSI_PIN;
    GPIO_PinAFConfig( SPI2_MOSI_GPIO_PORT, SPI2_MOSI_PIN_SOURCE, SPI2_MOSI_AF );
    GPIO_Init( SPI2_MOSI_GPIO_PORT, &theGPIOInit );
    // MISO
    theGPIOInit.GPIO_Pin = SPI2_MISO_PIN;
    GPIO_PinAFConfig( SPI2_MISO_GPIO_PORT, SPI2_MISO_PIN_SOURCE, SPI2_MISO_AF );
    GPIO_Init( SPI2_MISO_GPIO_PORT, &theGPIOInit );

    SPI_InitTypeDef theSpiInit;
    theSpiInit.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // set to full duplex
    theSpiInit.SPI_Mode = SPI_Mode_Master;     // transmit in master mode
    theSpiInit.SPI_DataSize = SPI_DataSize_8b; // one packet of data is 8 bits wide
    theSpiInit.SPI_CPOL = SPI_CPOL_Low;        // clock is low when idle
    theSpiInit.SPI_CPHA = SPI_CPHA_1Edge;      // data sampled at first edge
    theSpiInit.SPI_NSS = SPI_NSS_Soft;
    theSpiInit.SPI_BaudRatePrescaler = SPI2_BAUDRATE_PRESCALER; // SPI frequency is
    theSpiInit.SPI_FirstBit = SPI_FirstBit_MSB;// data is transmitted MSB first
    theSpiInit.SPI_CRCPolynomial = 7;

#ifdef STM32F3DISCOVERY
    // threshold
    SPI_RxFIFOThresholdConfig( SPI2, SPI_RxFIFOThreshold_QF );
#endif
    // init spi
    SPI_Init(SPI2, &theSpiInit);

    //enable spi
    SPI_Cmd( SPI2, ENABLE );
}

void DeinitSPI2()
{   
    /* SD_SPI disable */
    SPI_Cmd(SPI2, DISABLE);
    SPI2_CLK_DISABLE();
    /*##-1- Reset SPI2 ##################################################*/
    SPI2_FORCE_RESET();
    SPI2_RELEASE_RESET();
}

uint16_t SPI2WriteRead( uint16_t inByte )
{    
    while((SPI2->SR & SPI_I2S_FLAG_TXE) == RESET);
    SPI_SEND_DATA(SPI2, inByte);
    while((SPI2->SR & SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_RECEIVE_DATA(SPI2);

}
#endif


#ifdef USE_SPI1
#if defined(NUCLEO401RE)

#include <stm32f4xx.h>
#include <stm32f4xx_spi.h>

#define SPI1_SCK_GPIO_CLK_ENABLE()       RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE)
#define SPI1_MISO_GPIO_CLK_ENABLE()      RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE)
#define SPI1_MOSI_GPIO_CLK_ENABLE()      RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE)

#define SPI1_MOSI_AF                     GPIO_AF_SPI1
#define SPI1_MISO_AF                     GPIO_AF_SPI1
#define SPI1_SCK_AF                      GPIO_AF_SPI1

#define SPI_SEND_DATA                    SPI_SendData
#define SPI_RECEIVE_DATA                 SPI_ReceiveData
#define SPI1_BAUDRATE_PRESCALER          SPI_BaudRatePrescaler_8

/* Definition for SPIx clock resources */
#define SPI1_CLK_ENABLE()                RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, ENABLE )
#define SPI1_CLK_DISABLE()               RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, DISABLE )

#define SPI1_FORCE_RESET()               RCC_APB2PeriphResetCmd( RCC_APB2Periph_SPI1, ENABLE )
#define SPI1_RELEASE_RESET()             RCC_APB2PeriphResetCmd( RCC_APB2Periph_SPI1, DISABLE )

// SPI1 Baud RATE

/* Definition for SPIx Pins */

#define SPI1_SCK_PIN                     GPIO_Pin_5
#define SPI1_SCK_PIN_SOURCE              GPIO_PinSource5
#define SPI1_SCK_GPIO_PORT               GPIOA

#define SPI1_MISO_PIN                    GPIO_Pin_6
#define SPI1_MISO_PIN_SOURCE             GPIO_PinSource6
#define SPI1_MISO_GPIO_PORT              GPIOA

#define SPI1_MOSI_PIN                    GPIO_Pin_7
#define SPI1_MOSI_PIN_SOURCE             GPIO_PinSource7
#define SPI1_MOSI_GPIO_PORT              GPIOA
#endif

#if defined(STM32F3DISCOVERY)
#include <stm32f30x.h>
#include <stm32f30x_spi.h>

#define SPI1_SCK_GPIO_CLK_ENABLE()       RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE)
#define SPI1_MISO_GPIO_CLK_ENABLE()      RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE)
#define SPI1_MOSI_GPIO_CLK_ENABLE()      RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE)

#define SPI1_MOSI_AF                     GPIO_AF_5
#define SPI1_MISO_AF                     GPIO_AF_5
#define SPI1_SCK_AF                      GPIO_AF_5

#define SPI_SEND_DATA                    SPI_SendData8
#define SPI_RECEIVE_DATA                 SPI_ReceiveData8
#define SPI1_BAUDRATE_PRESCALER          SPI_BaudRatePrescaler_8

/* Definition for SPIx clock resources */
#define SPI1_CLK_ENABLE()                RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, ENABLE )
#define SPI1_CLK_DISABLE()               RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, DISABLE )

#define SPI1_FORCE_RESET()               RCC_APB2PeriphResetCmd( RCC_APB2Periph_SPI1, ENABLE )
#define SPI1_RELEASE_RESET()             RCC_APB2PeriphResetCmd( RCC_APB2Periph_SPI1, DISABLE )

// SPI1 Baud RATE

/* Definition for SPIx Pins */

#define SPI1_SCK_PIN                     GPIO_Pin_5
#define SPI1_SCK_PIN_SOURCE              GPIO_PinSource5
#define SPI1_SCK_GPIO_PORT               GPIOA

#define SPI1_MISO_PIN                    GPIO_Pin_6
#define SPI1_MISO_PIN_SOURCE             GPIO_PinSource6
#define SPI1_MISO_GPIO_PORT              GPIOA

#define SPI1_MOSI_PIN                    GPIO_Pin_7
#define SPI1_MOSI_PIN_SOURCE             GPIO_PinSource7
#define SPI1_MOSI_GPIO_PORT              GPIOA
#endif // STM32F3DISCOVERY
#endif // USE_SPI1

#ifdef USE_SPI1
void InitSPI1()
{
    // enable port clock for MOSI, MISO, SCK
    SPI1_MOSI_GPIO_CLK_ENABLE();
    SPI1_MISO_GPIO_CLK_ENABLE();
    SPI1_SCK_GPIO_CLK_ENABLE();

    // enable spi clock
    SPI1_CLK_ENABLE();

    GPIO_InitTypeDef theGPIOInit;

    // configure MOSI, MISO, SCK
    theGPIOInit.GPIO_Mode = GPIO_Mode_AF;
    theGPIOInit.GPIO_OType = GPIO_OType_PP;
    theGPIOInit.GPIO_Speed = GPIO_Speed_50MHz;
    theGPIOInit.GPIO_PuPd = GPIO_PuPd_NOPULL;

    // SCK
    theGPIOInit.GPIO_Pin = SPI1_SCK_PIN;
    GPIO_PinAFConfig( SPI1_SCK_GPIO_PORT, SPI1_SCK_PIN_SOURCE, SPI1_SCK_AF );
    GPIO_Init( SPI1_SCK_GPIO_PORT, &theGPIOInit );

    // MOSI
    theGPIOInit.GPIO_Pin = SPI1_MOSI_PIN;
    GPIO_PinAFConfig( SPI1_MOSI_GPIO_PORT, SPI1_MOSI_PIN_SOURCE, SPI1_MOSI_AF );
    GPIO_Init( SPI1_MOSI_GPIO_PORT, &theGPIOInit );
    // MISO
    theGPIOInit.GPIO_Pin = SPI1_MISO_PIN;
    GPIO_PinAFConfig( SPI1_MISO_GPIO_PORT, SPI1_MISO_PIN_SOURCE, SPI1_MISO_AF );
    GPIO_Init( SPI1_MISO_GPIO_PORT, &theGPIOInit );

    SPI_InitTypeDef theSpiInit;
    theSpiInit.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // set to full duplex
    theSpiInit.SPI_Mode = SPI_Mode_Master;     // transmit in master mode
    theSpiInit.SPI_DataSize = SPI_DataSize_8b; // one packet of data is 8 bits wide
    theSpiInit.SPI_CPOL = SPI_CPOL_Low;        // clock is low when idle
    theSpiInit.SPI_CPHA = SPI_CPHA_1Edge;      // data sampled at first edge
    theSpiInit.SPI_NSS = SPI_NSS_Soft;
    theSpiInit.SPI_BaudRatePrescaler = SPI1_BAUDRATE_PRESCALER; // SPI frequency is
    theSpiInit.SPI_FirstBit = SPI_FirstBit_MSB;// data is transmitted MSB first
    theSpiInit.SPI_CRCPolynomial = 7;

#ifdef STM32F3DISCOVERY
    // threshold
    SPI_RxFIFOThresholdConfig( SPI1, SPI_RxFIFOThreshold_QF );
#endif
    // init spi
    SPI_Init(SPI1, &theSpiInit);

    //enable spi
    SPI_Cmd( SPI1, ENABLE );
}

void DeinitSPI1()
{
    /* SD_SPI disable */
    SPI_Cmd(SPI1, DISABLE);
    SPI1_CLK_DISABLE();
    /*##-1- Reset SPI1 ##################################################*/
    SPI1_FORCE_RESET();
    SPI1_RELEASE_RESET();
}

uint16_t SPI1WriteRead( uint16_t inByte )
{
    while((SPI1->SR & SPI_I2S_FLAG_TXE) == RESET);
    SPI_SEND_DATA(SPI1, inByte);
    while((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_RECEIVE_DATA(SPI1);

}
#endif

SPIInterface::SPIInterface(SPIInterface::SPIName inName)
    : mSPI( inName )
{
    mSPI = inName;
}

void SPIInterface::Init()
{
#ifdef USE_SPI2
    if ( mSPI == SPI_2 )
    {
        InitSPI2();
    }
#endif
#ifdef USE_SPI1
    if ( mSPI == SPI_1 )
    {
        InitSPI1();
    }
#endif
}

void SPIInterface::Deinit()
{
#ifdef USE_SPI2
    if ( mSPI == SPI_2 )
    {
        DeinitSPI2();
    }
#endif
#ifdef USE_SPI1
    if ( mSPI == SPI_1 )
    {
        DeinitSPI1();
    }
#endif
}

//uint8_t SPIInterface::WriteRead( uint8_t inByte )
//{
//#ifdef USE_SPI2
//    if ( mSPI == SPI_2 )
//    {
//        return SPI2WriteRead( inByte );
//    }
//#endif
//#ifdef USE_SPI1
//    if ( mSPI == SPI_1 )
//    {
//        return SPI1WriteRead( inByte );
//    }
//#endif
//}

uint8_t SPIInterface::WriteRead(uint8_t inByte)
{
#ifdef USE_SPI2
    if ( mSPI == SPI_2 )
    {
        return SPI2WriteRead( inByte );
    }
#endif
#ifdef USE_SPI1
    if ( mSPI == SPI_1 )
    {
        return SPI1WriteRead( inByte );
    }
#endif
}

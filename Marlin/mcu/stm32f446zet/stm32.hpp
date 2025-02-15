#pragma once

#ifdef STM32F4xx
#include "variant.h"
#include "stm32f446xx.h"

enum IO_MODE{
	_IPU,//pull-up input
	_FLOATING,//floating input
	_AIN,//Analog input
	_IPD,//Drop down input
	_PP,//push-pull output
	_OD,//Open drain output
	_AFPP,//Multiplex push-pull
	_AFOD,//Multiplexed open drain
};

typedef struct
{
  uint8_t memToMem;
  uint8_t fromMem;
  uint8_t memInc;
  uint8_t perInc;
  uint32_t mem;
  uint32_t per;
  uint32_t len;
}DMA_START_St;

void myDelayUs(uint32_t n);
void myDelayMs(uint32_t n);
void pinInit(uint8_t pin,IO_MODE mode);
void gpioInit(GPIO_TypeDef* port,uint8_t index,IO_MODE mode);
void yDmaInit(DMA_Stream_TypeDef* DMA_Streamx,uint32_t channel,int memDataBit,int perDataBit);
void yDmaStart(DMA_Stream_TypeDef* DMA_Streamx, DMA_START_St* st,bool start = true);
void yClearDmaStatus(DMA_Stream_TypeDef* DMA_Streamx);
uint8_t yDmaOk(DMA_Stream_TypeDef* DMA_Streamx);
void AFSet(GPIO_TypeDef* port,uint32_t io_index,uint32_t af);

typedef struct
{
	volatile uint16_t REG;
	volatile uint16_t RAM;
}FMC_LCD_TypeDef;
extern FMC_LCD_TypeDef *mLCD;
void debugStr(const char*str);

int LCD_McuInit(uint8_t background_on = 0);
void LCD_McuDeinit(void);
#define BACKGROUND_LIGHT_ON()  GPIOG->BSRR = 1UL << 8
#define BACKGROUND_LIGHT_OFF() GPIOG->BSRR = (1UL << 8) << 16
void LCD_McuHighSpeed(int index);
void LCD_Reset(void);

#define LCD_WReg(v)    mLCD->REG = (v);__DSB()
#define LCD_WData(v)   mLCD->RAM = (v);__DSB()
#define LCD_RData(v)  v = mLCD->RAM;__DSB()

void LCD_BatchSyn(const uint16_t*colors,int len,uint8_t autoInc);
#define LCD_DMA_Stream                DMA2_Stream7 //After making changes here, pay attention to modifying the completion flag and corresponding registers and clearing the flag register.
//Note: LCD uses the FSMC interface. For MCU-DMA, it is memory to memory, so any data stream and channel can be used here.
#define LCD_DMA_Channel               0
#define LCD_DMA_TCIF                  DMA_HISR_TCIF7 //Transfer completed
#define LCD_DMA_TEIF                  DMA_HISR_TEIF7 //Transmission error
#if defined(LCD_DMA_Stream)
void LCD_BatchStart(const uint16_t* colors,int num,uint8_t autoInc);
uint8_t LCD_Busy();
#define LCD_BatchMaxNum               65520
#else
#define LCD_BatchStart  LCD_BatchSyn
inline uint8_t LCD_Busy(){return 0;}
#endif

#define W25Q_CS_PIN           PG15
#define W25Q_SPI              SPI3
void w25q_spi_init(void);
inline void w25q_spi_cs_high() {GPIOG->BSRR = 1UL << 15;} //W25Q_CS_PIN High
inline void w25q_spi_cs_low()  {GPIOG->BSRR = (1UL << 15) << 16;} //W25Q_CS_PIN Low
uint8_t w25q_spi_send_receive(const uint8_t *sendDt, bool sendInc,uint8_t *rcvDt,bool rcvInc,int len);

#define W25Q_Send_DMA_Stream             DMA1_Stream5 //After making changes here, pay attention to modifying the completion flag and corresponding registers and clearing the flag register.
#define W25Q_Rcv_DMA_Stream              DMA1_Stream2
#define W25Q_Send_DMA_Channel            0 //Data stream 5 channel 0 is used for SPI3-TX, see P205
#define W25Q_Rcv_DMA_Channel             0
#define W25Q_RX_DMA_Channel              0 //Data stream 2 channel 0 is used for SPI3-RX, see P205
#define W25Q_RX_DMA_TCIF                 DMA_LISR_TCIF0
#define W25Q_RX_DMA_TEIF                 DMA_LISR_TEIF0
void w25q_spi_dma(const uint8_t*sendBuff, uint8_t* recvBuff, uint32_t len);
#if defined(W25Q_Send_DMA_Stream) || defined(W25Q_Rcv_DMA_Stream)
void w25q_spi_dma_stop(uint32_t period = 200);
uint8_t w25q_spi_dma_ok(void);
#else
inline void w25q_spi_dma_stop(uint32_t period = 200){(void)period;}
inline uint8_t w25q_spi_dma_ok() {return 1;}
#endif

inline void at24Cs_High() {
  GPIOB->BSRR = 1UL << 7; //AT24CXX_WP:PB7=1
}
inline void at24Cs_Low() {
  GPIOB->BSRR = (1UL << 7) << 16; //AT24CXX_WP:PB7=0
}
#endif

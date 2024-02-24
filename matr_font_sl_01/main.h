#pragma once
#ifndef __MAIN_H__
#define __MAIN_H__
#include "stm32f0xx.h"

#define  LEDTOGGLE GPIOA->ODR ^= (1<<2)

volatile uint32_t ttms   = 0; // системный тикер
volatile uint32_t ddms   = 0; //
volatile uint32_t pa2ms  = 0;
volatile uint32_t count  = 0;

void SysTick_Handler(void) 
{
  ++ttms;
  if (ddms) ddms--;
}

void rcc_sysclockinit(void)
{
  RCC->CR = RCC_CR_HSEON;
  while(! (RCC->CR & RCC_CR_HSERDY));
  RCC->CFGR2 = RCC_CFGR2_PREDIV_DIV2;
  RCC->CFGR  = RCC_CFGR_PLLMUL4 | // 
               RCC_CFGR_PLLSRC_HSE_PREDIV;
  RCC->CR |= RCC_CR_PLLON;
  FLASH->ACR = _VAL2FLD(FLASH_ACR_LATENCY, 1) | FLASH_ACR_PRFTBE;
  while(! (RCC->CR & RCC_CR_PLLRDY));
  RCC->CFGR |= RCC_CFGR_SW_PLL;
}

void gpio_init(void)
{
  RCC->AHBENR   |= RCC_AHBENR_GPIOAEN;
  RCC->AHBENR   |= RCC_AHBENR_GPIOBEN;
  // LED PA2
  GPIOA->MODER  |= GPIO_MODER_MODER2_0;
}

void blink_(uint16_t freq)
{
  if (pa2ms > ttms || ttms - pa2ms > freq)
  {
    LEDTOGGLE;
    pa2ms = ttms;
  }
}

void delay_ms(uint32_t ms)
{
  ddms = ms;
  while(ddms) {};
}

void spi_init(void)
{
  // SCK PB3
  GPIOB->MODER   |= GPIO_MODER_MODER3_1; // alternate function
  // MOSI PB5
  GPIOB->MODER   |= GPIO_MODER_MODER5_1; // alternate function
  // soft nCS PB4
  GPIOB->MODER   |= GPIO_MODER_MODER4_0; // PB4 as output
  GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR4_0);
  // DC (RS) PB1
  GPIOB->MODER   |= GPIO_MODER_MODER1_0;
  GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR1_0);
  // Reset PB0
  GPIOB->MODER   |= GPIO_MODER_MODER0_0;
  GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR0_0);
  // GPIOB->BSRR |= GPIO_BSRR_BS_4 // CS_UP;
  RCC->APB2ENR   |= RCC_APB2ENR_SPI1EN;
  SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR_0;
  SPI1->CR2 |= SPI_CR2_FRXTH;
  
  SPI1->CR2 |= SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0; // 8-bit

  // SPI1->CR1 &= ~(SPI_CR1_RXONLY);	
  // SPI1->CR1 |= SPI_CR2_FRF;
  SPI1->CR1 |= SPI_CR1_SPE; // Go
}


//Index 1==0b0001 => 0b1000
//Index 7==0b0111 => 0b1110
//etc
static unsigned char lookup[16] = {
0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };

uint8_t reverse(uint8_t n) {
   // Reverse the top and bottom nibble then swap them.
   return (lookup[n&0b1111] << 4) | lookup[n>>4];
}

// Detailed breakdown of the math
//  + lookup reverse of bottom nibble
//  |       + grab bottom nibble
//  |       |        + move bottom result into top nibble
//  |       |        |     + combine the bottom and top results 
//  |       |        |     | + lookup reverse of top nibble
//  |       |        |     | |       + grab top nibble
//  V       V        V     V V       V
// (lookup[n&0b1111] << 4) | lookup[n>>4]

unsigned char rever2se(unsigned char b) 
{
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}

unsigned char rever1se(unsigned char b) {
   b = (b & 0b11110000) >> 4 | (b & 0b00001111) << 4;
   b = (b & 0b11001100) >> 2 | (b & 0b00110011) << 2;
   b = (b & 0b10101010) >> 1 | (b & 0b01010101) << 1;
   return b;
}



#endif // __MAIN_H__
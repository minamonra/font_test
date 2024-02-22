#pragma once
#ifndef __LCD_ST7735SL__
#define __LCD_ST7735SL__

#include "stm32f0xx.h"

#define LCD_SOFT_RST_DELAY 120  // пауза после программного сброса дисплея по даташиту пауза должна быть 120 мс
#define LCD_RST_DLY        010	// пауза в мс при сбросе дисплея, по даташиту должно быть 120 мс, но зачастую работает и 10 мс
// ОПРЕДЕЛЕНИЕ ПОРЯДКА КОДИРОВАНИЯ ЦВЕТА
// если закоментировать параметр ниже, то порядок кодирования будет 5B - 6G - 5R
//#define RGB                                   // цвета кодируются 5R - 6G - 5B
// Примечание:
// число перед буквой показывает сколько бит какой цвет кодируют
// латинской буквой определяется цвет: R - Red (красный), G - Green (зеленый), B - Blue (синий)

// PB3 - SPI1_SCK (clock)
// PB4 - CS (chip select)
// PB5 - SPI1_MOSI (data)

// Chip select PB4
#define CS_UP GPIOB->BSRR |= GPIO_BSRR_BS_4
#define CS_DN GPIOB->BSRR |= GPIO_BSRR_BR_4

// DC (RS) PB1
#define DC_UP GPIOB->BSRR |= GPIO_BSRR_BS_1
#define DC_DN GPIOB->BSRR |= GPIO_BSRR_BR_1

// Reset PB0
#define RST_UP GPIOB->BSRR |= GPIO_BSRR_BS_0
#define RST_DN GPIOB->BSRR |= GPIO_BSRR_BR_0

#define SPI2SIXTEEN SPI1->CR2 &= ~SPI_CR2_FRXTH; SPI1->CR2 |=  SPI_CR2_DS_3; // переключаемся на 16 бит
#define SPI2EIGHT   SPI1->CR2 |=  SPI_CR2_FRXTH; SPI1->CR2 &= ~SPI_CR2_DS_3; // обратно на 8 бит  

#define SPIDR8BIT (*(__IO uint8_t *)((uint32_t)&SPI1->DR))

// Some ready-made 16-bit ('565') color settings:
#define ST77XX_BLACK      0x0000
#define ST77XX_WHITE      0xFFFF
#define ST77XX_RED        0xF800
#define ST77XX_GREEN      0x07E0
#define ST77XX_BLUE       0x001F
#define ST77XX_CYAN       0x07FF
#define ST77XX_MAGENTA    0xF81F
#define ST77XX_YELLOW     0xFFE0
#define ST77XX_ORANGE     0xFC00


#define CWHITE0  0xFFFF //- БЕЛЫЙ
#define CWHITE1  0xE79F //- БЕЛЫЙ
#define CBLACK   0x0000 //- черный
#define CGRAY    0x738E //- серый
#define CYELLOW0 0xFFE0 //- желтый - светлый
#define CYELLOW1 0x07FF //- бирюзовый - yellow
#define CRED0    0xF800 //- красный - red
#define CRED     0x00F8 //- красный - red
#define CBLUE0   0x001F //- синий - blue
#define CBLUE1   0xFFE5 //- синий - blue
#define CCYAN    0xFFE0 //- голубой - CYAN
#define CGREEN0  0x07E0 //- зеленый - green
#define CGREEN1  0x0F13 //- зеленый - green
#define CGREEN2  0x0F8E //- зеленый - green
#define CGREEN3  0x0F26 //- зеленый - green
#define CMAGENTA 0xF81F //- пурпурный - MAGENTA
#define CRUBY    0x30ba //- рубин - ruby
#define CPURPLE  0xC05E //- фиолетовый - purple
                 
void lcd7735_init(uint16_t color); // инициализация дисплея
void lcd7735_sendCmd(unsigned char cmd); // отправка команды на дисплей
void lcd7735_sendData(unsigned char data) ;
void lcd7735_at(unsigned char startX, unsigned char startY, unsigned char stopX, unsigned char stopY);
void lcd7735_fillrect(unsigned char startX, unsigned char startY, unsigned char stopX, unsigned char stopY, unsigned int color); // заполнение прямоугольной области экрана
void lcd7735_putpix(unsigned char x, unsigned char y, unsigned int Color); // вывод пиксела
void lcd7735_line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned int color); // вывод линии
void lcd7735_rect(char x1,char y1,char x2,char y2, unsigned int color); // рисование прямоугольника (не заполненного)

#endif // __LCD_ST7735SL__
#include "lcd7735sl.h"      // объявления модуля

// some flags for initR() :(
#define INITR_GREENTAB 0x00
#define INITR_REDTAB 0x01
#define INITR_BLACKTAB 0x02
#define INITR_18GREENTAB INITR_GREENTAB
#define INITR_18REDTAB INITR_REDTAB
#define INITR_18BLACKTAB INITR_BLACKTAB
#define INITR_144GREENTAB 0x01
#define INITR_MINI160x80 0x04
#define INITR_HALLOWING 0x05

// Some register settings
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH 0x04

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5

#define ST7735_PWCTR6 0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Some ready-made 16-bit ('565') color settings:
#define ST7735_BLACK ST77XX_BLACK
#define ST7735_WHITE ST77XX_WHITE
#define ST7735_RED ST77XX_RED
#define ST7735_GREEN ST77XX_GREEN
#define ST7735_BLUE ST77XX_BLUE
#define ST7735_CYAN ST77XX_CYAN
#define ST7735_MAGENTA ST77XX_MAGENTA
#define ST7735_YELLOW ST77XX_YELLOW
#define ST7735_ORANGE ST77XX_ORANGE


#define ST_CMD_DELAY 0x80 // special signifier for command lists

#define ST77XX_NOP 0x00
#define ST77XX_SWRESET 0x01
#define ST77XX_RDDID 0x04
#define ST77XX_RDDST 0x09

#define ST77XX_SLPIN 0x10
#define ST77XX_SLPOUT 0x11
#define ST77XX_PTLON 0x12
#define ST77XX_NORON 0x13

#define ST77XX_INVOFF 0x20
#define ST77XX_INVON 0x21
#define ST77XX_DISPOFF 0x28
#define ST77XX_DISPON 0x29
#define ST77XX_CASET 0x2A
#define ST77XX_RASET 0x2B
#define ST77XX_RAMWR 0x2C
#define ST77XX_RAMRD 0x2E

#define ST77XX_PTLAR 0x30
#define ST77XX_TEOFF 0x34
#define ST77XX_TEON 0x35
#define ST77XX_MADCTL 0x36
#define ST77XX_COLMOD 0x3A

#define ST77XX_MADCTL_MY 0x80
#define ST77XX_MADCTL_MX 0x40
#define ST77XX_MADCTL_MV 0x20
#define ST77XX_MADCTL_ML 0x10
#define ST77XX_MADCTL_RGB 0x00

#define ST77XX_RDID1 0xDA
#define ST77XX_RDID2 0xDB
#define ST77XX_RDID3 0xDC
#define ST77XX_RDID4 0xDD











extern void delay_ms(uint32_t ms);
#define SPIDR8BIT (*(__IO uint8_t *)((uint32_t)&SPI1->DR))

// отправка данных\команд на дисплей
void lcd7735_senddata(unsigned char data)
{
  while (!(SPI1->SR & SPI_SR_TXE));
  SPIDR8BIT = data;
  while (!(SPI1->SR & SPI_SR_RXNE));
  uint8_t temp = SPIDR8BIT;
}

// отправка команды на дисплей с ожиданием конца передачи
void lcd7735_sendCmd(unsigned char cmd)
{
  DC_DN;
  lcd7735_senddata(cmd);
  while(SPI1->SR & SPI_SR_BSY);
}

// отправка данных на дисплей с ожиданием конца передачи
void lcd7735_sendData(unsigned char data) 
{
  DC_UP;
  lcd7735_senddata(data);
  while(SPI1->SR & SPI_SR_BSY);
}

// определение области экрана для заполнения
void lcd7735_at(unsigned char startX, unsigned char startY, unsigned char stopX, unsigned char stopY) 
{
  lcd7735_sendCmd(0x2A); // ST77XX_CASET
  DC_UP;
  lcd7735_senddata(0x00); // ST77XX_MADCTL_RGB
  lcd7735_senddata(startX);
  lcd7735_senddata(0x00);
  lcd7735_sendData(stopX);

  lcd7735_sendCmd(0x2B); // ST77XX_RASET
  DC_UP;
  lcd7735_senddata(0x00);
  lcd7735_senddata(startY);
  lcd7735_senddata(0x00);
  lcd7735_sendData(stopY);
}

// Инициализация
void lcd7735_init(uint16_t color) 
{
  CS_DN; // CS=0  - начали сеанс работы с дисплеем
  // сброс дисплея
  // аппаратный сброс дисплея
  RST_DN;
  delay_ms(LCD_RST_DLY); // пауза
  RST_UP; // RST=1
  delay_ms(LCD_RST_DLY); // пауза
  
  // инициализация дисплея
  lcd7735_sendCmd(0x11); // после сброса дисплей спит - даем команду проснуться
  delay_ms(10); // пауза
  lcd7735_sendCmd (0x3A); // режим цвета:
  lcd7735_sendData(0x05); //             16 бит
  lcd7735_sendCmd (0x36); // направление вывода изображения:
#ifdef RGB
  lcd7735_sendData(0x1C); // снизу вверх, справа на лево, порядок цветов RGB
#else
  lcd7735_sendData(0x14); // снизу вверх, справа на лево, порядок цветов BGR
#endif
  lcd7735_sendCmd (0x29); // включаем изображение
  CS_UP;
  lcd7735_fillrect(0, 0, 127, 159, color);
}

// процедура заполнения прямоугольной области экрана заданным цветом
void lcd7735_fillrect(unsigned char startX, unsigned char startY, unsigned char stopX, unsigned char stopY, unsigned int color)
{
  unsigned char y;
  unsigned char x;
  CS_DN;
  lcd7735_at(startX, startY, stopX, stopY);
  lcd7735_sendCmd(0x2C);
  DC_UP;
  SPI2SIXTEEN;

  for (y=startY;y<stopY+1;y++)
    for (x=startX;x<stopX+1;x++) 
    {
      while (!(SPI1->SR & SPI_SR_TXE));
      SPI1->DR = color;
    }
  while (!(SPI1->SR & SPI_SR_TXE) || (SPI1->SR & SPI_SR_BSY));
  SPI2EIGHT;
  CS_UP;
}

// вывод пиксела
void lcd7735_putpix(unsigned char x, unsigned char y, unsigned int Color)
{
  CS_DN;

  lcd7735_at(x, y, x, y);
  lcd7735_sendCmd(0x2C);
  lcd7735_sendData((unsigned char)((Color & 0xFF00)>>8));
  lcd7735_sendData((unsigned char) (Color & 0x00FF));

  CS_UP;
}

// процедура рисования линии
void lcd7735_line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned int color) {
  signed char   dx, dy, sx, sy;
  unsigned char  x,  y, mdx, mdy, l;

  if (x1==x2) { // быстрая отрисовка вертикальной линии
    lcd7735_fillrect(x1,y1, x1,y2, color);
    return;
  }
  if (y1==y2) { // быстрая отрисовка горизонтальной линии
    lcd7735_fillrect(x1,y1, x2,y1, color);
    return;
  }
  dx=x2-x1; dy=y2-y1;
  if (dx>=0) { mdx=dx; sx=1; } else { mdx=x1-x2; sx=-1; }
  if (dy>=0) { mdy=dy; sy=1; } else { mdy=y1-y2; sy=-1; }
  x=x1; y=y1;
  if (mdx>=mdy) {
    l=mdx;
    while (l>0) {
      if (dy>0) { y=y1+mdy*(x-x1)/mdx; }
      else      { y=y1-mdy*(x-x1)/mdx; }
      lcd7735_putpix(x,y,color);
      x=x+sx;
      l--;
    }
  } else {
    l=mdy;
    while (l>0) {
      if (dy>0) { x=x1+((mdx*(y-y1))/mdy); }
      else      { x=x1+((mdx*(y1-y))/mdy); }
      lcd7735_putpix(x,y,color);
      y=y+sy;
      l--;
    }
  }
  lcd7735_putpix(x2, y2, color);
}

// рисование прямоугольника (не заполненного)
void lcd7735_rect(char x1,char y1,char x2,char y2, unsigned int color)
{
  lcd7735_fillrect(x1,y1, x2,y1, color);
  lcd7735_fillrect(x1,y2, x2,y2, color);
  lcd7735_fillrect(x1,y1, x1,y2, color);
  lcd7735_fillrect(x2,y1, x2,y2, color);
}






// NEW

#define COMM      0x00
#define DATA      0x01


void st7735send(uint8_t dc, uint8_t data)
{
  if (dc == DATA) DC_UP;  // gpio_set(GPIOB,DC);
  else            DC_DN;  // gpio_reset(GPIOB,DC);

  while (!(SPI1->SR & SPI_SR_TXE));
  SPIDR8BIT = data;
  while (!(SPI1->SR & SPI_SR_RXNE));
  uint8_t temp = SPIDR8BIT;
  //SPI1->DR=data;
  //while (!(SPI1->SR & SPI_I2S_FLAG_TXE) || (SPI1->SR & SPI_I2S_FLAG_BSY));
}

void st7735init(unsigned int orientation, unsigned int color) {
  CS_DN; // CS=0  - начали сеанс работы с дисплеем
  // сброс дисплея
  // аппаратный сброс дисплея
  RST_DN;
  delay_ms(LCD_RST_DLY); // пауза

  RST_UP; // RST=1
  delay_ms(LCD_RST_DLY); // пауза

  st7735send(COMM,ST77XX_SWRESET);
  delay_ms(10);

  st7735send(COMM, 0x11); // SLPOUT (0x11) после сброса дисплей спит - даем команду проснуться  // SLPIN (10h) / SLPOUT (11h)
  delay_ms(10);

  st7735send(COMM, 0x3A); // COLMOD (0x3A) режим цвета:
  st7735send(DATA, 0x05); // 16 бит
  
  // MADCTL (36h) – установка режима адресации и, соответственно, порядка вывода данных на дисплей, эта команда определяет ориентацию изображения на экране,
  // кроме того, бит RGB параметра этой команды ответственен за распределение интенсивности между субпикселями (красным, зелёным и синим)
  st7735send(COMM, 0x36); // MADCTL (0x36) направление вывода изображения:
  if (orientation ==  PORTRAIT)  st7735send(DATA, 0xC0);
  if (orientation == LANDSCAPE)  st7735send(DATA, 0x60);

  // 00 = upper left printing right
  // 10 = does nothing (MADCTL_ML)
  // 20 = upper left printing down (backwards) (Vertical flip)
  // 40 = upper right printing left (backwards) (X Flip)
  // 80 = lower left printing right (backwards) (Y Flip)
  // 04 = (MADCTL_MH)
  // 60 = 90 right rotation
  // C0 = 180 right rotation
  // A0 = 270 right rotation

  st7735send(COMM, ST7735_FRMCTR1);
  st7735send(DATA, 0x01);
  st7735send(DATA, 0x2C);
  st7735send(DATA, 0x2D);

  st7735send(COMM, ST7735_FRMCTR2);
  st7735send(DATA, 0x01);
  st7735send(DATA, 0x2C);
  st7735send(DATA, 0x2D);

  st7735send(COMM, ST7735_FRMCTR3);
  st7735send(DATA, 0x01);
  st7735send(DATA, 0x2C);
  st7735send(DATA, 0x2D);
  st7735send(DATA, 0x01);
  st7735send(DATA, 0x2C);
  st7735send(DATA, 0x2D);

  st7735send(COMM, ST7735_INVCTR);
  st7735send(DATA, 0x07);

  st7735send(COMM, ST7735_PWCTR1);
  st7735send(DATA, 0xA2);
  st7735send(DATA, 0x02);
  st7735send(DATA, 0x84);

  st7735send(COMM, ST7735_PWCTR2);
  st7735send(DATA, 0xC5);

  st7735send(COMM, ST7735_PWCTR3);
  st7735send(DATA, 0x0A);
  st7735send(DATA, 0x00);

  st7735send(COMM, ST7735_PWCTR4);
  st7735send(DATA, 0x8A);
  st7735send(DATA, 0x2A);

  st7735send(COMM, ST7735_PWCTR5);
  st7735send(DATA, 0x8A);
  st7735send(DATA, 0xEE);

  st7735send(COMM, ST7735_VMCTR1);
  st7735send(DATA, 0x0E);

  st7735send(COMM, ST77XX_INVOFF);  // INVOFF (20h) / INVON (21h) – выключение/включение инверсии дисплея. 

  st7735send(COMM, ST7735_GMCTRP1);
  st7735send(DATA, 0x02);
  st7735send(DATA, 0x1C);
  st7735send(DATA, 0x07);
  st7735send(DATA, 0x12);
  st7735send(DATA, 0x37);
  st7735send(DATA, 0x32);
  st7735send(DATA, 0x29);
  st7735send(DATA, 0x2D);
  st7735send(DATA, 0x29);
  st7735send(DATA, 0x25);
  st7735send(DATA, 0x2B);
  st7735send(DATA, 0x39);
  st7735send(DATA, 0x00);
  st7735send(DATA, 0x01);
  st7735send(DATA, 0x03);
  st7735send(DATA, 0x10);

  st7735send(COMM, ST7735_GMCTRN1);
  st7735send(DATA, 0x03);
  st7735send(DATA, 0x1D);
  st7735send(DATA, 0x07);
  st7735send(DATA, 0x06);
  st7735send(DATA, 0x2E);
  st7735send(DATA, 0x2C);
  st7735send(DATA, 0x29);
  st7735send(DATA, 0x2D);
  st7735send(DATA, 0x2E);
  st7735send(DATA, 0x2E);
  st7735send(DATA, 0x37);
  st7735send(DATA, 0x3F);
  st7735send(DATA, 0x00);
  st7735send(DATA, 0x00);
  st7735send(DATA, 0x02);
  st7735send(DATA, 0x10);
  
  st7735send(COMM, ST77XX_NORON);
  delay_ms(10);

  st7735send(COMM, ST77XX_DISPON);
  delay_ms(10);

  CS_DN; // chip_select_disable();

  if (orientation ==  PORTRAIT) lcd7735_fillrect(0, 0, 127, 159, color);
  if (orientation == LANDSCAPE) lcd7735_fillrect(0, 0, 159, 127, color);
}
// EOF
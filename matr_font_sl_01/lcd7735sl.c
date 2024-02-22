#include "lcd7735sl.h"      // объявления модуля

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
  //while(SPI1->SR & SPI_SR_BSY);
}

// определение области экрана для заполнения
void lcd7735_at(unsigned char startX, unsigned char startY, unsigned char stopX, unsigned char stopY) 
{
  lcd7735_sendCmd(0x2A);
  DC_UP;
  lcd7735_senddata(0x00);
  lcd7735_senddata(startX);
  lcd7735_senddata(0x00);
  lcd7735_sendData(stopX);

  lcd7735_sendCmd(0x2B);
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

// EOF
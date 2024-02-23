#include "main.h"
#include "myfont.h"
#include "lcd7735sl.h"
#include "consolas_22_font.h"

void print_char_temp(unsigned char CH,            // символ который выводим
                unsigned char X, unsigned char Y, // координаты
                unsigned char SymbolWidth,        // ширина символа
                unsigned char SymbolHeight,       // высота символа
                unsigned char MatrixLength,       // длина матрицы символа
                const unsigned char font[],       // шрифт
                const unsigned int index[],       // индексный массив шрифта
                unsigned int fcolor,              // цвет шрифта
                unsigned int bcolor)              // цвет фона
{
  unsigned char MatrixByte;   // временная для обработки байта матрицы
  unsigned char BitMask;      // маска, накладываемая на байт для получения значения одного бита
  unsigned char BitWidth;     // счётчик выведенных по горизонтали точек
  const unsigned char *MatrixPointer = font;
  unsigned char color;
  BitWidth = SymbolWidth;     // начальная установка счётчика
  MatrixPointer += index[CH]; // перемещаем указатель на начало символа CH
  // начало алгоритма вывода. Вывод без поворота
  // x матрицы = x символа (ширина)
  // y матрицы = y символа (высота)
  CS_DN;
  lcd7735_at(X, Y, X + SymbolWidth - 1, Y - SymbolHeight - 1); // Ширина и высота шрифта от 0
  lcd7735_sendCmd(0x2C);
  SPI2SIXTEEN; // SPI в 16 бит
  DC_UP;
  
  do {
    MatrixByte    = *MatrixPointer;     // чтение очередного байта матрицы
    MatrixPointer =  MatrixPointer + 1; // после чтения передвинем указатель на следующий байт матрицы
    MatrixLength  =  MatrixLength  - 1; // можно писать просто MatrixLength--; но мне так не нравится...
    BitMask       =  0b10000000;        // предустановка маски и ширины символа на начало для каждого прочитанного байта
    do {
      while (!(SPI1->SR & SPI_SR_TXE)) {}; // ждём TXE перед отправкой (проверка есть ли данные в SPI)
      if ((MatrixByte & BitMask) > 0) SPI1->DR = fcolor; // вывод в поток точки цвета символа
      else                            SPI1->DR = bcolor; // вывод в поток точки цвета фона
      BitMask  = BitMask >> 1; // как только бит выедет вправо, BitMask станет равен 0. Значит, вывели все 8 битов в поток
      BitWidth = BitWidth - 1; // как только значение станет равным 0. Значит, все биты строки вывели в поток
    } while ((BitWidth > 0) && (BitMask > 0)); // если хоть что-то стало 0, надо читать следующий байт - выходим из этого цикла
    if (BitWidth == 0) BitWidth = SymbolWidth; // если выход был по окончании вывода строки - снова предустановим счётчик
                                               // один раз на символ предустановка лишняя, но организовывать проверку
                                               // окажется много дороже, чем один лишний раз присвоить значение
  } while (MatrixLength > 0);
  // закончили
  while (!(SPI1->SR & SPI_SR_TXE) || (SPI1->SR & SPI_SR_BSY)) {};
  CS_UP;
  SPI2EIGHT;
} // print_char_temp()

int main(void) 
{
  rcc_sysclockinit();
  SysTick_Config(48000); // 1ms if HSI
  gpio_init();
  spi_init();
  delay_ms(10);
  lcd7735_init(CBLUE0);
  unsigned char x = 32;
  do // do main 
  { 
    if (count > ttms || ttms - count > 1000) {
      //print_char_temp(x, 20, 30, 24, 32, 96, InconsolaNum24x32, InconsolaNum24x32index, CORANGE, CBLACK); 
      //print_char_temp(x, 20+24, 30, 24, 32, 96, InconsolaNum24x32, InconsolaNum24x32index, CBLACK, CWHITE0); 

      print_char_temp(x, 0, 0, 18, 35, 90, font_consolas_22, consolas22index, CORANGE, CBLACK);
      count = ttms;
      x++; if (x > 253) x = 32;
    }
  } while (1); // main do
} // main

// EoF
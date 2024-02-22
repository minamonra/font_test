#include <stdio.h>
#include <stdlib.h>
#include <stm32f0xx.h>
#include "myfont.h"
#include "consolas_22_font.h"


void print_char(unsigned char CH,
                uint8_t X, uint8_t Y, 
                unsigned char SymbolWidth, 
                unsigned char MatrixLength,
                const unsigned char font[], 
                const unsigned int index[]) 
{
  unsigned char MatrixByte; // временная для обработки байта матрицы
  unsigned char BitMask;    // маска, накладываемая на байт для получения значения одного бита
  unsigned char BitWidth;   // счётчик выведенных по горизонтали точек
  const unsigned char *MatrixPointer = font;//InconsolaNum24x32;
  BitWidth = SymbolWidth;   // начальная установка счётчика
  MatrixPointer += index[CH];//InconsolaNum24x32index[CH];  // перемещаем указатель на начало символа CH

  // начало алгоритма вывода. Вывод без поворота
  // x матрицы = x символа (ширина)
  // y матрицы = y символа (высота)

  do {
    MatrixByte = *MatrixPointer;       // чтение очередного байта матрицы
    MatrixPointer = MatrixPointer + 1; // после чтения передвинем указатель на следующий байт матрицы
    MatrixLength = MatrixLength - 1;   // можно писать просто MatrixLength--; но мне так не нравится...
    BitMask = 0b10000000;              // предустановка маски и ширины символа на начало для каждого прочитанного байта

    do {
      if ((MatrixByte & BitMask) > 0) {

      } // вывод в поток точки цвета символа
      else {

      }; // вывод в поток точки цвета фона

      BitMask = BitMask >> 1;  // как только бит выедет вправо, BitMask станет равен 0. Значит, вывели все 8 битов в поток
      BitWidth = BitWidth - 1; // как только значение станет равным 0. Значит, все биты строки вывели в поток

    } while ((BitWidth > 0) && (BitMask > 0)); // если хоть что-то стало 0, надо читать следующий байт - выходим из этого цикла

    if (BitWidth == 0) BitWidth = SymbolWidth; // если выход был по окончании вывода строки - снова предустановим счётчик
                                               // один раз на символ предустановка лишняя, но организовывать проверку
                                               // окажется много дороже, чем один лишний раз присвоить значение
  } while (MatrixLength > 0);
}

int main(void) 
{
  do // do main 
  { 
    // InconsolaNum24x32 массив битмапов символов
    // InconsolaNum24x32index его игдексы
    print_char('8', 0, 0, 24, 96, InconsolaNum24x32, InconsolaNum24x32index);
  } while (1); // main do
}

// End of file
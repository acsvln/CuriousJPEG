﻿#include <boost/numeric/ublas/matrix.hpp>
using std::cout;
using std::endl;

int main() {

    boost::numeric::ublas::matrix<int> A(64,64);


int x, y,  // Координаты текущего элемента массива
    value = 1; // значение, которым заполняется массив

// зполнение первой половины массива по диагонали, зигзагом, начиная
// слева и сверху, заканчивая  побочной диагональю
for (int diag = 0; diag < A.size2(); diag++) // выполняем проход по диагоналям
{
    if (diag % 2 == 0) // по четным диагоналям
    {
        x = 0; // х-координата первого лемента массива на диагонали - diag
        y = diag; // у-координата элемента массива на диагонали - diag

        while (y >= 0) // пока y-координата находится в верхней части диагонали
        {
            A(x,y) = value;
           //array[x][y] = value; // записать значение в массив
            value++;
            x++;     // по горизонтали, смещаемся влево
            y--;    // по вертикали, смещаемся вниз
        }
    }
    else // по нечетным диагоналям
    {
        x = diag; // х-координата элемента массива на диагонали - diag
        y = 0; // у-координата первого элемента массива на диагонали - diag

        while (x >= 0) // пока x-координата находится в левой части диагонали
        {
            A(x,y) = value; // записать значение в массив
            value++;
            x -= 1;  // по горизонтали, смещаемся вправо
            y += 1; // по вертикали, смещаемся вверх
        }
    }
} // конец for

// заполнение второй половины массива по диагонали, зигзагом, начиная
// слева и сверху, заканчивая  последним элементом массива
for (int diag = 1; diag < A.size2(); diag++)
{
    if (diag % 2 == 0) // по четным диагоналям
    {
        x = 9; // х-координата первого элемента массива на диагонали - diag
        y = diag;  // у-координата элемента массива на диагонали - diag

        while (y <= 9) // Пока не кончилась диагональ
        {
            A(x,y) = value;
            value++;
            x--; // по горизонтали, смещаемся влево
            y++; // по вертикали, смещаемся вниз
        }
    }
    else // по не четным диагоналям
    {
        x = diag; // х-координата первого элемента к-ой диагонали
        y = 9;  // у-координата первого элемента к-ой диагонали

        while (x <= 9) // Пока не кончилась диагональ
        {
            A(x,y) = value;
            value++;
            x++; // по горизонтали, смещаемся вправо
            y--; // по вертикали, смещаемся вверх
        }
    } // конец if-else
} // конец цикла for (заполнение второй половины массива)

// вывод масиива на экран
for (int ix = 0; ix < A.size1(); ix++)
{
    for (int jx = 0; jx < A.size2(); jx++)
        cout  << A(ix,jx) << " " << endl;
    /*cout  << array[ix][jx] << " " << endl;*/


}

return 0;
}




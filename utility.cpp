#include <iostream>


#include <iomanip>
#include <istream>
#include "utility.hpp"

std::experimental::optional<std::pair<unsigned, unsigned> > AddToZigZagMatrix(
          boost::numeric::ublas::matrix<uint16_t> matrix
        , std::pair<unsigned, unsigned> position
        , uint16_t value)
{

    return {};
    //#define Up true
    //#define Down false
    //#define setQuantTableItem(bT,aT,ix,iy,iT) ({aT[iy][ix]=bT;iT++;})
    //    typedef bool Direction;

    //    static Direction fDirection=Down;
    //    static int iT=0;
    //    static int ix=0,iy=0;
    //    static int state=0;
    //    switch (state) {
    //    case 0: setQuantTableItem(bT,aTable,ix,iy,iT);/* начало функции */
    //        //return true;
    //        while (iT<cnMatrixDimension-2)
    //        {
    //            if (fDirection==Down)
    //            {
    //                if (ix<cnMatrixSide-1)++ix;else iy++;
    //                for (ix,iy;ix>=0&&iy<cnMatrixSide;ix--,iy++)
    //                {

    //                    state = 1;/* возвратиться к "case 1" */
    //                    return false;
    //                case 1: setQuantTableItem(bT,aTable,ix,iy,iT);/* продолжить выполнения после точки возврата */;/* возвратиться к "case 1" */
    //                }
    //                ix++;
    //                iy--;
    //                fDirection=Up;
    //            }
    //            else
    //            {
    //                if (iy<cnMatrixSide-1)++iy;else ++ix;
    //                for (ix,iy;ix<cnMatrixSide&&iy>=0;ix++,iy--)
    //                {
    //                    state = 2;/* возвратиться к "case 1" */
    //                    return false;
    //                case 2: setQuantTableItem(bT,aTable,ix,iy,iT);
    //                }
    //                ix--;
    //                iy++;
    //                fDirection=Down;
    //            }
    //        }
    //        ix++;
    //        state=3;
    //        return false;
    //    case 3: setQuantTableItem(bT,aTable,ix,iy,iT);
    //    }
    //    fDirection=Down;
    //    iT=0;
    //    ix=0;
    //    iy=0;
    //    state=0;
    //    return true;
    //#undef Up
    //#undef Down
    //#undef setQuantTableItem
}




#if 0

using namespace std;

int main()
{
	const int colum = 10; // количество столбцов массива
	const int row = 10; // количество строк массива
	int array[100][100];

	int x, y,  // Координаты текущего элемента массива
		value = 1; // значение, которым заполняется массив

	// зполнение первой половины массива по диагонали, зигзагом, начиная
	// слева и сверху, заканчивая  побочной диагональю
	for (int diag = 0; diag < colum; diag++) // выполняем проход по диагоналям
	{
		if (diag % 2 == 0) // по четным диагоналям
		{
			x = 0; // х-координата первого лемента массива на диагонали - diag
			y = diag; // у-координата элемента массива на диагонали - diag

			while (y >= 0) // пока y-координата находится в верхней части диагонали
			{
				array[x][y] = value; // записать значение в массив
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
				array[x][y] = value; // записать значение в массив
				value++;
				x -= 1;  // по горизонтали, смещаемся вправо
				y += 1; // по вертикали, смещаемся вверх
			}
		}
	} // конец for

	// заполнение второй половины массива по диагонали, зигзагом, начиная
	// слева и сверху, заканчивая  последним элементом массива
	for (int diag = 1; diag < colum; diag++)
	{
		if (diag % 2 == 0) // по четным диагоналям
		{
			x = 9; // х-координата первого элемента массива на диагонали - diag
			y = diag;  // у-координата элемента массива на диагонали - diag

			while (y <= 9) // Пока не кончилась диагональ
			{
				array[x][y] = value;
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
				array[x][y] = value;
				value++;
				x++; // по горизонтали, смещаемся вправо
				y--; // по вертикали, смещаемся вверх
			}
		} // конец if-else
	} // конец цикла for (заполнение второй половины массива)

	// вывод масиива на экран
	for (int ix = 0; ix < row; ix++)
	{
		for (int jx = 0; jx < colum; jx++)
            cout  << array[ix][jx] << " " << endl;
		/*cout  << array[ix][jx] << " " << endl;*/


	}

	return 0;
}

#endif

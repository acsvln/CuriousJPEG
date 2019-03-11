//#include "pch.h"
#include<iostream>
#include<conio.h>
#include<iomanip>
#include <boost/numeric/ublas/matrix.hpp>
#include <array>
using std::cout;
using std::endl;
using std::array;
using std::setw;
boost::numeric::ublas::matrix<int> A(10,10);

array<int,11> num = {1,2,3,4,5,6,7,8,9,10};


class ArrZigzag
{
private:
    static const int SIZE = 10;
    int arr[SIZE][SIZE];
    int x;
public:
    ArrZigzag()
    {
        int count = 0;
        const int maxtrix = SIZE * SIZE;
        for (int i = 0; i < SIZE; i++)
        {
            x = i;
            if (i % 2)
                for (int j = 0; j <= i; j++, x--)
                {
                    arr[x][j] = count++;
                    arr[SIZE - 1 - x][SIZE - 1 - j] = (maxtrix - count + 1);
                }
            else
                for (int j = 0; j <= i; j++, x--)
                {
                    arr[j][x] = count++;
                    arr[SIZE - 1 - j][SIZE - 1 - x] = (maxtrix - count + 1);
                }
        }
    }

    void Show() const
    {
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
                cout<< setw(4) << arr[i][j];
            cout << endl;
        }
    }
};

int test(int argc, char** argv)
{


    ArrZigzag arrZ;
    arrZ.Show();
    //END//
    _getch();
    return 0;
}

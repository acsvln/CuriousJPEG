//#include "pch.h"
#include<iostream>
#include<conio.h>
#include<iomanip>
#include <boost/numeric/ublas/matrix.hpp>
#include <array>
using std::cout;
using std::endl;
using std::array;
using namespace std;



static int x;
static const int SIZE = 8;
static boost::numeric::ublas::matrix<int> A(SIZE, SIZE);

//const array<int, 65> num = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,
//14,15,16,17,18,19,20,21,22,
//23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,
//46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64 };


int   ArrZigzag(array<int,65>num)
{
	int count = 0;
	for (int i = 0; i < SIZE; i++)
	{
		x = i;
		if (i % 2)
			for (int j = 0; j <= i; j++, x--)
			{
				A(x, j) = count++;
                A(SIZE - 1 - x, SIZE - 1 - j) = (num[65] - count + 1);
			}
		else
			for (int j = 0; j <= i; j++, x--)
			{
				A(j, x) = count++;
                A(SIZE - 1 - j, SIZE - 1 - x) = (num[65] - count + 1);
			}
	}
// вывод 
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
			cout << setw(4) << A(i, j);
		cout << endl;
	}
    return 0;
}





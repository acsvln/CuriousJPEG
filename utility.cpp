//#include "pch.h"
#include<iostream>
#include<iomanip>
#include <boost/numeric/ublas/matrix.hpp>
#include <array>
using std::array;


 static uint16_t x;
 static const uint16_t SIZE = 8;


    boost::numeric::ublas::matrix<uint16_t> CreateZigZagMatrix( std::array<uint16_t, 64> source )

    {
        boost::numeric::ublas::matrix<uint16_t> matrix(SIZE,SIZE);


            uint16_t count = 0;
            for (uint16_t i = 0; i < SIZE; i++)
            {
                x = i;
                if (i % 2)
                    for (uint16_t j = 0; j <= i; j++, x--)
                    {
                        matrix(x, j) = count++;
                        matrix(SIZE - 1 - x, SIZE - 1 - j) = (source[64] - count + 1);
                    }
                else
                    for (uint16_t j = 0; j <= i; j++, x--)
                    {
                        matrix(j, x) = count++;
                        matrix(SIZE - 1 - j, SIZE - 1 - x) = (source[64] - count + 1);
                    }
            }

            


        return matrix;
    }




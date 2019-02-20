#include <QtCore/QCoreApplication>
#include "cstdlib"
#include <iostream>
#include <fstream>
#include <istream>
#include <clocale>
#include <vector>
#include <cmath>
#include <QDebug>
#include "union.h"

class Factorial
{
public:
    int Fac(int F){
        return F * Fac(F-1);
    }
};

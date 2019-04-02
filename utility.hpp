#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstddef>
#include <boost/numeric/ublas/matrix.hpp>

// Херово, у нас будут проблемы со сборкой на двух платформах
//     я хочу на линукс перейти
// скажи какие приложения для c++ там нужны и совместимости qt creator, conan, boost ?

// ДА всё как и в винде, не парься. ставь конан и кути.
// подойдёт любая версия но нужна поддержка 17 стандарта
// у тебя проект собирается, можешь писать тесты под студийный компилятор
// /очитай про препроцессор и напиши условную компиляцю.
// /удет как то так тут

//#ifdef MSVC
//#include <optional>
//using optional = std::optional
//#else
//#include <experimental/optional>
//using optional = std::experimental::optional;
//#endif

#include <optional>

namespace mystd {
template<typename T>
using optional = std::optional<T>;
}

// TODO: remove that helper
template <typename T>
        T swap_endian(T u)
{
    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (std::size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}


        // nu i tut vstav optional vmesto std::optional
        // ты на другом компиляторе собрал ? А разница какая их ?
        //

mystd::optional<std::pair<unsigned,unsigned>> AddToZigZagMatrix(boost::numeric::ublas::matrix<uint16_t> matrix, std::pair<unsigned,unsigned> position, uint16_t value);
boost::numeric::ublas::matrix<uint16_t> CreateZigZagMatrix( std::array<uint16_t, 64> source );

#endif // UTILITY_HPP

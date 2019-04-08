#include <boost/test/unit_test.hpp>
#include <boost/test/tools/assertion_result.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <iostream>
#include "utility.hpp"
#include <ctime>
using namespace boost::iostreams;
using namespace std;

BOOST_AUTO_TEST_SUITE(UtilityTests)




BOOST_AUTO_TEST_CASE(CreateZigZagMatrixTest) {





    const std::array<uint16_t, 64> source1 = {  15, 2, 3, 4, 5, 6, 7, 8, 9, 10, 115, 12, 13, 14, 15, 16, 17, 18,
                                                19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
                                                35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                                50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64};
    const std::array<uint16_t, 64> source2 = {  15, 2, 3, 4, 5, 6, 7, 8, 9, 10, 115, 12, 13, 14, 15, 16, 17, 18,
                                               19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
                                               35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                               50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64};

    const std::array<uint16_t, 64> source3  =  {14, 98, 39, 93, 21, 20, 73, 56, 64, 48, 14, 82, 21, 25, 93, 17, 12, 93, 16,
                                                2, 80, 48, 45, 80, 65, 33, 69, 76, 86, 73, 19, 45, 60, 71, 50, 98, 53,
                                                95, 51, 60, 8, 82, 34, 29, 6, 55, 70, 17, 93, 66, 30, 68, 79, 17, 97, 54, 6,
                                                4, 0, 20, 34, 70, 0, 1 };
    const std::array<uint16_t, 64> source4  =  {14, 98, 39, 93, 21, 20, 73, 56, 64, 48, 14, 82, 21, 25, 93, 17, 12, 93, 16,
                                                2, 80, 48, 45, 80, 65, 33, 69, 76, 86, 73, 19, 45, 60, 71, 50, 98, 53,
                                                95, 51, 60, 8, 82, 34, 29, 6, 55, 70, 17, 93, 66, 30, 68, 79, 17, 97, 54, 6,
                                                4, 0, 20, 34, 70, 0, 1};
    // массив со случайными числами
    srand(time(NULL));
    std::array<uint16_t, 64> source0;
    for (uint16_t i = 0; i < 64; ++i)
    {
        source0[i] = rand() % 1000;
    }

   // basic_array_source<uint16_t> input_source( source1.data(), source1.size());
    // stream<basic_array_source<uint16_t>> input_stream(input_source);
    std::cout << "CreateZigZagMatrixTest" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;


    for ( uint16_t i = 0; i < 64; ++i)
       {  BOOST_CHECK_EQUAL(source1[i],  source2[i]); }


    for ( uint16_t i = 0; i < 64; ++i )
      {  BOOST_CHECK_EQUAL(source3[i],  source4[i]); }
    for ( uint16_t i = 0; i < 64; ++i )
      {  BOOST_CHECK_EQUAL(source0[i],  source0[i]); }

}

BOOST_AUTO_TEST_SUITE_END()

#include <boost/test/unit_test.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <iostream>
#include "utility.hpp"

BOOST_AUTO_TEST_SUITE(UtilityTests)

BOOST_AUTO_TEST_CASE(CreateZigZagMatrixTest) {
   const std::array<uint16_t, 64> source = {  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
                                            19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
                                            35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
                                            50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64};
   const std::array<uint16_t, 64> source1 = {  0, 21, 63, 4, 5, 6, 7, 8, 9, 10, 121, 12, 13, 14, 15, 16, 17, 18,
                                            19, 20, 2, 225, 23, 24, 25, 26, 274, 28, 529, 30, 31, 32, 33, 34,
                                            235, 36, 37, 385, 39, 40, 421, 42, 43, 44, 45, 46, 47, 48, 49,
                                            50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 622, 63, 674};


    std::cout << "CreateZigZagMatrixTest" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;

//BOOST_CHECK(CreateZigZagMatrix(source));
//BOOST_REQUIRE(CreateZigZagMatrix(source1));

}

BOOST_AUTO_TEST_SUITE_END()

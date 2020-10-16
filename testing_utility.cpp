#include "testing_utility.hpp"

#include <boost/numeric/ublas/io.hpp>
#include <iostream>
#include <tiffio.h>
#include <vector>

namespace boost::numeric::ublas {

namespace {
template <class T>
auto comparsionImpl(matrix<T> const &Left, matrix<T> const &Right) -> bool {
  using SizeType = typename matrix<T>::size_type;

  if (Left.size1() != Right.size1()) {
    return false;
  }

  if (Left.size2() != Right.size2()) {
    return false;
  }

  for (SizeType I = 0; I < Left.size1(); ++I) {
    for (SizeType J = 0; J < Left.size2(); ++J) {
      if (Left(I, J) != Right(I, J)) {
        return false;
      }
    }
  }

  return true;
}
} // namespace

bool operator==(matrix<uint16_t> const &Left, matrix<uint16_t> const &Right) {
  return comparsionImpl(Left, Right);
}
bool operator==(matrix<uint8_t> const &Left, matrix<uint8_t> const &Right) {
  return comparsionImpl(Left, Right);
}
auto operator==(matrix<int16_t> const &Left, matrix<int16_t> const &Right)
    -> bool {
  return comparsionImpl(Left, Right);
}
auto operator==(matrix<int8_t> const &Left, matrix<int8_t> const &Right)
    -> bool {
  return comparsionImpl(Left, Right);
}
} // namespace boost::numeric::ublas

void saveRGBToImage(MatrixT<uint8_t> const &R, MatrixT<uint8_t> const &G,
                    MatrixT<uint8_t> const &B, std::string const &Path) {
  std::unique_ptr<TIFF, std::function<void(TIFF *const)>> Image{
      TIFFOpen(Path.c_str(), "w"), [](TIFF *const Image) { TIFFClose(Image); }};

  BOOST_ASSERT_MSG((R.size1() == G.size1() == B.size1()) &&
                       (R.size2() == G.size2() == B.size2()),
                   "Raster tables size is not correct");

  const auto RowsCount = static_cast<uint32>(R.size1());
  const auto ColsCount = static_cast<uint32>(R.size2());
  uint32 const ChannelForPixelCount = 3;

  TIFFSetField(Image.get(), TIFFTAG_IMAGEWIDTH, ColsCount);
  TIFFSetField(Image.get(), TIFFTAG_IMAGELENGTH, RowsCount);
  TIFFSetField(Image.get(), TIFFTAG_BITSPERSAMPLE, 8);
  TIFFSetField(Image.get(), TIFFTAG_SAMPLESPERPIXEL, ChannelForPixelCount);
  TIFFSetField(Image.get(), TIFFTAG_ROWSPERSTRIP, 1);
  TIFFSetField(Image.get(), TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
  TIFFSetField(Image.get(), TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(Image.get(), TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  TIFFSetField(Image.get(), TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
  TIFFSetField(Image.get(), TIFFTAG_COMPRESSION, COMPRESSION_NONE);

  std::vector<uint8> ScanLine;
  ScanLine.resize(ColsCount * ChannelForPixelCount);

  for (std::size_t Row = 0; Row < RowsCount; Row++) {
    auto It = std::begin(ScanLine);
    for (std::size_t Col = 0; Col < ColsCount; ++Col) {
      *It = R(Row, Col);
      *(It + 1) = G(Row, Col);
      *(It + 2) = B(Row, Col);
      It += 3;
    }
    TIFFWriteScanline(Image.get(), &ScanLine[0], static_cast<uint32>(Row), 0);
  }
}

void printVector(std::array<int16_t, 64> const &Vector) {
  std::cout << Vector.size() << ':' << std::endl;
  for (const auto &Element : Vector) {
    std::cout << Element << '\t';
  }
  std::cout << std::endl;
}

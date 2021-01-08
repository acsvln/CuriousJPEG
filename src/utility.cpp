#include "utility.hpp"

#include <array>
#include <boost/assert.hpp>
#include <tiffio.h>

auto highByte(const uint8_t Number) -> uint8_t { return Number >> 4; }

auto lowByte(const uint8_t Number) -> uint8_t { return Number & 0xF; }

template <class Type>
auto CreateZigZagMatrixImpl(std::array<Type, 64> Source)
    -> boost::numeric::ublas::matrix<Type> {
  using SizeType = typename boost::numeric::ublas::matrix<Type>::size_type;
  const SizeType Size = 8;
  boost::numeric::ublas::matrix<Type> Matrix(Size, Size);

  auto StartIterator = Source.begin();
  auto EndIterator = Source.rbegin();

  for (SizeType I = 0; I < Size; I++) {
    SizeType X = I;
    if (I % 2) { // нечетные
      for (SizeType J = 0; (J <= I) && (StartIterator < Source.end()) &&
                           (EndIterator < Source.rend());
           J++) {
        Matrix(J, X) = *StartIterator;
        Matrix(Size - 1 - J, Size - 1 - X) = *EndIterator;
        ++StartIterator;
        ++EndIterator;

        if (0 == X) {
          break;
        } else {
          X--;
        }
      }
    } else { // четные
      for (SizeType J = 0; (J <= I) && (StartIterator < Source.end()) &&
                           (EndIterator < Source.rend());
           J++) {
        Matrix(X, J) = *StartIterator;
        Matrix(Size - 1 - X, Size - 1 - J) = *EndIterator;
        ++StartIterator;
        ++EndIterator;

        if (0 == X) {
          break;
        } else {
          X--;
        }
      }
    }
  }

  return Matrix;
}

auto CreateZigZagMatrix(std::array<int16_t, 64> source)
    -> boost::numeric::ublas::matrix<int16_t> {
  return CreateZigZagMatrixImpl(source);
}

auto CreateZigZagMatrix(std::array<uint16_t, 64> source)
    -> boost::numeric::ublas::matrix<uint16_t> {
  return CreateZigZagMatrixImpl(source);
}

void saveRGBToImage(const boost::numeric::ublas::matrix<uint8_t> &R,
                    const boost::numeric::ublas::matrix<uint8_t> &G,
                    const boost::numeric::ublas::matrix<uint8_t> &B,
                    const std::string &Path) {
  std::unique_ptr<TIFF, std::function<void(TIFF *const)>> Image{
      TIFFOpen(Path.c_str(), "w"), [](TIFF *const Image) { TIFFClose(Image); }};

  BOOST_ASSERT_MSG((R.size1() == G.size1()) && (R.size1() == B.size1()) &&
                       (R.size2() == G.size2()) && (R.size2() == B.size2()),
                   "Raster tables size is not correct");

  const auto RowsCount = static_cast<uint32>(R.size1());
  const auto ColsCount = static_cast<uint32>(R.size2());
  const uint32 ChannelForPixelCount = 3;

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

#include "testing_utility.hpp"

#include <iostream>
#include <vector>
#include <boost/numeric/ublas/io.hpp>
#include <tiffio.h>

namespace boost::numeric::ublas {

namespace {
template <class T>
auto comparsionImpl(matrix<T> const &left, matrix<T> const &right) -> bool {
  using size_type = typename matrix<T>::size_type;

  if (left.size1() != right.size1()) {
    return false;
  }

  if (left.size2() != right.size2()) {
    return false;
  }

  for (size_type i = 0; i < left.size1(); ++i) {
    for (size_type j = 0; j < left.size2(); ++j) {
      if (left(i, j) != right(i, j)) {
        return false;
      }
    }
  }

  return true;
}
} // namespace

bool operator==(matrix<uint16_t> const &left, matrix<uint16_t> const &right) {
  return comparsionImpl(left, right);
}
bool operator==(matrix<uint8_t> const &left, matrix<uint8_t> const &right) {
  return comparsionImpl(left, right);
}
auto operator==(matrix<int16_t> const &left, matrix<int16_t> const &right)
    -> bool {
  return comparsionImpl(left, right);
}
auto operator==(matrix<int8_t> const &left, matrix<int8_t> const &right)
    -> bool {
  return comparsionImpl(left, right);
}
} // namespace boost::numeric::ublas

void saveRGBToImage(
        MatrixT<uint8_t> const& R,
        MatrixT<uint8_t> const& G,
        MatrixT<uint8_t> const& B,
        std::string const& path) {
    std::unique_ptr<TIFF, std::function<void(TIFF* const)>> Image{
          TIFFOpen(path.c_str(), "w")
        , []( TIFF * const image) {
            TIFFClose(image);
        }
    };

    BOOST_ASSERT_MSG(
          ( R.size1() == G.size1() == B.size1() )
        &&( R.size2() == G.size2() == B.size2() ),
        "Raster tables size is not correct"
    );

    const auto RowsCount = static_cast<uint32>( R.size1() );
    const auto ColsCount = static_cast<uint32>( R.size2() );
    uint32 const ChannelForPixelCount = 3;

    TIFFSetField(Image.get(), TIFFTAG_IMAGEWIDTH, ColsCount);
    TIFFSetField(Image.get(), TIFFTAG_IMAGELENGTH, RowsCount);
    TIFFSetField(Image.get(), TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(Image.get(), TIFFTAG_SAMPLESPERPIXEL, ChannelForPixelCount);
    TIFFSetField(Image.get(), TIFFTAG_ROWSPERSTRIP, 1);
    TIFFSetField(Image.get(), TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(Image.get(), TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(Image.get(), TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB); //
    TIFFSetField(Image.get(), TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT );
    TIFFSetField(Image.get(), TIFFTAG_COMPRESSION, COMPRESSION_NONE);

    std::vector<uint8> ScanLine;
    ScanLine.resize( ColsCount * ChannelForPixelCount );

    for ( std::size_t Row = 0; Row < RowsCount; Row++) {
        auto It = std::begin(ScanLine);
        for ( std::size_t Col = 0; Col < ColsCount; ++Col ) {
            *It = R(Row,Col);
            *(It + 1) = G(Row,Col);
            *(It + 2) = B(Row,Col);
            It += 3;
        }
        TIFFWriteScanline(Image.get(), &ScanLine[0], static_cast<uint32>(Row), 0);
    }
}

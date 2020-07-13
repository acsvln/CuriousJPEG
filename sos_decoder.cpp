#include "sos_decoder.hpp"
#include "utility.hpp"

#include <map>
#include <bitset>
#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/assert.hpp>

#include "data_reader.hpp"

auto SOSDecoder::LocateNodeInHuffmanTree(
    BitExtractor &Extractor, std::shared_ptr<HuffmanTree::Node> const &Tree)
    -> std::shared_ptr<HuffmanTree::Node> {
  auto node = Tree;

  do {
    const auto BitNum = Extractor.nextNumber();

    switch (BitNum) {
    case 0:
        node = node->left();
        break;
    case 1:
        node = node->right();
        break;
    default:
        BOOST_ASSERT(false);
    }

    BOOST_ASSERT_MSG(nullptr != node, "Invalid tree structure");

  } while (!node->isLeaf());

  return node;
}

boost::numeric::ublas::matrix<uint8_t> SOSDecoder::ReadMatrix(
    BitExtractor& extractor,
    const std::shared_ptr<HuffmanTree::Node>& DC_Table,
    const std::shared_ptr<HuffmanTree::Node>& AC_Table )
{
    std::array<uint16_t, 64> buffer = {0};

    const auto fixNum = []( uint8_t const num, std::size_t const length ) -> uint8_t {
        std::bitset<8> set = num;
        if ( set[length - 1] == 0 ) {
            return num - std::pow( 2, length ) + 1;
        }
        return num;
    };

    const auto DC_node = LocateNodeInHuffmanTree( extractor, DC_Table );
    const auto DC_value = DC_node->data();
    if ( DC_value == 0 ) {
        buffer[0] = 0;
    } else {
        uint8_t num = extractor.nextNumber( DC_value );
        buffer[0] = fixNum( num, DC_value );
    }

    auto it = buffer.begin() + 1;

    do
    {
        const auto AC_node = LocateNodeInHuffmanTree( extractor, AC_Table );

        if ( nullptr == AC_node ) {
            throw "bad";
        }

        const auto AC_value = AC_node->data();
        if ( 0 == AC_value ) {
            // матрица уже заполнена нулями
            return CreateZigZagMatrix( buffer );
        }

        // добавляем нули
        auto null_cnt = ( AC_value & 0xF0 ) >> 4;
        if ( it + null_cnt == buffer.end() ) {
            throw "WTF";
        }

        it += null_cnt;

        // добавляем значение
        const auto coeff_len = AC_value & 0xF;

        if( coeff_len !=  0 ){
            const auto coeff = extractor.nextNumber( coeff_len );
            *it = fixNum( coeff, coeff_len );
            it++;
        }
    } while( it != buffer.end() );

    return CreateZigZagMatrix( buffer );
}

SOSDecoder::Cs SOSDecoder::ReadMCU(
    BitExtractor& extractor,
    DCTTable const& dct,
    std::vector<Channel>const& channels,
    std::vector<std::shared_ptr<HuffmanTree::Node>> AC_HuffmanTables,
    std::vector<std::shared_ptr<HuffmanTree::Node>> DC_HuffmanTables  )
{
    Cs result;
    
    for ( const auto& channel : channels ) {
        const auto it = std::find_if(
              std::begin( dct.components )
            , std::end( dct.components )
            , [id = channel.id]( const auto& comp ){
            return comp.id == id;
        } );

        const auto dc_id = channel.dc_id;
        const auto ac_id = channel.ac_id;
        const auto AC_Root = AC_HuffmanTables.at(ac_id);
        const auto DC_Root = DC_HuffmanTables.at(dc_id);

        std::vector<boost::numeric::ublas::matrix<uint8_t>> cs;

        if ( it != std::end( dct.components ) ) {
            for ( auto t = 0; t < ( it->h * it->v ); t++ ) {
                const auto matrix = ReadMatrix(
                    extractor,
                    DC_Root,
                    AC_Root
                );
                cs.push_back( matrix );
            }
        }

        if ( cs.size() > 1 ) {
            for (   auto prev_it = cs.begin() + 1, it = cs.begin();
                    it < cs.end() && prev_it < cs.end();
                    ++it, ++prev_it ) {
                auto& prev_matrix = *prev_it;
                const auto& matrix = *it;
                prev_matrix(0,0) = matrix(0,0) + prev_matrix(0,0);
            }
        }

        switch ( channel.id ) {
        case 1:
            result.Cs1 = cs;
            break;
        case 2:
            result.Cs2 = cs;
            break;
        case 3:
            result.Cs3 = cs;
            break;
        case 4:
            assert(false);
            break;
        }
    }
    
    return result;
}

SOSDecoder::Cs SOSDecoder::QuantMCU(
      SOSDecoder::Cs mcu
    , std::vector<DCTComponent> const& components
    , std::vector<boost::numeric::ublas::matrix<uint8_t>> quant )
{
    const auto find_chan = [&]( const auto id ) -> DCTComponent {
        const auto it = std::find_if(
              std::begin( components )
            , std::end( components )
            , [id]( const auto& comp ){
            return comp.id == id;
        } );

        if ( it == std::end( components ) ) {
            assert( false );
        }

        return *it;
    };

    const auto do_smthng = [&]( auto& matrx_lsit, const auto chanid ){
        const auto dqtid = find_chan( chanid ).dqtId;


//        std::cout << "dqtid" << (int) dqtid;

        for ( auto& matrx : matrx_lsit ) {
            const auto dqt = quant.at( dqtid );

            // домножили на матрицу квантования
            matrx = boost::numeric::ublas::element_prod( matrx, dqt );
        }

        return matrx_lsit;
    };

    //-------------------------------------
    mcu.Cs1 = do_smthng( mcu.Cs1, 1 );
    mcu.Cs2 = do_smthng( mcu.Cs2, 2 );
    mcu.Cs3 = do_smthng( mcu.Cs3, 3 );

    return mcu;
}


boost::numeric::ublas::matrix<int8_t> SOSDecoder::ReverseDQT3(
    boost::numeric::ublas::matrix<int8_t> const& in )
{
    int i, j, u, v;
     double s;

     boost::numeric::ublas::matrix<int8_t> out(8,8);

     for (i = 0; i < 8; i++)
       for (j = 0; j < 8; j++)
       {
         s = 0;

         for (u = 0; u < 8; u++)
           for (v = 0; v < 8; v++)
             s += in(v,u) * cos((2 * i + 1) * u * M_PI / 16) *
                             cos((2 * j + 1) * v * M_PI / 16) *
                  ((u == 0) ? 1 / sqrt(2) : 1.) *
                  ((v == 0) ? 1 / sqrt(2) : 1.);

         out(i,j) = s / 4;
       }

     for (int i = 0; i< out.size1(); i++ ){
         for (int j = 0; j< out.size1(); j++ ){
             out( i, j ) = std::min(std::max(0, out( i, j )  + 128), 255);
         }
     }

     return out;
}

//-------------------------------------

//ivect4 ycrcb2rgb(int y, int cr, int cb)
//{
//    int r = round(1.402*(cr-128) + y);
//    int g = round(-0.34414*(cb-128)-0.71414*(cr-128) + y);
//    int b = round(1.772*(cb-128) + y);
//    return ivect4(r, g, b, 255);
//}

std::tuple<
      boost::numeric::ublas::matrix<int16_t>
    , boost::numeric::ublas::matrix<int16_t>
    , boost::numeric::ublas::matrix<int16_t>
>
SOSDecoder::YCbCrToRGB(
      boost::numeric::ublas::matrix<int16_t> const& y
    , boost::numeric::ublas::matrix<int16_t> const& cb
    , boost::numeric::ublas::matrix<int16_t> const& cr )
{
    boost::numeric::ublas::matrix<int16_t> R(8,8);
    boost::numeric::ublas::matrix<int16_t> G(8,8);
    boost::numeric::ublas::matrix<int16_t> B(8,8);

    // https://impulseadventure.com/photo/jpeg-color-space.html

//    R = round(Y                      + 1.402   * (Cr-128))
//       G = round(Y - 0.34414 * (Cb-128) - 0.71414 * (Cr-128))
//       B = round(Y + 1.772   * (Cb-128)                     )

    for ( unsigned i = 0; i < 8; i++ ) {
        for ( unsigned j = 0; j < 8; j++ ) {
            R(i,j) = (int16_t) std::round( y(i,j)                               + 1.402   * ( cr(i/2,j/2) - 128 ) );
            G(i,j) = (int16_t) std::round( y(i,j) - 0.34414 * ( cb(i/2,j/2 ) - 128 ) - 0.71414 * ( cr(i/2,j/2) - 128 ) );
            B(i,j) = (int16_t) std::round( y(i,j) + 1.772   * ( cb(i/2,j/2) - 128 ) );

            const auto fix = [=]( auto& C ) {
                if ( C(i,j) < 0 ){
                    C(i,j) = 0;
                } else if ( C(i,j) > 255 ) {
                    C(i,j) = 255;
                }
            };

            fix( R );
            fix( G );
            fix( B );


            //       R = min(max(0, R), 255)
            //       G = min(max(0, G), 255)
            //       B = min(max(0, B), 255)

//            if ( R(i,j) < 0 ){
//                R(i,j) = 0;
//            } else if ( R(i,j) > 255 ) {
//                R(i,j) = 255;
//            }

//            if ( G(i,j) < 0 ){
//                G(i,j) = 0;
//            } else if ( G(i,j) > 255 ) {
//                G(i,j) = 255;
//            }

//            if ( B(i,j) < 0 ){
//                B(i,j) = 0;
//            } else if ( B(i,j) > 255 ) {
//                B(i,j) = 255;
//            }
        }
    }

    return {R, G, B};
}

//-------------------------------------
boost::numeric::ublas::matrix<int16_t> SOSDecoder::ReverseDQT_1(
        boost::numeric::ublas::matrix<int16_t> const& matrix ) {
    const auto Cx = []( const auto x ) -> double {
        return ( 0 == x ) ? (1. / std::sqrt( 2. )) : 1.;
    };

    boost::numeric::ublas::matrix<int16_t> res(8,8);

    for ( int y = 0; y < 8; y++ ) {
        for ( int x = 0; x < 8; x++ ) {
            //-------------------------------------
            double tmp1 = 0.;
            for ( int u = 0; u < 8; u++ ) {
                double tmp2 = 0.;
                for ( int v = 0; v < 8; v++ ) {
                    const double Cu = Cx(u);
                    const double Cv = Cx(v);
                    const double Svu = matrix(v,u);
                    tmp2 += Cu * Cv * Svu
                        * std::cos( ( 2*x + 1 ) * u * M_PI / 16. )
                        * std::cos( ( 2*y + 1 ) * v * M_PI / 16. );
                }
                tmp1 += tmp2;
            }
            //-------------------------------------
            const auto w = (1. / 4.)  *  tmp1; //
            res(y,x) = w;
//            std::cout << "res" << w << std::endl;
        }
    }

    return res;
}


void SOSDecoder::Invoke(std::istream &aStream, Context& aContext) {
    const auto size = DataReader::readNumber<uint16_t>(aStream);
    printSectionDescription("Start Of Scan!", size);
    const auto pos = aStream.tellg() + std::streampos{size};
    const auto channel_count = DataReader::readNumber<uint8_t>(aStream);
    if ( channel_count != 3 ) {
        assert(false);
    }

    std::vector<Channel> channels;

    for ( std::size_t i = 0; i < channel_count; ++i ) {
        const auto id = DataReader::readNumber<uint8_t>(aStream);
        const auto huff_id = DataReader::readNumber<uint8_t>(aStream);

        Channel channel;
        channel.id = id;
        channel.ac_id = huff_id >> 4;
        channel.dc_id = huff_id & 0xF;

        channels.push_back( channel );
    }

    DataReader::skipChars(aStream,3);


    using Matrix = boost::numeric::ublas::matrix<uint16_t>;
    using ImageData = std::map<int, std::vector<Matrix>>;

    std::vector<ImageData> imageData;
    BitExtractor extractor{ aStream };

    do {
        const auto mcu = ReadMCU(
            extractor,
            aContext.dct,
            channels,
            aContext.AC_HuffmanTables,
            aContext.DC_HuffmanTables
        );

        //-------------------------------------
        const auto do_smthng = [&]( const auto& matrx_lsit, const auto dqtid ){
            for ( auto& matrx : matrx_lsit ) {
                const auto dqt = aContext.DQT_Vector.at( dqtid );

                // домножили на матрицу квантования
                const auto matrx_wtf = boost::numeric::ublas::element_prod( matrx, dqt );

            }
        };

        //-------------------------------------
        const auto find_chan = []( const auto& components, const auto id ) -> DCTComponent {
            const auto it = std::find_if(
                  std::begin( components )
                , std::end( components )
                , [id]( const auto& comp ){
                return comp.id == id;
            } );

            if ( it == std::end( components ) ) {
                assert( false );
            }

            return *it;
        };

        //-------------------------------------
        do_smthng( mcu.Cs1, find_chan( aContext.dct.components, 0 ).dqtId );
        do_smthng( mcu.Cs2, find_chan( aContext.dct.components, 1 ).dqtId );
        do_smthng( mcu.Cs3, find_chan( aContext.dct.components, 2 ).dqtId );

//        for ( const auto& data : imageData ) {
//            const auto rgb = YCbCrToRGB(data[0], data[1], data[2]);
//            aContext.Image.push_back(rgb);
//        }

    } while (false);


}

//struct Channel {
//    std::size_t id;
//    std::size_t dc_id;
//    std::size_t ac_id;
//};

//const auto read_table = [&]( auto dc_id, auto ac_id ){
//    while (true) {
//        const auto i = extractor.nextNumber();

//    }
//    return boost::numeric::ublas::matrix<uint16_t>{};
//};

//do {
//
//    for ( const auto& channel : channels ) {

//        // dct_component =
//        const auto it = std::find_if(
//              std::begin( aContext.dct.components )
//            , std::end( aContext.dct.components )
//            , [id = channel.id]( const auto& comp ){
//            return comp.id == id;
//        } );

//        if ( it != std::end( aContext.dct.components ) ) {
//            for ( auto t = 0; t < (it->h * it->v); t++ ) {
//                auto matrix = read_table( channel.dc_id, channel.ac_id );
//                // matix *= dqt
//                // DC += 1024
//                data[channel.id].push_back(matrix);
//            }
//        }
//    }
//    imageData.push_back(data);
//} while (pos != aStream.tellg());

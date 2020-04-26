#include "sos_decoder.hpp"
#include "utility.hpp"

#include <map>
#include <bitset>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <iostream>

SOSDecoder::BitExtractor::BitExtractor( std::istream &aStream )
    : mStream{ aStream }
{}

uint8_t SOSDecoder::BitExtractor::nextNumber( std::size_t bit_cnt ) {
    if ( 0 == bit_cnt || bit_cnt > 8) {
        // TODO: ASSERT
        throw "wtf";
    }

    std::bitset<8> result;

    for ( int i = bit_cnt - 1; i >= 0; --i ) {
        if ( 0 == mCounter ) {
            mbits = static_cast<unsigned>( mStream.get() );
            mCounter = mbits.size();
        }
        mCounter--;
        result[i] = mbits[mCounter];
    }

    return result.to_ulong();
}


//-------------------------------------
std::shared_ptr<DHTNode> SOSDecoder::LocateNodeInTree(
          BitExtractor& extractor
        , const std::shared_ptr<DHTNode>& tree ){
    std::shared_ptr<DHTNode> node = tree;
    do {
        const auto next_number = extractor.nextNumber();
        if ( 0 == next_number ) {
            node = node->left;
        } else if (  1 == next_number ) {
            node = node->right;
        } else {
            assert( false );
        }
        if ( nullptr == node ) {
            assert( false );
        }
    } while( false == node->IsLeaf() );
    return node;
}

boost::numeric::ublas::matrix<uint8_t> SOSDecoder::ReadMatrix(
    BitExtractor& extractor,
    const std::shared_ptr<DHTNode>& DC_Table,
    const std::shared_ptr<DHTNode>& AC_Table )
{
    std::array<uint16_t, 64> buffer = {0};

    const auto fixNum = []( uint8_t const num, std::size_t const length ) -> uint8_t {
        std::bitset<8> set = num;
        if ( set[length - 1] == 0 ) {
            return num - std::pow( 2, length ) + 1;
        }
        return num;
    };

    const auto DC_node = LocateNodeInTree( extractor, DC_Table );
    const auto DC_value = DC_node->data.value();
    if ( DC_value == 0 ) {
        buffer[0] = 0;
    } else {
        uint8_t num = extractor.nextNumber( DC_value );
        buffer[0] = fixNum( num, DC_value );
    }

    auto it = buffer.begin() + 1;

    do
    {
        const auto AC_node = LocateNodeInTree( extractor, AC_Table );

        if ( nullptr == AC_node ) {
            throw "bad";
        }

        const auto AC_value = AC_node->data.value();
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
    std::vector<std::shared_ptr<DHTNode>> AC_HuffmanTables,
    std::vector<std::shared_ptr<DHTNode>> DC_HuffmanTables  )
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


        std::cout << "dqtid" << (int) dqtid;

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

void SOSDecoder::Invoke(std::istream &aStream, Context& aContext) {
    const auto size = ReadNumberFromStream<uint16_t>(aStream);
    printSectionDescription("Start Of Scan!", size);
    const auto pos = aStream.tellg() + std::streampos{size};
    const auto channel_count = ReadNumberFromStream<uint8_t>(aStream);
    if ( channel_count != 3 ) {
        assert(false);
    }

    std::vector<Channel> channels;

    for ( std::size_t i = 0; i < channel_count; ++i ) {
        const auto id = ReadNumberFromStream<uint8_t>(aStream);
        const auto huff_id = ReadNumberFromStream<uint8_t>(aStream);

        Channel channel;
        channel.id = id;
        channel.ac_id = huff_id >> 4;
        channel.dc_id = huff_id & 0xF;

        channels.push_back( channel );
    }

    aStream.ignore( 3 );

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





//

//



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



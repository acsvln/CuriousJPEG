#include "sos_decoder.hpp"
#include "utility.hpp"

#include <map>
#include <bitset>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>


SOSDecoder::BitExtractor::BitExtractor( std::istream &aStream )
    : mStream{ aStream }
{}

uint8_t SOSDecoder::BitExtractor::nextNumber() {
    if ( 0 == mCounter ) {
        mbits = static_cast<unsigned>( mStream.get() );
        mCounter = mbits.size();
    }
    mCounter--;
    return mbits.test( mCounter ) ? 1 : 0;
}

uint8_t SOSDecoder::BitExtractor::readBits(std::size_t cnt)
{
    std::bitset<8> buffer;
    for ( std::size_t i = 0; i < cnt; i++ ) {
        buffer[i] = nextNumber();
    }
    return buffer.to_ulong();
}

boost::numeric::ublas::matrix<uint8_t> SOSDecoder::ReadMatrix(
    BitExtractor& extractor,
    const std::shared_ptr<DHTNode>& AC_Table,
    const std::shared_ptr<DHTNode>& DC_Table )
{
    const auto locate_node = [&]( const auto& table ){
        std::shared_ptr<DHTNode> node{ nullptr };
        do {
            const auto next_number = extractor.nextNumber();
            if ( 0 == next_number ) {
                node = table->left;
            } else if (  1 == next_number ) {
                node = table->right;
            } else {
                assert( false );
            }
        } while( false == node->IsLeaf() );
        return node;
    };

    std::array<uint16_t, 64> buffer = {0};

    boost::numeric::ublas::matrix<uint8_t> result{ 8, 8, 0u };

    const auto DC_node = locate_node( DC_Table );
    const auto DC_value = DC_node->data.value();
    if ( DC_value == 0 ) {
        buffer[0] = 0;
    } else {
        buffer[0] = extractor.readBits( DC_value );
        // tODO: пофиксить цифру
    }

    const auto AC_node = locate_node( AC_Table );
    const auto AC_value = AC_node->data.value();
    if ( 0 == AC_value ) {
        // матрица уже заполнена нулями
        return CreateZigZagMatrix( buffer );
    }

    auto it = buffer.begin();
    const auto end_it = buffer.end();
    do
    {
        auto null_cnt = ( AC_value & 0xF0 ) >> 4;
        const auto coeff_len = AC_value & 0xF;
        const auto coeff =   (coeff_len != 0)
                            ? extractor.readBits( coeff_len )
                            : 0;

        while ( null_cnt > 0 ) {
            // проверка
            *it;
            --null_cnt;
            it++;
        }
        if(coeff_len !=  0){
            *it = coeff;
            it++;
        }
    } while( it != end_it );
}

void SOSDecoder::Invoke(std::istream &aStream, Context& aContext) {
    const auto size = ReadNumberFromStream<uint16_t>(aStream);
    printSectionDescription("Start Of Scan!", size);
    const auto pos = aStream.tellg() + std::streampos{size};
    const auto channel_count = ReadNumberFromStream<uint8_t>(aStream);
    if ( channel_count != 3 ) {
        // TODO: exception
        return;
    }

    struct Channel {
        std::size_t id;
        std::size_t dc_id;
        std::size_t ac_id;
    };

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

    BitExtractor extractor{ aStream };

    using Matrix = boost::numeric::ublas::matrix<uint16_t>;
    using ImageData = std::map<int, std::vector<Matrix>>;

    const auto read_table = [&]( auto dc_id, auto ac_id ){
        while (true) {
            const auto i = extractor.nextNumber();

        }
        return boost::numeric::ublas::matrix<uint16_t>{};
    };

    std::vector<ImageData> imageData;
    do {
        ImageData data;
        for ( const auto& channel : channels ) {

            // dct_component =
            const auto it = std::find_if(
                  std::begin( aContext.dct.components )
                , std::end( aContext.dct.components )
                , [id = channel.id]( const auto& comp ){
                return comp.id == id;
            } );

            if ( it != std::end( aContext.dct.components ) ) {
                for ( auto t = 0; t < (it->h * it->v); t++ ) {
                    auto matrix = read_table( channel.dc_id, channel.ac_id );
                    // matix *= dqt
                    // DC += 1024
                    data[channel.id].push_back(matrix);
                }
            }
        }
        imageData.push_back(data);
    } while (pos != aStream.tellg());


//    for ( const auto& data : imageData ) {
//        const auto rgb = YCbCrToRGB(data[0], data[1], data[2]);
//        aContext.Image.push_back(rgb);
//    }
}



#if 0

//        uint8_t nextNumber();

//-------------------------------------
// uint8_t mNumber;
//        static_assert((sizeof(std::istream::char_type) == sizeof(uint8_t)));


uint8_t SOSDecoder::BitExtractor::nextNumber() {
    if ( 0 == mCounter ) {
        mNumber = static_cast<uint8_t>(mStream.get());
        mCounter = 8;
    }

    std::cout << std::bitset<8>(mNumber) << std::endl;

    uint8_t const result = (0x80 & mNumber) >> 7;

    mNumber <<= 1;
    mCounter--;
    return result;
}

#endif

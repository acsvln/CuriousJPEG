import qbs

CppApplication {
    name: "src"

    cpp.cxxLanguageVersion: "c++17"

    consoleApplication: true
    files: [
        "bit_extractor.hpp",
        "data_reader.hpp",
        "decoder_base.hpp",
        "decoder_context.hpp",
        "decoder_processor.hpp",
        "dht_decoder.hpp",
        "exceptions.hpp",
        "huffman_tree.hpp",
        "idecoder.hpp",
        "sof0_decoder.hpp",
        "sos_decoder.hpp",
        "utility.hpp",
        "dqt_decoder.hpp",
        "bit_extractor.cpp",
        "data_reader.cpp",
        "decoder_base.cpp",
        "exceptions.cpp",
        "idecoder.cpp",
        "huffman_tree.cpp",
        "dht_decoder.cpp",
        "dqt_decoder.cpp",
        "sof0_decoder.cpp",
        "sos_decoder.cpp",
        "decoder_processor.cpp",
        "main.cpp",
        "utility.cpp"
    ]

    Depends {
        name: "ConanBasicSetup"
        required: true
    }
}

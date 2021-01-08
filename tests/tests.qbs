import qbs

CppApplication {
    name: "tests"
    type: ["application", "autotest"]

    cpp.cxxLanguageVersion: "c++17"

    cpp.includePaths: [
        "../src",
    ]

    consoleApplication: true

    files: [
        "../src/bit_extractor.cpp",
        "../src/decoder_base.cpp",
        "../src/dqt_decoder.cpp",
        "../src/huffman_tree.cpp",
        "../src/dht_decoder.cpp",
        "../src/sof0_decoder.cpp",
        "../src/sos_decoder.cpp",
        "../src/utility.cpp",
        "../src/idecoder.cpp",
        "../src/exceptions.cpp",
        "../src/data_reader.cpp",
        "testing_utility.hpp",
        "bit_extractor_t.cpp",
        "dqt_decoder_t.cpp",
        "huffman_tree_t.cpp",
        "main_t.cpp",
	    "dht_decoder_t.cpp",
        "sof0_decoder_t.cpp",
        "sos_decoder_t.cpp",
        "utility_t.cpp",
        "testing_shared_data.cpp",
        "testing_utility.cpp"
    ]

    Depends {
      name: "ConanBasicSetup"
      required: true
    }
}

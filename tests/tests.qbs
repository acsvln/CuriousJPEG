import qbs

CppApplication {
    name: "tests"
    type: ["application", "autotest"]

    cpp.cxxLanguageVersion: "c++17"

    consoleApplication: true

    cpp.includePaths: [
        "../src",
    ]

    Group {
        name: "src_files"
        files: [
            "bit_extractor.cpp",
            "decoder_base.cpp",
            "dqt_decoder.cpp",
            "huffman_tree.cpp",
            "dht_decoder.cpp",
            "sof0_decoder.cpp",
            "sos_decoder.cpp",
            "utility.cpp",
            "idecoder.cpp",
            "exceptions.cpp",
            "data_reader.cpp"
        ]
        prefix: "../src/"
    }

    files: [
        "testing_utility.hpp",
        "testing_shared_data.hpp",
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
    }

    Depends {
      name: "src"
    }
}

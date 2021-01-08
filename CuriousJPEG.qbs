Project {
    name: "CuriousJPEG"
    minimumQbsVersion: "1.16"
    references: [
         "src/src.qbs",
         "tests/tests.qbs",
         "conanbuildinfo.qbs"
    ]
    qbsSearchPaths: "qbs"

    AutotestRunner {}
}


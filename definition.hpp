#ifndef DEFINITION_HPP
#define DEFINITION_HPP

#include <cstdint>

// Start of Frame markers; non-differential; Huffman coding
const uint16_t kJpgTagHuffBaselineDCT = 0xFFC0;
const uint16_t kJpgTagHuffExtSequentialDCT = 0xFFC1;
const uint16_t kJpgTagHuffProgressiveDCT = 0xFFC2;
const uint16_t kJpgTagHuffLosslessSeq = 0xFFC3;

// Start of Frame markers; differential; Huffman coding
const uint16_t kJpgTagHuffDiffSequentialDCT = 0xFFC5;
const uint16_t kJpgTagHuffDiffProgressiveDCT = 0xFFC6;
const uint16_t kJpgTagHuffDiffLosslessSeq = 0xFFC7;

// Start of Frame markers; non-differential; arithmetic coding
const uint16_t kJpgTagArthBaselineDCT = 0xFFC8;
const uint16_t kJpgTagArthExtSequentialDCT = 0xFFC9;
const uint16_t kJpgTagArthProgressiveDCT = 0xFFCA;
const uint16_t kJpgTagArthLosslessSeq = 0xFFCB;

// Start of Frame markers; differential; arithmetic coding
const uint16_t kJpgTagArthDiffSequentialDCT = 0xFFCD;
const uint16_t kJpgTagArthDiffProgressiveDCT = 0xFFCE;
const uint16_t kJpgTagArthDiffLosslessSeq = 0xFFCF;

// Huffman table spec
const uint16_t kJpgTagHuffmanTableDef = 0xFFC4;

// Arithmetic table spec
const uint16_t kJpgTagArithmeticTableDef = 0xFFCC;

// Restart Interval termination
const uint16_t kJpgTagRestartIntervalStart = 0xFFD0;
const uint16_t kJpgTagRestartIntervalEnd = 0xFFD7;

// Other markers
const uint16_t kJpgTagStartOfImage = 0xFFD8;
const uint16_t kJpgTagEndOfImage = 0xFFD9;
const uint16_t kJpgTagStartOfScan = 0xFFDA;
const uint16_t kJpgTagQuantTableDef = 0xFFDB;
const uint16_t kJpgTagNumberOfLinesDef = 0xFFDC;
const uint16_t kJpgTagRestartIntervalDef = 0xFFDD;
const uint16_t kJpgTagHierarchProgressionDef = 0xFFDE;
const uint16_t kJpgTagExpandRefComponents = 0xFFDF;

// App segments
const uint16_t kJpgTagApp0 = 0xFFE0;
const uint16_t kJpgTagApp1 = 0xFFE1;
const uint16_t kJpgTagApp2 = 0xFFE2;
const uint16_t kJpgTagApp3 = 0xFFE3;
const uint16_t kJpgTagApp4 = 0xFFE4;
const uint16_t kJpgTagApp5 = 0xFFE5;
const uint16_t kJpgTagApp6 = 0xFFE6;
const uint16_t kJpgTagApp7 = 0xFFE7;
const uint16_t kJpgTagApp8 = 0xFFE8;
const uint16_t kJpgTagApp9 = 0xFFE9;
const uint16_t kJpgTagApp10 = 0xFFEA;
const uint16_t kJpgTagApp11 = 0xFFEB;
const uint16_t kJpgTagApp12 = 0xFFEC;
const uint16_t kJpgTagApp13 = 0xFFED;
const uint16_t kJpgTagApp14 = 0xFFEE;
const uint16_t kJpgTagApp15 = 0xFFEF;

// Jpeg Extensions
const uint16_t kJpgTagJpegExt0 = 0xFFF0;
const uint16_t kJpgTagJpegExt1 = 0xFFF1;
const uint16_t kJpgTagJpegExt2 = 0xFFF2;
const uint16_t kJpgTagJpegExt3 = 0xFFF3;
const uint16_t kJpgTagJpegExt4 = 0xFFF4;
const uint16_t kJpgTagJpegExt5 = 0xFFF5;
const uint16_t kJpgTagJpegExt6 = 0xFFF6;
const uint16_t kJpgTagJpegExt7 = 0xFFF7;
const uint16_t kJpgTagJpegExt8 = 0xFFF8;
const uint16_t kJpgTagJpegExt9 = 0xFFF9;
const uint16_t kJpgTagJpegExtA = 0xFFFA;
const uint16_t kJpgTagJpegExtB = 0xFFFB;
const uint16_t kJpgTagJpegExtC = 0xFFFC;
const uint16_t kJpgTagJpegExtD = 0xFFFD;

// Comments
const uint16_t kJpgTagComment = 0xFFFE;

// Reserved
const uint16_t kJpgTagArithTemp = 0xFF01;
const uint16_t kJpgTagReservedStart = 0xFF02;
const uint16_t kJpgTagReservedEnd = 0xFFBF;

#endif // DEFINITION_HPP

#include <QtCore/QCoreApplication>
#include "cstdlib"
#include <iostream>
#include <fstream>
#include <clocale>
#include <vector>
#include <math.h>

using namespace std;

template <typename T>
        T swap_endian(T u)
{
    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}



#define SwapEndianWord(wM) (wM=swap_endian<word>(wM))

typedef unsigned short int word;
typedef char byte;

struct CUnit
{
    byte m_bId:8;
    byte m_H:4;
    byte m_V:4;
    byte m_bDQTId:8;

};


#pragma pack(push,1)

struct CComponent
{
    unsigned m_bId:8;
    unsigned m_DC:4;
    unsigned m_AC:4;
    //unsigned lol:12;
};

#pragma pack(pop)

enum JpegMarkers
{
    // Start of Frame markers, non-differential, Huffman coding
    HuffBaselineDCT = 0xFFC0,
    HuffExtSequentialDCT = 0xFFC1,
    HuffProgressiveDCT = 0xFFC2,
    HuffLosslessSeq = 0xFFC3,

    // Start of Frame markers, differential, Huffman coding
    HuffDiffSequentialDCT = 0xFFC5,
    HuffDiffProgressiveDCT = 0xFFC6,
    HuffDiffLosslessSeq = 0xFFC7,

    // Start of Frame markers, non-differential, arithmetic coding
    ArthBaselineDCT = 0xFFC8,
    ArthExtSequentialDCT = 0xFFC9,
    ArthProgressiveDCT = 0xFFCA,
    ArthLosslessSeq = 0xFFCB,

    // Start of Frame markers, differential, arithmetic coding
    ArthDiffSequentialDCT = 0xFFCD,
    ArthDiffProgressiveDCT = 0xFFCE,
    ArthDiffLosslessSeq = 0xFFCF,

    // Huffman table spec
    HuffmanTableDef = 0xFFC4,

    // Arithmetic table spec
    ArithmeticTableDef = 0xFFCC,

    // Restart Interval termination
    RestartIntervalStart = 0xFFD0,
    RestartIntervalEnd = 0xFFD7,

    // Other markers
    StartOfImage = 0xFFD8,
    EndOfImage = 0xFFD9,
    StartOfScan = 0xFFDA,
    QuantTableDef = 0xFFDB,
    NumberOfLinesDef = 0xFFDC,
    RestartIntervalDef = 0xFFDD,
    HierarchProgressionDef = 0xFFDE,
    ExpandRefComponents = 0xFFDF,

    // App segments
    App0 = 0xFFE0,
    App1 = 0xFFE1,
    App2 = 0xFFE2,
    App3 = 0xFFE3,
    App4 = 0xFFE4,
    App5 = 0xFFE5,
    App6 = 0xFFE6,
    App7 = 0xFFE7,
    App8 = 0xFFE8,
    App9 = 0xFFE9,
    App10 = 0xFFEA,
    App11 = 0xFFEB,
    App12 = 0xFFEC,
    App13 = 0xFFED,
    App14 = 0xFFEE,
    App15 = 0xFFEF,

    // Jpeg Extensions
    JpegExt0 = 0xFFF0,
    JpegExt1 = 0xFFF1,
    JpegExt2 = 0xFFF2,
    JpegExt3 = 0xFFF3,
    JpegExt4 = 0xFFF4,
    JpegExt5 = 0xFFF5,
    JpegExt6 = 0xFFF6,
    JpegExt7 = 0xFFF7,
    JpegExt8 = 0xFFF8,
    JpegExt9 = 0xFFF9,
    JpegExtA = 0xFFFA,
    JpegExtB = 0xFFFB,
    JpegExtC = 0xFFFC,
    JpegExtD = 0xFFFD,

    // Comments
    Comment = 0xFFFE,

    // Reserved
    ArithTemp = 0xFF01,
    ReservedStart = 0xFF02,
    ReservedEnd = 0xFFBF
              };

enum JpegHuffTableClass {HuffClassDC,HuffClassAC};

void zig_zag()
{
    int siz = 5,k=0;
    cout<<"Please enter the size of the matrix"<<endl;
    cin>>siz;
    int **mat = new int*[siz];
    for(int i = 0; i<siz; i++)
    {
        mat[i] = new int[siz];
        for(int j=0; j<siz; j++)
        {
            mat[i][j] = ++k;
            cout<<mat[i][j]<<" ";
        }
        cout << endl;
    }

    int x,y ,z;
    for(z = 0 ; z <siz ; z++)
    {
        x = 0;
        y = z;
        while(x<=z && y>=0)
        {
            if(z%2)
                cout<<mat[y--][x++]<<" ";
            else
                cout<<mat[x++][y--]<<" ";
        }
        cout<<"\n";
    }
    for(z = 1 ; z<siz ; z++)
    {
        x = z;
        y = siz -1;
        while(x<siz && y>=z)
        {
            if((siz+z)%2)
                cout<<mat[x++][y--]<<" ";
            else
                cout<<mat[y--][x++]<<" ";
        }
        cout<<"\n";
    }
    for(int i=0;i<siz;i++)
        delete[] mat[i];
    delete[] mat;
}

struct CHuffNode
{
    int m_iData;
    CHuffNode * m_phnLeft;
    CHuffNode * m_phnRight;
    CHuffNode * m_phnUp;
    bool m_bIsLeaf;
    signed int m_iLayer;
};

typedef CHuffNode * CHuffNodePointer;


struct CHuffTree
{
    CHuffNode * m_phnRoot;
    int m_iCount;
};

struct CHuffTable
{
    JpegHuffTableClass m_nTableClass;
    byte m_nTableIndex;
    CHuffTree m_htTree;
    CHuffTable * next;
};

typedef CHuffTable * CHuffTablePointer;

struct CDCTTable
{
    byte m_bPrecision;
    byte m_bUnitsCount;
    int m_iHeight;
    int m_iWidth;
    int m_iHmax;
    int m_iVmax;
    CUnit * m_puComponents;
};

CHuffNode * SetNode(CHuffNode * phnParent,int iVal, int iLayer)
{
    CHuffNode * phn=new CHuffNode;
    phn->m_iData=iVal;
    phn->m_bIsLeaf=true;
    phn->m_phnLeft=NULL;
    phn->m_phnRight=NULL;
    phn->m_phnUp=NULL;
    phn->m_iLayer=iLayer;
    if (phnParent->m_phnLeft==NULL)
    {
        phnParent->m_phnLeft=phn;
    }
    else if(phnParent->m_phnRight==NULL)
    {
        phnParent->m_phnRight=phn;
    }
    else return NULL;
    return phn;
}

CHuffNode * GoUpNode(CHuffNode ** phn)
{
    if ((*phn)->m_phnUp!=NULL)
        return (*phn)=(*phn)->m_phnUp;
    else return NULL;
}

//void AddNode(CHuffTree * htTree,CHuffNode * phn,int iVal, int iLayer)
//{
//
//    CHuffNode * phnNewNode=SetNode(phn,iVal);
//    if (phnNewNode!=NULL)
//    {
//
//    }
//    else
//    {
//        if (GoUpNode(&phn)==NULL)
//        {
//            CHuffNode * phnTemp=new CHuffNode;
//            phn->m_bIsLeaf=false;
//            phn->m_phnLeft=phn;
//            phn->m_phnRight=NULL;
//            phn->m_phnUp=NULL;
//            SetNode(phnTemp,iVal);
//        }
//    }
//}

CHuffNode * FindRightUpFreeBranch(CHuffNode * phn)
{
    if (phn->m_phnRight==NULL || phn->m_phnUp==NULL)
    {
        return phn;
    }
    else return FindRightUpFreeBranch(phn->m_phnUp);
}

//CHuffNode * WTF_WTF(CHuffNode * phn,int iLayer,bool bIsUp=true)
//{
//    if (bIsUp)
//    {
//        if (phn->m_phnRight==NULL)
//        {
//            return WTF_WTF(phn,);
//        }
//        else return WTF_WTF(phn,,false);
//    }
////    if (iLayer==phn->m_iLayer)
////    {
////        if (phn->m_phnLeft!=NULL)
////        {
////
////        }
////        else if (phn->m_phnRight!=NULL)
////        {
////
////        }
////        else
////        {
////
////        }
////    }
//    return phn;
//}

//        if (phn->m_phnLeft!=NULL)
//        {
//
//        }
//        else if (phn->m_phnRight!=NULL)
//        {
//
//        }
//        else
//        {
//
//        }
//



//CHuffNode * AddNode(CHuffTree * htTree,CHuffNode * phn,int iVal, int iLayer)
//{
//    if (iLayer==phn->m_iLayer)
//    {
//        if (SetNode(phn,iVal,iLayer)!=NULL)
//        {
//            htTree->m_iCount++;
//            return phn;
//        }
//    }
//    CHuffNode * phnTemp;
//    if (htTree->m_iCount!=1)
//    {
//
//        phn=FindRightUpFreeBranch(phn);
//        if (phn->m_phnUp==NULL)
//        {
//            phnTemp=new CHuffNode;
//            phnTemp->m_bIsLeaf=false;
//            phnTemp->m_phnLeft=phn;
//            phnTemp->m_phnRight=NULL;
//            phnTemp->m_phnUp=NULL;
//            phnTemp->m_iLayer=phn->m_iLayer+1;
//            phn=phnTemp;
//            htTree->m_phnRoot=phn;
//            htTree->m_iCount++;
//        }
//    }
//        if (phn->m_phnRight==NULL)
//        {
//            do
//            {
//                phnTemp=new CHuffNode;
//                phnTemp->m_bIsLeaf=false;
//                phnTemp->m_phnLeft=NULL;
//                phnTemp->m_phnRight=NULL;
//                phnTemp->m_phnUp=phn;
//                phnTemp->m_iLayer=phn->m_iLayer+1;
//                phn->m_phnRight=phnTemp;
//                phn=phnTemp;
//                htTree->m_iCount++;
//            } while (iLayer!=phn->m_iLayer);
//            if (SetNode(phn,iVal,iLayer)!=NULL)
//            {
//                htTree->m_iCount++;
//                return phn;
//            }
//            else cout << "WHY???";
//        }
//        else cout << "Motherf";
//}






//CHuffNode * AddNode(CHuffTree * htTree,CHuffNode * phn,int iVal, int iLayer)
//{
//    if (iLayer==phn->m_iLayer)
//    {
//        if (SetNode(phn,iVal,iLayer)!=NULL)
//        {
//            htTree->m_iCount++;
//            return phn;
//        }
//    }
//    CHuffNode * phnTemp;
//    if (htTree->m_iCount!=1)
//    {
//        phn=FindRightUpFreeBranch(phn);
//        if (phn->m_phnRight==NULL)
//        {
//            do
//            {
//                phnTemp=new CHuffNode;
//                phnTemp->m_bIsLeaf=false;
//                phnTemp->m_phnLeft=NULL;
//                phnTemp->m_phnRight=NULL;
//                phnTemp->m_phnUp=phn;
//                phnTemp->m_iLayer=phn->m_iLayer+1;
//                phn->m_phnRight=phnTemp;
//                phn=phnTemp;
//                htTree->m_iCount++;
//            } while (iLayer!=phn->m_iLayer);
//            if (SetNode(phn,iVal,iLayer)!=NULL)
//            {
//                htTree->m_iCount++;
//                return phn;
//            }
//            else cout << "WHY???";
//        }
//        else cout << "Motherf";
//        if (phn->m_phnUp==NULL)
//        {
//            phnTemp=new CHuffNode;
//            phnTemp->m_bIsLeaf=false;
//            phnTemp->m_phnLeft=phn;
//            phnTemp->m_phnRight=NULL;
//            phnTemp->m_phnUp=NULL;
//            phnTemp->m_iLayer=phn->m_iLayer+1;
//            phn=phnTemp;
//            htTree->m_phnRoot=phn;
//            htTree->m_iCount++;
//        }
//    }
//}

CHuffNode * AddNode(CHuffTree * htTree,CHuffNode * phn,int iVal, int iLayer)
{
    if (iLayer==phn->m_iLayer)
    {
        if (SetNode(phn,iVal,iLayer)!=NULL)
        {
            htTree->m_iCount++;
            return phn;
        }
    }
    CHuffNode * phnTemp;
    if (htTree->m_iCount!=1)
    {
        phn=FindRightUpFreeBranch(phn);
    }
    lbl1:
    if (phn->m_phnRight==NULL)
    {
        do
        {
            phnTemp=new CHuffNode;
            phnTemp->m_bIsLeaf=false;
            phnTemp->m_phnLeft=NULL;
            phnTemp->m_phnRight=NULL;
            phnTemp->m_phnUp=phn;
            phnTemp->m_iLayer=phn->m_iLayer+1;
            if (phn->m_phnLeft!=NULL)
                phn->m_phnRight=phnTemp;
            else
                phn->m_phnLeft=phnTemp;
            phn=phnTemp;
            htTree->m_iCount++;
        } while (iLayer!=phn->m_iLayer);
        if (SetNode(phn,iVal,iLayer)!=NULL)
        {
            htTree->m_iCount++;
            return phn;
        }
        else cout << "WHY???";
    }
    else cout << "Motherf";
    if (phn->m_phnUp==NULL)
    {
        phnTemp=new CHuffNode;
        phnTemp->m_bIsLeaf=false;
        phnTemp->m_phnLeft=phn;
        phnTemp->m_phnRight=NULL;
        phnTemp->m_phnUp=NULL;
        phnTemp->m_iLayer=phn->m_iLayer+1;
        phn=phnTemp;
        htTree->m_phnRoot=phn;
        htTree->m_iCount++;
    }
    goto lbl1;

}


/*
    //Using ZigZag Matrix example
    short unsigned **  aQTable=new short unsigned  * [8];
    for (int x=0;x<8;x++)
    {
        aQTable[x]=new short unsigned  [8];
    }
    for (int x=1;x<65;x++)
    {
        AddToZigZagMatrix(aQTable,x);
    }
    for(int x=0;x<8;x++)  // loop 3 times for three lines
    {
        for(int y=0;y<8;y++)  // loop for the three elements on the line
        {
            cout<< dec << (short unsigned) aQTable[x][y] << " ";  // display the current element out of the array
        }
        cout<<endl;  // when the inner loop is done, go to a new line
    }
*/

bool AddToZigZagMatrix(short unsigned ** aTable, byte bT)
{
#define nTableRows 8
#define cbElements 64
#define Up true
#define Down false
#define setQuantTableItem(bT,aT,ix,iy,iT) ({aT[iy][ix]=bT;iT++;})
    typedef bool Direction;

    static Direction fDirection=Down;
    static int iT=0;
    static int ix=0,iy=0;
    static int state=0;
    switch (state) {
    case 0: setQuantTableItem(bT,aTable,ix,iy,iT);/* начало функции */
        //return true;
        while (iT<cbElements-2)
        {
            if (fDirection==Down)
            {
                if (ix<nTableRows-1)++ix;else iy++;
                for (ix,iy;ix>=0&&iy<nTableRows;ix--,iy++)
                {

                    state = 1;/* возвратиться к "case 1" */
                    return false;
                case 1: setQuantTableItem(bT,aTable,ix,iy,iT);/* продолжить выполнения после точки возврата */;/* возвратиться к "case 1" */
                }
                ix++;
                iy--;
                fDirection=Up;
            }
            else
            {
                if (iy<nTableRows-1)++iy;else ++ix;
                for (ix,iy;ix<nTableRows&&iy>=0;ix++,iy--)
                {
                    state = 2;/* возвратиться к "case 1" */
                    return false;
                case 2: setQuantTableItem(bT,aTable,ix,iy,iT);
                }
                ix--;
                iy++;
                fDirection=Down;
            }
        }
        ix++;
        state=3;
        return false;
    case 3: setQuantTableItem(bT,aTable,ix,iy,iT);
    }
    fDirection=Down;
    iT=0;
    ix=0;
    iy=0;
    state=0;
    return true;
#undef Up
#undef Down
#undef cbElements
#undef nTableRows
#undef setQuantTableItem
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //zig_zag();
    char * szImagePath="F:\\Documents\\Computing\\Programming\\Projects\\QtCreator\\jpg2bmp-build-desktop\\debug\\dd85adf7.jpg";//="webcam.jpg";

    //char * szImagePath="x_fb7a21a4.jpg";
    ifstream inImage;
    //setlocale(LC_ALL, "Russian");
    inImage.open(szImagePath,ios::in| ios::binary);
    if (!inImage)
    {
        cout << "Невозможно открыть файл";
        return false;
    }
    //cout << "Файл открыт";
    //    cout.setf(ios::hex, ios::basefield);
    //    cout.setf(ios::hex, ios::basefield);
    //    cout.setf(ios::showbase);
    //    cout.setf(ios::hex|ios::showbase);
    //cout.width(3);
    word wMarker;
    word wSegmentSize;
    unsigned short int *** paQuantTables=0;
    unsigned short int ** aQuantTable=0;
    CHuffTable * phtList=0;
    CDCTTable dct;
    while  (inImage)
    {
        byte * pbSegment;
        inImage.read((char*)&wMarker,sizeof(word));
        wMarker=swap_endian<word>(wMarker);
        //        switch (wMarker)
        //        {
        //        case StartOfImage:
        //            continue;
        //        case EndOfImage:
        //            goto lblFinish;
        //        case App0:
        //        case QuantTableDef:
        //        default:
        //            inImage.read((char*)&wSegmentSize,sizeof(word));
        //            wSegmentSize=swap_endian<>(wSegmentSize);
        //            break;
        //        }
        if (wMarker==StartOfImage)continue;
        else if (wMarker==EndOfImage)break;
        else
        {
            inImage.read((char*)&wSegmentSize,sizeof(word));
            wSegmentSize=swap_endian<word>(wSegmentSize);
            switch  (wMarker)
            {
            case QuantTableDef:
                {
                    cout << "Marker:" << hex  << wMarker << endl;
                    cout << "Segment size:" << wSegmentSize<< endl;
#define nTableRows 8
#define cbElements 64
                    typedef bool Direction;
#define Up true
#define Down false
#define setQuantTableItem(is,bT,aT,ix,iy,iT) ({is.get(bT);/* cout << "char " << hex << (int) bT << endl*/;aT[iy][ix]=bT;iT++;})
                    byte nTableElementSize=0;
                    byte nTableIndex=0;
                    byte bT;
                    inImage.get(bT);
                    nTableElementSize=bT>>4;
                    nTableIndex=bT&0xF;
                    if (nTableElementSize==0) bT=8;
                    else bT=16;
                    unsigned short * aQuantTableRow;
                    //aQuantTableLim
                    //64*(nTableElementSize+1)
                    aQuantTable=new unsigned short *[nTableRows];
                    for (int ix=0;ix<nTableRows;ix++)
                    {
                        aQuantTableRow=new unsigned short [bT];
                        aQuantTable[ix]=aQuantTableRow;
                    }
                    int iT=0;
                    Direction fDirection=Down;
                    int ix=0,iy=0;
                    setQuantTableItem(inImage,bT,aQuantTable,ix,iy,iT);
                    while (iT<cbElements-2)
                    {
                        if (fDirection==Down)
                        {
                            if (ix<nTableRows-1)++ix;else iy++;
                            for (ix,iy;ix>=0&&iy<nTableRows;ix--,iy++)
                            {
                                setQuantTableItem(inImage,bT,aQuantTable,ix,iy,iT);
                            }
                            ix++;
                            iy--;
                            fDirection=Up;
                        }
                        else
                        {
                            if (iy<nTableRows-1)++iy;else ++ix;
                            for (ix,iy;ix<nTableRows&&iy>=0;ix++,iy--)
                            {
                                setQuantTableItem(inImage,bT,aQuantTable,ix,iy,iT);
                                //                            inImage.get(&bT);
                                //                            aQuantTable[ix][iy]=bT;
                                //                            iT++;
                            }
                            ix--;
                            iy++;
                            fDirection=Down;
                        }
                    }
                    ix++;
                    setQuantTableItem(inImage,bT,aQuantTable,ix,iy,iT);
                    //cout.precision(2);
                    //cout.width(2);
                    for(int x=0;x<8;x++)  // loop 3 times for three lines
                    {
                        for(int y=0;y<8;y++)  // loop for the three elements on the line
                        {
                            cout<< hex << (short int) aQuantTable[x][y] << " ";  // display the current element out of the array
                        }
                        cout<<endl;  // when the inner loop is done, go to a new line
                    }

                    paQuantTables=(unsigned short ***)realloc((void *)paQuantTables,sizeof(unsigned short ***)*(nTableIndex+1));
#undef Up
#undef Down
#undef cbElements
#undef nTableRows
                    //cout.setf(ios::hex);
                }
                break;
            case HuffBaselineDCT:
                {
                    cout << "Marker:" << hex  << wMarker << endl;
                    cout << "Segment size:" << wSegmentSize<< endl;
                    inImage.get(dct.m_bPrecision);
                    inImage.read((char*)&(dct.m_iHeight),sizeof(word));
                    SwapEndianWord(dct.m_iHeight);
                    inImage.read((char*)&(dct.m_iWidth),sizeof(word));
                    SwapEndianWord(dct.m_iWidth);
                    inImage.get(dct.m_bUnitsCount);
                    dct.m_puComponents=new CUnit[dct.m_bUnitsCount];
                    dct.m_iHmax=-1;
                    dct.m_iVmax=-1;
                    for (int ix=0;ix<dct.m_bUnitsCount;ix++)
                    {
                        inImage.read((char*)&(dct.m_puComponents[ix]),sizeof(CUnit));
                        if (dct.m_puComponents[ix].m_H>dct.m_iHmax)dct.m_iHmax=dct.m_puComponents[ix].m_H;
                        if (dct.m_puComponents[ix].m_V>dct.m_iVmax)dct.m_iVmax=dct.m_puComponents[ix].m_V;
                        //aUnits[ix]=swap_endian<CUnit>(aUnits[ix]);
                    }

                    cout << "lol";
                }
                break;
            case HuffmanTableDef:
                {
                    cout << "Marker:" << hex  << wMarker << endl;
                    cout << "Segment size:" << wSegmentSize<< endl;
                    CHuffTable * phtCurrent=new CHuffTable;
                    phtCurrent->next=0;
                    if (phtList!=0)
                    {
                        CHuffTable * phtTemp=phtList;
                        while (phtTemp->next!=0)phtTemp=phtTemp->next;
                        phtTemp->next=phtCurrent;
                    }
                    else
                    {
                        phtList=phtCurrent;
                    }
                    byte bT;
                    inImage.get(bT);
                    phtCurrent->m_nTableClass=static_cast<JpegHuffTableClass>(bT>>4);
                    phtCurrent->m_nTableIndex=bT&0xF;
                    //                    if (!((nTableClass==HuffClassAC)&&(nTableIndex==1)))
                    //                    {
                    //                        pbSegment=new byte[wSegmentSize];
                    //                        inImage.read((char*)pbSegment,sizeof(byte)*(wSegmentSize-3));
                    //                        cout << "Marker:" << hex  << wMarker << endl;
                    //                        cout << "Segment size:" << wSegmentSize<< endl;
                    //                        delete [] pbSegment;
                    //                        break;
                    //                    }
#define cbHuffCodes 16

                    streampos spCodeCount;
                    byte * pbHuffCodes;
                    CHuffNode * phn=new CHuffNode;
                    phn->m_bIsLeaf=false;
                    phn->m_phnUp=NULL;
                    phn->m_phnLeft=NULL;
                    phn->m_phnRight=NULL;
                    phn->m_iLayer=0;
                    phn->m_iData=0;
                    phtCurrent->m_htTree.m_phnRoot=phn;
                    phtCurrent->m_htTree.m_iCount=1;
                    int iOffset=0;
                    int iBaseOffset;
                    for (int ix=0;ix<cbHuffCodes;ix++)
                    {
                        inImage.get(bT);
                        if (bT>0)
                        {

                            spCodeCount=inImage.tellg();
                            iBaseOffset=cbHuffCodes-ix-1;
                            inImage.seekg((streamoff)iBaseOffset+iOffset,ios_base::cur);
                            //streampos spTemp=inImage.tellg();
                            pbHuffCodes=new byte[bT+1];
                            pbHuffCodes[bT]='\0';
                            inImage.read(pbHuffCodes,sizeof(byte)*bT);
                            cout << "Line: "<< ix << "Length: " << (int) bT << endl;
                            for (int iy=0;iy<bT;iy++)
                            {
                                cout << "Val: " <<  (int) pbHuffCodes[iy] << endl;
                                phn=AddNode(&(phtCurrent->m_htTree),phn,pbHuffCodes[iy],ix);
                            }
                            iOffset+=bT;
                            inImage.seekg(spCodeCount);
                            delete [] pbHuffCodes;
                        }
                    }
                    inImage.seekg((streamoff)iOffset,ios_base::cur);
                    streampos spTemp=inImage.tellg();
#undef cbHuffCodes

                }
                break;
            case StartOfScan:
                {

                    cout << "Marker:" << hex  << wMarker << endl;
                    cout << "Segment size:" << wSegmentSize<< endl;
                    cout << "Pedo" << sizeof(CComponent) << endl;
                    byte nComponents;
                    inImage.get(nComponents);
                    int ik;
                    if (nComponents<1||nComponents>4);// throw 0;
                    CComponent * pwComponents=(CComponent *)malloc(sizeof(CComponent)*nComponents);
                    for (int ix=0;ix<nComponents;ix++)
                    {
                        inImage.read((char*)pwComponents+ix*sizeof(CComponent),sizeof(CComponent));
                        //pwComponents[ix]=swap_endian<struct CComponent>(pwComponents[ix]);
                    }
                    byte bT;
                    word wT;
                    inImage.get(bT);
                    inImage.get(bT);
                    inImage.get(bT);

                    short ix=0; // Счетчик цикла
                    short ** aMatrix=new short *[8];
                    for (ix;ix<8;ix++)
                    {
                        aMatrix[ix]=new short[8];
                    }

                    const int cnByteSize=8;//Число бит в байте
#ifdef HACKY_TRICKY_SOLUTION
                    union OutNumber
                    {
                        byte b;
                        short s;
                        int i;
                        long l;
                        char data[];
                    };
                    union OutNumber * pOutNumber; // "Выходное" число

                    do
                    {
                        int nReadBits=2;
                        inImage.get(bT);
                        bT= bT<<nReadBits;

                        short nInBitCount = 25; // Число битов для чтения

                        int nTest = (nInBitCount)/cnByteSize;
                        int nOutNumberSize; // Реальный размер выходного числа
                        int nOutAlign=cnByteSize-(nInBitCount-(nInBitCount/cnByteSize)*cnByteSize); // Смещение от начаint ла выходного числа
//nInBitCount-
                        int nInCurrPos= nReadBits/*=nDummy*/, nOutCurrPos=0; // Текущая позиция в числах
                        byte bInRaw=bT, bInSlice, bOutSlice; // Только что считаный байт, нужный нам кусочек

                        int nInSliceSize, nOutSliceSize;

                        if ((nInBitCount)%cnByteSize!=0) // Если текущая позиция + количество бит для чтения
                            // не кратно размеру байта ix????
                        {
                            nOutNumberSize=(nInBitCount)/cnByteSize+1; // то создаём число с размером + дополнительным байтом
                            //fPutIn=false;
                        }
                        else
                        {
                            nOutNumberSize=(nInBitCount)/cnByteSize; // иначе, умещаем число в размер
                            //fPutIn=true;
                        }
                        pOutNumber = (union OutNumber *) malloc(nOutNumberSize);
//                        ::memchr((void *)pOutNumber,0,nOutNumberSize);
//                        ZeroMemory(pOutNumber,nOutNumberSize);

                        for (int cx=0;cx< nOutNumberSize; cx++)
                        {
                            pOutNumber->data[cx]='\0';
                        }

                        nOutNumberSize--;

                        while (nInBitCount>0)
                        {
                            if ((nReadBits+nInBitCount)>=cnByteSize) // Проверяем переполнение байта
                            {
                                nInSliceSize=cnByteSize-nReadBits; // Если переполнение - устанавливаем счетчик бит равным числу бит от cx до конца байта
                            }
                            else
                                nInSliceSize=nInBitCount; // в прот. случае устанавливаем в чило бит поданых на чтение

                            // nReadBits==0
                            if (nInCurrPos%cnByteSize==0) // Вслучае если количество считанных битов в байте равно нулю, читаем новый байт
                            {
                                inImage.get(bInRaw);
                            }

                            //bT1=bT;
                            bInSlice = ((unsigned char) bInRaw) >> (cnByteSize-nInSliceSize);
                            bInRaw = ((unsigned char) bInRaw) << (nInSliceSize);

                            if (0!=nOutAlign)
                            {
                                if (nOutAlign+nInSliceSize>= cnByteSize)
                                {
                                    nOutSliceSize= cnByteSize-nOutAlign;
                                    bOutSlice= ((unsigned char)bInSlice ) >>nInSliceSize- abs(nInSliceSize - nOutAlign);  //   nOutAlign
                                    pOutNumber->data[nOutNumberSize]=bOutSlice;
                                    nOutNumberSize--;
                                    bInSlice= ((unsigned char)bInSlice ) << nOutSliceSize + (cnByteSize - nInSliceSize);//  cnByteSize - nInSliceSize - (cnByteSize - );
                                    bInSlice= ((unsigned char)bInSlice ) >> nOutSliceSize + (cnByteSize - nInSliceSize);;// cnByteSize - nInSliceSize - (cnByteSize - nOutSliceSize);
                                    nInSliceSize-= nOutSliceSize;
                                    nInCurrPos+= nOutSliceSize;
                                    nInBitCount-= nOutSliceSize;
                                    nReadBits=0;
                                    nOutAlign=0;
                                    //bInSlice= ((unsigned char)bInSlice ) << cnByteSize - nInSliceSize - (cnByteSize - nOutSliceSize);
                                   //

                                    //bOutSlice= ((unsigned char)bInSlice ) >> ((cnByteSize - nOutSliceSize));
                                }
                            }

                            //bInSlice = ((unsigned char) bInSlice) << (cnByteSize-nInSliceSize);


                            bOutSlice= ((unsigned char)bInSlice ) << abs(nReadBits-(cnByteSize-nInSliceSize));
                            pOutNumber->data[nOutNumberSize]|=bOutSlice;
                            nReadBits+=nInSliceSize;
                            nInCurrPos+=nInSliceSize;
                            nInBitCount-=nInSliceSize;
                            nInSliceSize=0;

                            //nOutCurrPos=nOutAlign+nInSliceSize;

                            //pOutNumber->data[nOutNumberSize]=bInSlice;


                            if (nReadBits>=cnByteSize)
                            {
                                nReadBits=0;
                                nOutNumberSize--;
                            };
                        }

                        free(pOutNumber);
                    } while (!inImage.eof());
#else

//                    struct CPixel
//                    {
//                        short *** m_
//                    };


//                    short ** aMatrix=new short *[8];
//                    for (ix;ix<8;ix++)
//                    {
//                        aMatrix[ix]=new short[8];
//                    }
                    unsigned short **** pPixels = 0;
                    int nPixelCount=0;

                    vector<byte> vecBitArray;
                    int nBitRead=8;
                    do
                    {
                        ++nPixelCount;
                        pPixels= (unsigned short ****) realloc(pPixels,sizeof(unsigned short ***)*nPixelCount);
                        pPixels[nPixelCount-1]=0;

                        for (int cx=0,ix=0;ix<dct.m_bUnitsCount;cx++,ix++) // nComponents
                        {
                            for (int iy=0;iy<dct.m_puComponents[ix].m_H*dct.m_puComponents[ix].m_V;cx++,iy++)
                            {
                                pPixels[nPixelCount-1] = (unsigned short ***) realloc(pPixels[nPixelCount-1],sizeof(unsigned short **)*cx);

                                unsigned short ** aMatrix=new unsigned short *[8];
                                for (int ex=0;ex<8;ex++)
                                {
                                    aMatrix[ex]=new unsigned short[8];
                                    memchr(aMatrix[ex],'\0',sizeof(unsigned short)*8);
                                }

//                                for (int ex = 0;ex<8;ex++)
//                                {
//                                    pPixels[nPixelCount-1][ex]=new short[8];
//                                    memchr(pPixels[nPixelCount-1][ex],'\0',sizeof(short));
//                                }

                                pPixels[nPixelCount-1][cx]=aMatrix;

                                int nPos=0;

                                do
                                {
                                    byte bT1;
                                    byte bValue;

                                    CHuffTablePointer it = phtList;



                                    if (nPos==0)
                                        while ((it->m_nTableClass!= HuffClassDC)
                                               ||
                                               (pwComponents[ix].m_DC!= it->m_nTableIndex))
                                            it=it->next;
                                    else
                                        while ((it->m_nTableClass!=HuffClassAC)
                                               ||
                                               (pwComponents[ix].m_AC!= it->m_nTableIndex))
                                            it=it->next;

                                    CHuffNodePointer it1=it->m_htTree.m_phnRoot;

                                    do
                                    {
                                        if (nBitRead==8){ inImage.get(bT); nBitRead=0; };

                                        bT1=((unsigned char)bT)&0x80;
                                        bT1=((unsigned char)bT1)>>7;
                                        bT= bT << 1;
                                        nBitRead++;

                                        if (bT1==0)
                                        {
                                            cout << "left";
                                            it1=it1->m_phnLeft;
                                        }
                                        if (bT1==1)
                                        {
                                            cout << "right";
                                            it1=it1->m_phnRight;
                                        }

                                    } while(!it1->m_bIsLeaf);

                                    cout << "leaf data" << dec << it1->m_iData;

                                    if (it1->m_iData==0) bValue=0;
                                    else
                                    {
                                        int bx=0;
                                        int nData= it1->m_iData;
                                        if (nPos!=0)
                                        {
                                            bT1= it1->m_iData&0xF0;
                                            bT1 = bT1 >> 0xF;
                                            for (int bx=0; bx<bT1 ;bx++)
                                            {
                                                AddToZigZagMatrix(aMatrix,0);
                                            }

                                            nData= it1->m_iData&0xF;
                                        }

                                        while (bx< nData)
                                        {
                                            if (nBitRead==8){ inImage.get(bT);nBitRead=0; };
                                            bT1=((unsigned char)bT)&0x80;
                                            bT1=((unsigned char)bT1)>>7;
                                            bT= bT << 1;
                                            nBitRead++;
                                            vecBitArray.push_back(bT1);
                                            bx++;
                                        }

                                        bValue= 0;
                                        for (int vx=0;vx<vecBitArray.size();vx++)
                                        {
                                            bValue=bValue<<1;
                                            bValue|=vecBitArray[vx];
                                        }
                                    }

                                    //byte bX= ;

                                    if ((0 != bValue) && (0==((bValue << (cnByteSize-it1->m_iData))&0x80)))
                                        bValue=bValue-pow(2,it1->m_iData)+1;


                                    AddToZigZagMatrix(aMatrix,bValue);

                                    vecBitArray.clear();

                                    if (nPos!=0 && 0 == bValue) break;

                                    if  (nPos==0)
                                    {
                                        int x=0;
                                        x++;

                                        x=8;
                                    }

                                }while(++nPos < 64);
                            }
                        }


                    } while (!inImage.eof());

                    //                                //bT1
                    //                                bT=bT<<1;
                    //                            } while (++ix<8);



                //                        for (int ix=0;ix<cnByteSize; ix++)
                //                        {
                //                            bT2= bT&0x80;
                //                            vecBitArray.push_back(bT2);
                //                            bT= bT << 1;
                //                        }
#if 0
                    for (ix=0;ix<dct.m_bUnitsCount;ix++)
                    {
                        for (int iy=0;iy<dct.m_puComponents[ix].m_H*dct.m_puComponents[ix].m_V;iy++)
                        {

                            CHuffTablePointer it = phtList;
                            while (it->m_nTableClass!=HuffClassDC&&pwComponents[ix].m_DC!=it->m_nTableIndex)it=it->next;
                            //pwComponents[ix].m_DC=
                            //dct.m_puComponents[ix].
                            //pwComponents[ix].m_AC
                            CHuffNodePointer it1=it->m_htTree.m_phnRoot;

                            /*
                            while (inImage.get(bT))
                            {
                                do
                                {
                                    cout << ix;
                                    bT1=((unsigned char)bT)&0x80;
                                    bT1=((unsigned char)bT1)>>7;
                                    if (bT1==0)
                                    {
                                        cout << "left";
                                        it1=it1->m_phnLeft;
                                    }
                                    if (bT1==1)
                                    {
                                        cout << "right";
                                        it1=it1->m_phnRight;
                                    }
                                    if (it1->m_bIsLeaf)
                                    {
                                        cout << "leaf data" << dec << it1->m_iData;
                                        if (it1->m_iData==0) WTF(0);
                                        else
                                        {
                                            if (it1->m_iData==8)
                                                inImage.get(bT);
                                            else if (it1->m_iData<8)
                                            {
                                                do
                                                {
                                                    bT1=bT&0x80;
                                                    bT=bT<<1;
                                                } while (++ix<8);
                                                bT=
                                                    }
                                            else if (it1->m_iData>8)
                                            {

                                            }


                                            WTF(bT);
                                        }
                                    }
                                    //bT1
                                    bT=bT<<1;
                                } while (++ix<8);
                            }
                            */
                        }
                    }

#endif
#endif

//                    do
//                    {
//                        int nDummy=2;
//                        inImage.get(bT);
//                        bT= bT<<nDummy;

//                        short nInBitCount = 5; // Число битов для чтения

//                        int nOutNumberSize; // Реальный размер выходного числа
//                        int nOutAlign=(nInBitCount)%cnByteSize; // Смещение от начаint ла выходного числа

//                        int nInCurrPos/*=nDummy*/, nOutCurrPos=0; // Текущая позиция в числах
//                        byte bInRaw=bT, bInSlice; // Только что считаный байт, нужный нам кусочек

//                        int nInSliceSize;

//                        if (nOutAlign!=0) // Если текущая позиция + количество бит для чтения
//                            // не кратно размеру байта ix????
//                        {
//                            nOutNumberSize=(nInBitCount)/cnByteSize+1; // то создаём число с размером + дополнительным байтом
//                            //fPutIn=false;
//                        }
//                        else
//                        {
//                            nOutNumberSize=(nInBitCount)/cnByteSize; // иначе, умещаем число в размер
//                            //fPutIn=true;
//                        }
//                        pOutNumber = (union OutNumber *) malloc(nOutNumberSize);
//                        ::memchr((void *)pOutNumber,0,nOutNumberSize);

//                        while (nInBitCount>0)
//                        {
//                            if ((nInCurrPos+nInBitCount)>=cnByteSize) // Проверяем переполнение байта
//                            {
//                                nInSliceSize=cnByteSize-nInCurrPos; // Если переполнение - устанавливаем счетчик бит равным числу бит от cx до конца байта
//                            }
//                            else
//                                nInSliceSize=nInBitCount; // в прот. случае устанавливаем в чило бит поданых на чтение

//                            if (nInCurrPos==0)
//                            {
//                                inImage.get(bInRaw);
//                            }


//                            //bT1=bT;
//                            bInSlice = ((unsigned char) bInRaw) >> (cnByteSize-nInSliceSize);
//                            bInRaw = ((unsigned char) bInRaw) << (nInSliceSize);

//                            bInSlice = ((unsigned char) bInSlice) << (cnByteSize-nInSliceSize-nOutAlign);
//                            nOutCurrPos=nOutAlign+nInSliceSize;
//                            nInBitCount-=nInSliceSize;
//                            //pOutNumber->data[nOutNumberSize]=bInSlice;
//                            nOutNumberSize--;

//                            if (nInCurrPos+nInSliceSize>=cnByteSize)
//                                nInCurrPos=0;
//                            else
//                                nInCurrPos+=nInSliceSize;

//                        }

//                        free(pOutNumber);
//                    } while (!inImage.eof());




//                    byte bT1;
//                    //                        int nLast=cCurrPos+ix;
//                    //                        bool fPutIn;
//                    //                        int nCurrBitsCount;
//                    //                        int ic=0;

//                    //CHuffNodeIterator it=phtList[];


//                    // offset = 277
//                    //inImage.get(bT);
//                    /*
//                    ix=5;
//                    int max=8;
//                    short cx=0;// текущая позиция
//                    int ic=0;
//                    short ab; // Число бит до конца чтения или байта
//                    union UNI
//                    {
//                        byte b;
//                        short s;
//                        int i;
//                        long l;
//                        char data[];
//                    };
//                    union UNI * pUni;
//                    ix=5;
//*/
//                    ix=5;// Начальное значение, сколько бит считать

//                    short nBitUntilByteLeft; // Число бит до конца байта
//                    short cCurrPos=0;//Текущая позиция

//                    do
//                    {
//                        /*
//                        bT1=0;
//                        int nSize;

//                        if ((cx+ix)%8!=0)
//                            nSize=(cx+ix)/8+1;
//                        else
//                            nSize=(cx+ix)/8;
//                        pUni = (union UNI *) malloc(nSize);
//                        //pUni->l=500000;
//                        // 0 0 0 0 0 1 1 1 0 1 0 1 1 ...
//                        // 0 1 2 3 4 5 6 7 0 1 2 3 4 ...
//                        // . . . . ^ . . ^ . . . ^ . ...
//                        // . . . . | . . | . . . | . ...
//                        // . . . . cx. .max. . .cx+ix...
//                        while (ix>0) // В ix - количество бит для чтения
//                        {
//                            if ((cx+ix)>=8) // Проверяем переполнение байта
//                            {
//                                ab=8-cx; // Если переполнение - устанавливаем счетчик бит равным числу бит от cx до конца байта
//                            }
//                            else
//                                ab=ix; // в прот. случае устанавливаем в чило бит поданых на чтение
//                            if (cx==0)
//                            {
//                                inImage.get(bT);
//                            }
//                            nSize--;

//                            //pUni[ic]
//                            bT1 = ((unsigned char) bT) >> (8-ab);
//                            bT = ((unsigned char) bT) << (ab);
//                            pUni->data[nSize]=bT1;

//                            //bT1=bT1 << ab;
//                            //bT1 |= ((unsigned char) bT) >> (8-ab);
//                            //bT = ((unsigned char) bT) << (ab);

//                            ix-=ab;
//                            if (cx+ab>=8)
//                                cx=0;
//                            else
//                                cx+=ab;
//                            ic++;
//                        }
//                        ix=7;
//                        // Функциональный блок

//                        //if (cx<8)
//                        //{
//                        //    bT1= ((unsigned char) bT) >> (8-ix);
//                        //    bT = ((unsigned char) bT) << (ix);
//                        //    cx+=ix;
//                        //}
//                        //else
//                        //{
//                        //    inImage.get(bT);
//                        //    cx=0;
//                        //}
//                        // обработка фала
//                        cout << hex << bT1;
//                        ix=4;

//                        free(pUni);
//                        cout << "blabla";
//                        */
//                        bT1=0;
//                        int nLargeNumberSize;
//                        int nLast=cCurrPos+ix;
//                        bool fPutIn;
//                        int nCurrBitsCount;
//                        int ic=0;
//                        if ((ix)%nByteSize!=0) // Если текущая позиция + количество бит для чтения
//                                               // не кратно размеру байта ix????
//                        {
//                            nLargeNumberSize=(ix)/nByteSize+1; // то создаём число с размером + дополнительным байтом
//                            fPutIn=false;
//                        }
//                        else
//                        {
//                            nLargeNumberSize=(ix)/nByteSize; // иначе, умещаем число в размер
//                            fPutIn=true;
//                        }
//                        pNumber = (union OutNumber *) malloc(nLargeNumberSize);
//                        ::memchr((void *)pNumber,0,nLargeNumberSize);
//                        // 0 0 0 0 0 1 1 1 0 1 0 1 1 1 0 1 0
//                        // 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0
//                        // . . . . ^ . . ^ . . . ^ . ..^
//                        // . . . . | . . | . . . | . ..|
//                        // .cCurrPos.nByteSize.nLast...nLast(^2)

//                        while (ix>0) // В ix - количество бит для чтения
//                        {
//                            if ((cCurrPos+ix)>=nByteSize) // Проверяем переполнение байта
//                            {
//                                nCurrBitsCount=nByteSize-cCurrPos; // Если переполнение - устанавливаем счетчик бит равным числу бит от cx до конца байта
//                            }
//                            else
//                                nCurrBitsCount=ix; // в прот. случае устанавливаем в чило бит поданых на чтение

//                            if (cCurrPos==0)
//                            {
//                                inImage.get(bT);
//                            }

//                            //bT1=bT;
//                            bT1 = ((unsigned char) bT) >> (nByteSize-nCurrBitsCount);
//                            bT = ((unsigned char) bT) << (nCurrBitsCount);

//                            nLargeNumberSize--;


//                            if (cCurrPos!=0)
//                            {

//                            }
//                            pNumber->data[nLargeNumberSize]=bT1;
//                            //pUni[ic]
//                            //if (currPos!=0) then blabla bla
//                            /*
//                            if (fPutIn)
//                            {
//                                pNumber->data[nLargeNumberSize]=bT1;
//                            }
//                            else
//                            {
//                                pNumber->data[nLargeNumberSize]=bT1;
//                            }
//                            */


//                            //bT1=bT1 << ab;
//                            //bT1 |= ((unsigned char) bT) >> (8-ab);
//                            //bT = ((unsigned char) bT) << (ab);

//                            ix-=nCurrBitsCount;
//                            if (cCurrPos+nCurrBitsCount>=nByteSize)
//                                cCurrPos=0;
//                            else
//                                cCurrPos+=nCurrBitsCount;
//                            ic++;
//                        }
//                        ix=7;
//                        // Функциональный блок

//                        //if (cx<8)
//                        //{
//                        //    bT1= ((unsigned char) bT) >> (8-ix);
//                        //    bT = ((unsigned char) bT) << (ix);
//                        //    cx+=ix;
//                        //}
//                        //else
//                        //{
//                        //    inImage.get(bT);
//                        //    cx=0;
//                        //}
//                        // обработка фала
//                        cout << hex << bT1;
//                        //ix=4;

//                        free(pNumber);
//                        cout << "blabla";
//                    } while (!inImage.eof());
                    //for (int cx=0;cx<8;cx++)
                    //{

                    //}
                    /*bT1=((unsigned char)bT)&0x80;
                    bT1=((unsigned char)bT1)>>7;

                    bT=bT<<1;
                    */
                    for (ix=0;ix<dct.m_bUnitsCount;ix++)
                    {
                        for (int iy=0;iy<dct.m_puComponents[ix].m_H*dct.m_puComponents[ix].m_V;iy++)
                        {
                            CHuffTablePointer it = phtList;
                            while (it->m_nTableClass!=HuffClassDC&&pwComponents[ix].m_DC!=it->m_nTableIndex)it=it->next;
                            //pwComponents[ix].m_DC=
                            //dct.m_puComponents[ix].
                            //pwComponents[ix].m_AC
                            CHuffNodePointer it1=it->m_htTree.m_phnRoot;

                            /*
                            while (inImage.get(bT))
                            {
                                do
                                {
                                    cout << ix;
                                    bT1=((unsigned char)bT)&0x80;
                                    bT1=((unsigned char)bT1)>>7;
                                    if (bT1==0)
                                    {
                                        cout << "left";
                                        it1=it1->m_phnLeft;
                                    }
                                    if (bT1==1)
                                    {
                                        cout << "right";
                                        it1=it1->m_phnRight;
                                    }
                                    if (it1->m_bIsLeaf)
                                    {
                                        cout << "leaf data" << dec << it1->m_iData;
                                        if (it1->m_iData==0) WTF(0);
                                        else
                                        {
                                            if (it1->m_iData==8)
                                                inImage.get(bT);
                                            else if (it1->m_iData<8)
                                            {
                                                do
                                                {
                                                    bT1=bT&0x80;
                                                    bT=bT<<1;
                                                } while (++ix<8);
                                                bT=
                                                    }
                                            else if (it1->m_iData>8)
                                            {

                                            }


                                            WTF(bT);
                                        }
                                    }
                                    //bT1
                                    bT=bT<<1;
                                } while (++ix<8);
                            }
                            */
                        }
                    }

                }

                //}
                break;
            default:
                {
                    pbSegment=new byte[wSegmentSize];
                    inImage.read((char*)pbSegment,sizeof(byte)*(wSegmentSize-2));
                    cout << "Marker:" << hex  << wMarker << endl;
                    cout << "Segment size:" << wSegmentSize<< endl;
                    delete [] pbSegment;
                }
            }

        }
//        cout << inImage;
    }

    //lblFinish:
    return a.exec();
}

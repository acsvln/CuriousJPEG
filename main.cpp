#include <QtCore/QCoreApplication>
#include <iostream>
#include <fstream>
#include <clocale>

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

struct CHuffTable;

struct CHuffNode
{
    int m_iData;
    CHuffNode * m_phnLeft;
    CHuffNode * m_phnRight;
    CHuffNode * m_phnUp;
    bool m_bIsLeaf;
    signed int m_iLayer;
};

typedef CHuffNode * CHuffNodeIterator;
typedef CHuffTable * CHuffTableIterator;

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

bool WTF(short unsigned ** aTable, int iRowsCount, int iColsCount, byte bT)
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
    char * szImagePath="dd85adf7.jpg";//="webcam.jpg";
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

                    for(int x=0;x<8;x++)  // loop 3 times for three lines
                    {
                        for(int y=0;y<8;y++)  // loop for the three elements on the line
                        {
                            cout<< dec << (int) aQuantTable[x][y] << " ";  // display the current element out of the array
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
                    byte bT1;
                    short ix=0;
                    //CHuffNodeIterator it=phtList[];

                    short ** aMatrix=new short *[8];
                    for (ix;ix<8;ix++)
                    {
                        aMatrix[ix]=new short[8];
                    }


//                        __asm
//                        (
//                                "movl 4,%eax\n"
//                                "movl 5,%ebx"
//                        );
                        for (ix=0;ix<dct.m_bUnitsCount;ix++)
                        {
                            for (short iy=0;iy<dct.m_puComponents[ix].m_H*dct.m_puComponents[ix].m_V;iy++)
                            {
                                /*
                                CHuffTableIterator it = phtList;
                                while (it->m_nTableClass!=HuffClassDC&&pwComponents[ix].m_DC!=it->m_nTableIndex)it=it->next;
                                //pwComponents[ix].m_DC=
                                //dct.m_puComponents[ix].
                                //pwComponents[ix].m_AC
                                CHuffNodeIterator it1=it->m_htTree->mphn_Root;
                                while (inImage.get(bT))
                                {
                                    do
                                    {
                                        cout << ix;
                                        bT1=bT&0x80;
                                        if (bT1==0)it1=it1->m_phnLeft;
                                        if (bT1==1)it1=it1->m_phnRight;
                                        if (it1->m_bIsLeaf)
                                        {
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

           // }
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
        // cout << inImage;
    }

    lblFinish:
    return a.exec();
}

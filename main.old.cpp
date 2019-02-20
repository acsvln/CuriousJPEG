#include <QtCore/QCoreApplication>
#include "cstdlib"
#include <iostream>
#include <fstream>
#include <istream>
#include <clocale>
#include <vector>
#include <cmath>
#include <QDebug>

using namespace std;

#define ZIG_ZAG_TEST 0
#undef ZIG_ZAG_TEST
#ifndef QT_NO_DEBUG

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

#endif

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

typedef unsigned short int word;
typedef char byte;

typedef unsigned short int number;

const int cnMatrixSide = 8;
const int cnMatrixDimension = cnMatrixSide*cnMatrixSide; // 64


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
};

#pragma pack(pop)


struct CHuffNode
{
    int m_iData;
    CHuffNode * m_phnLeft;
    CHuffNode * m_phnRight;
    CHuffNode * m_phnUp;
    bool m_bIsLeaf;
    signed int m_iLayer;
};

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

typedef CHuffNode * CHuffNodePointer;

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

CHuffNode * FindRightUpFreeBranch(CHuffNode * phn)
{
    if (phn->m_phnRight==NULL || phn->m_phnUp==NULL)
    {
        return phn;
    }
    else return FindRightUpFreeBranch(phn->m_phnUp);
}

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
        while (iT<cnMatrixDimension-2)
        {
            if (fDirection==Down)
            {
                if (ix<cnMatrixSide-1)++ix;else iy++;
                for (ix,iy;ix>=0&&iy<cnMatrixSide;ix--,iy++)
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
                if (iy<cnMatrixSide-1)++iy;else ++ix;
                for (ix,iy;ix<cnMatrixSide&&iy>=0;ix++,iy--)
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
#undef setQuantTableItem
}

void PrintMatrix(number ** aMatrix, char szName[], int n=-1,bool fHex= true)
{
    cout << "Matrix\t" << szName;

    if (n!= -1)
    {
        cout << "\t" << n;
    }
    cout << ":";

    //cout.precision(2);
    //cout.width(2);

     //cout.setf(ios::hex);
    for(int x=0;x<cnMatrixSide;x++)  // loop 3 times for three lines
    {
        for(int y=0;y<cnMatrixSide;y++)  // loop for the three elements on the line
        {
            if (fHex)
                cout<< hex;

            cout << (short int) aMatrix[x][y] << " ";// display the current element out of the array
        }
        cout<<endl;  // when the inner loop is done, go to a new line
    }

}


#define PI 3.14159265358979323846

#define ROUND(a) 	(((a)<0)?(int)((a)-0.5):(int)((a)+0.5))
#define NORMALIZE(n) ((n<0) ? 0 : ((n>255) ? 255 : n));
#define N 8

int Quantum[ N ][ N ];
float C[ N ][ N ];
float Ct[ N ][ N ];

struct zigzag {
    int row;
    int col;
} ZigZag[ N * N ] =
{
    {0, 0},
    {0, 1}, {1, 0},
    {2, 0}, {1, 1}, {0, 2},
    {0, 3}, {1, 2}, {2, 1}, {3, 0},
    {4, 0}, {3, 1}, {2, 2}, {1, 3}, {0, 4},
    {0, 5}, {1, 4}, {2, 3}, {3, 2}, {4, 1}, {5, 0},
    {6, 0}, {5, 1}, {4, 2}, {3, 3}, {2, 4}, {1, 5}, {0, 6},
    {0, 7}, {1, 6}, {2, 5}, {3, 4}, {4, 3}, {5, 2}, {6, 1}, {7, 0},
    {7, 1}, {6, 2}, {5, 3}, {4, 4}, {3, 5}, {2, 6}, {1, 7},
    {2, 7}, {3, 6}, {4, 5}, {5, 4}, {6, 3}, {7, 2},
    {7, 3}, {6, 4}, {5, 5}, {4, 6}, {3, 7},
    {4, 7}, {5, 6}, {6, 5}, {7, 4},
    {7, 5}, {6, 6}, {5, 7},
    {6, 7}, {7, 6},
    {7, 7}
};



void Initalise(int quality)
{
  int i,j;

//subsampling matrix
  for (i=0;i<N;i++)
    for (j=0;j<N;j++)
      Quantum[i][j] = 16 + ( 1 + i + j ) * quality;

// define DCT and transposed DCT matrix:
// C(i,j) = 1/sqrt(N), i=0
// C(i,j) = sqrt(2/N) * cos(((2*j+1)*i*PI)/(2*N)), i<>0

  for(j=0;j<N;j++)
  {
    C[0][j]= 1.0 / sqrt((float) N);
    Ct[j][0]=C[0][j];
  }

  for (i=1;i<N;i++)
    for (j=0;j<N;j++)
          {
            C[i][j]= sqrt(2.0/N) * cos( ((2*j+1)*i*PI)/(2.0*N) );
        Ct[j][i]=C[i][j];
          }

  for (i=0;i<N*N;i++)
  {
    ZigZag[i].row = i%N;
    ZigZag[i].col = i/N;
  }

}

void MatrixMul(float input[N][N], float output[N][N],
    float C[N][N],float Ct[N][N])
{
  float temp[N][N];
  float temp1;
  int i,j,k;

  //output = C * input * Ct;

  for (i=0;i<N;i++)
    for (j=0;j<N;j++)
    {
      temp[i][j]=0.0;
      for(k=0;k<N;k++)
        temp[i][j]+=input[i][k] * Ct[k][j];
    }

  for (i=0;i<N;i++)
    for (j=0;j<N;j++)
    {
      temp1=0.0;
      for (k=0;k<N;k++)
        temp1+=C[i][k]*temp[k][j];
          output[i][j]=temp1;
    }
}

void DCT(float input[N][N],float output[N][N])
{
  int i,row,col;
  MatrixMul(input,output,C,Ct);

  for (i=0;i<(N*N);i++)
  {
    row=ZigZag[i].row;
    col=ZigZag[i].col;
    output[row][col] = ROUND( output[row][col] / Quantum[row][col] );
  }

}

void idct(float **Matrix, float **DCTMatrix, int _N, int M){
    int i, j, u, v;

    for (u = 0; u < _N; ++u) {
        for (v = 0; v < M; ++v) {
          Matrix[u][v] = 1/4.*DCTMatrix[0][0];
          for(i = 1; i < _N; i++){
          Matrix[u][v] += 1/2.*DCTMatrix[i][0];
           }
           for(j = 1; j < M; j++){
          Matrix[u][v] += 1/2.*DCTMatrix[0][j];
           }

           for (i = 1; i < _N; i++) {
                for (j = 1; j < M; j++) {
                    Matrix[u][v] += DCTMatrix[i][j] * cos(M_PI/((float)_N)*(u+1./2.)*i)*cos(M_PI/((float)M)*(v+1./2.)*j);
                }
            }
        Matrix[u][v] *= 2./((float)_N)*2./((float)M);
        }
    }
 }

void iDCT(float input[N][N],float output[N][N])
{
  int i,j,row,col;
  float result;
  for (i=0;i<(N*N);i++)
  {
    row=ZigZag[i].row;
    col=ZigZag[i].col;
    input[row][col] = (signed char)input[row][col] * Quantum[row][col];
  }

  MatrixMul(input,output,Ct,C);

  for (i=0;i<N;i++)
    for (j=0;j<N;j++)
      output[i][j]=NORMALIZE(output[i][j]);

}

float **calloc_mat(int dimX, int dimY){
    float **m = (float**) calloc(dimX, sizeof(float*));
    float *p = (float*) calloc(dimX*dimY, sizeof(float));
    int i;
    for(i=0; i <dimX;i++){
    m[i] = &p[i*dimY];

    }
   return m;
}



void free_mat(float **m){
  free(m[0]);
  free(m);
}




int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
#ifndef QT_NO_DEBUG
#ifdef ZIG_ZAG_TEST
    zig_zag();
    return false;
#endif
#endif
    char * szImagePath="D:\git\CuriousJPEG";//="webcam.jpg";
    //char * szImagePath="x_fb7a21a4.jpg";
    //setlocale(LC_ALL, "Russian");

    ifstream inImage;    
    inImage.open(szImagePath,ios::in| ios::binary);
    if (!inImage)
    {
        cout << "Невозможно открыть файл";
        return false;
    }
    cout << "Файл открыт";

    word wMarker;
    word wSegmentSize;

    //    cout.setf(ios::hex, ios::basefield);
    //    cout.setf(ios::hex, ios::basefield);
    //    cout.setf(ios::showbase);
    //    cout.setf(ios::hex|ios::showbase);
    //cout.width(3);


    number *** paQuantTables=0;
    CHuffTable * phtList=0;
    CDCTTable dct;

    while  (inImage)
    {
        byte * pbSegment;
        inImage.read((char*)&wMarker,sizeof(word));
        wMarker=swap_endian<word>(wMarker);

        if (wMarker==StartOfImage)continue;
        else if (wMarker==EndOfImage)break;
        else
        {
          byte bT;

            inImage.read((char*)&wSegmentSize,sizeof(word));
            wSegmentSize=swap_endian<word>(wSegmentSize);

            cout << "Marker:" << hex  << wMarker << endl;
            cout << "Segment size:" << wSegmentSize<< endl;

            switch  (wMarker)
            {
            case QuantTableDef:
                {
                    number ** aQuantTable=0;
                    byte nTableElementSize=0;
                    byte nTableIndex=0;
                    byte bT;

                    inImage.get(bT);

                    nTableElementSize= bT>>4;
                    nTableIndex= bT&0xF;

                    if (nTableElementSize==0) bT=8;
                    else bT=16;

                    aQuantTable=new unsigned short *[cnMatrixSide];
                    for (int ix=0;ix<cnMatrixSide;ix++)
                    {
                        number * aQuantTableRow=new unsigned short [bT];
                        aQuantTable[ix]=aQuantTableRow;
                    }

                    do
                        inImage.get(bT);
                    while (!AddToZigZagMatrix(aQuantTable,bT));

                    PrintMatrix(aQuantTable,"Data Quant Table",nTableIndex);

                    paQuantTables=(number ***)realloc((void *)paQuantTables,sizeof(number ***)*(nTableIndex+1));
                    paQuantTables[nTableIndex]= aQuantTable;


                }
                break;
            case HuffBaselineDCT:
                {
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
                }
                break;
            case HuffmanTableDef:
                {
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

                    inImage.get(bT);
                    phtCurrent->m_nTableClass=static_cast<JpegHuffTableClass>(bT>>4);
                    phtCurrent->m_nTableIndex=bT&0xF;

                    const int cnHuffCodes = 16;

                    streampos spNextCode;
                    byte * pbHuffCodes;
                    CHuffNode * phn=new CHuffNode;

//                    phn->m_phnUp=NULL;
//                    phn->m_phnLeft=NULL;
//                    phn->m_phnRight=NULL;
//                    phn->m_iLayer=0;
//                    phn->m_iData=0;

                    memset (phn,'\0',sizeof(CHuffNode));

                    phn->m_bIsLeaf=false;

                    phtCurrent->m_htTree.m_phnRoot=phn;
                    phtCurrent->m_htTree.m_iCount=1;
                    int iOffset=0;
                    int iBaseOffset;

                    for (int ix=0;ix<cnHuffCodes;ix++)
                    {
                        inImage.get(bT);
                        if (bT>0)
                        {
                            spNextCode=inImage.tellg();
                            iBaseOffset=cnHuffCodes-ix-1;
                            inImage.seekg((streamoff)iBaseOffset+iOffset,ios_base::cur);
                            //streampos spTemp=inImage.tellg();
                            pbHuffCodes=new byte[bT+1];
                            pbHuffCodes[bT]='\0';
                            inImage.read(pbHuffCodes,sizeof(byte)*bT);
                            cout << "Length: "<< ix << "Count: " << (int) bT << endl;
                            for (int iy=0;iy<bT;iy++)
                            {
                                cout << "Value: " <<  (int) pbHuffCodes[iy] << endl;
                                phn=AddNode(&(phtCurrent->m_htTree),phn,pbHuffCodes[iy],ix);
                            }
                            iOffset+=bT;
                            inImage.seekg(spNextCode);
                            delete [] pbHuffCodes;
                        }
                    }
                    inImage.seekg((streamoff)iOffset,ios_base::cur);


                }
                break;
            case StartOfScan:
                {
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

                    byte bNext=0;

                    unsigned short **** pPixels = 0;
                    int nPixelCount=0;

                    int nDataUnitCount= 0;

                    for (int ix = 0; ix < dct.m_bUnitsCount; ++ix) {
                        nDataUnitCount+= dct.m_puComponents[ix].m_H*dct.m_puComponents[ix].m_V;

                    }
                    vector<byte> vecBitArray;
                    int nBitRead=8;

                    Initalise(24);

                    do
                    {

                        if (bNext==-1)//0xFF
                        {
                            int pos= inImage.tellg();
                            inImage.seekg(1,inImage.ios_base::cur);
                            inImage.read((char*)&wMarker,sizeof(word));
                            //wMarker=swap_endian<word>(wMarker);
                            if (wMarker==EndOfImage) break;
                            inImage.seekg(pos);
                        }

                        ++nPixelCount;
                        pPixels= (unsigned short ****) realloc(pPixels,sizeof(unsigned short ***)*nPixelCount);
                        pPixels[nPixelCount-1]=0;

                        for (int cx=0,ix=0;ix<dct.m_bUnitsCount;ix++) // nComponents
                        {
                            int nx=cx;
                            unsigned short ** aMatrix;
                            number dx=0;

                            for (int iy=0;iy<dct.m_puComponents[ix].m_H*dct.m_puComponents[ix].m_V;cx++,iy++)
                            {

                                pPixels[nPixelCount-1] = (unsigned short ***) realloc(pPixels[nPixelCount-1],sizeof(unsigned short **)*(cx+1));

                                aMatrix=new unsigned short *[8];
                                for (int ex=0;ex<8;ex++)
                                {
                                    aMatrix[ex]=new unsigned short[8];
                                    memchr(aMatrix[ex],'\0',sizeof(unsigned short)*8);

                                    for (int ix = 0; ix < 8; ++ix) {
                                        aMatrix[ex][ix]=99;
                                    }
                                }

                                pPixels[nPixelCount-1][cx]=aMatrix;

                                int nPos=0;

                                do
                                {
                                    int nData;

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

                                        if (nBitRead==8){
                                            inImage.get(bT);nBitRead=0;
                                            bNext= inImage.peek();
                                        };

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

                                    nData= it1->m_iData;

                                    if (nPos!=0 && 0 == nData)
                                    {
                                        for (int ix = 0; ix < 64-nPos; ++ix) {
                                             AddToZigZagMatrix(aMatrix,0);
                                        }
                                        break;
                                    }
                                    else
                                    {
                                        int bx=0;
                                        if (nPos!=0)
                                        {
                                            bT1= it1->m_iData&0xF0;
                                            bT1 = bT1 >> 4;
                                            for (int bx=0; bx<bT1 ;bx++)
                                            {
                                                AddToZigZagMatrix(aMatrix,0);
                                                ++nPos;
                                            }

                                            nData= it1->m_iData&0xF;
                                        }

                                        while (bx< nData)
                                        {
                                            if (nBitRead==8){
                                                inImage.get(bT);nBitRead=0;
                                                bNext= inImage.peek();
                                            };
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

                                    if (/*(0 != bValue) &&*/ (0==((bValue << (cnByteSize-nData))&0x80)))
                                        bValue=bValue-pow(2,nData)+1;


                                    AddToZigZagMatrix(aMatrix,bValue);

                                    vecBitArray.clear();



                                }while(++nPos < 64);

                                dx= aMatrix[0][0]= aMatrix[0][0]+dx;

                                PrintMatrix(aMatrix,"DQT");

                                float **aMatrix1 = calloc_mat(N,N);
                                float **aMatrix2 = calloc_mat(N, N);
                                //float **testiDCT = calloc_mat(dimX, dimY);

                                for (int i = 0; i < 8; ++i) {
                                  for (int j = 0; j < 8; ++j) {
                                    aMatrix[i][j]*= 0xFF&paQuantTables[dct.m_puComponents[ix].m_bDQTId][i][j];
                                    cout << aMatrix[i][j] << ' ';
                                    signed iz = (signed)((short)aMatrix[i][j]);
                                    aMatrix1[i][j]= (float)(iz);
                                  }
                                  cout << endl;
                                }




//                                double S[8][8];
//                                memset(S,0.0,sizeof(double)*64);

//                                for (int y = 0; y < 8; ++y) {
//                                  for (int x = 0; x < 8; ++x) {
//                                    for (int u = 0; u < 8; ++u) {
//                                      for (int v = 0; v < 8; ++v) {

//                                        double Cu=(u!=0)?1.0:1.0/sqrt(2.0);
//                                        double Cv=(v!=0)?1.0/sqrt(2.0):1.0;

//                                        signed iz = (signed)((short)aMatrix[v][u]);

//                                        double cos1=cos((2.0*(double)x+1)*u*M_PI/16.0);
//                                        double cos2=cos((2.0*(double)y+1)*v*M_PI/16.0);
//                                        S[y][x]+=Cu*Cv*((double)(iz))*cos1*cos2;
//                                      }
//                                    }

//                                    S[y][x]*=1/4;

//                                    cout << '\t' << S[y][x];
//                                  }
//                                }




//                                cout << endl << "AFTER DCT" << endl;
//                                for (int ix = 0; ix < 8; ++ix) {
//                                  for (int ax = 0; ax < 8; ++ax) {
//                                    cout << '\t' << S[ix][ax];
//                                  }
//                                  cout << endl;
//                                }
//                                cout << "FIN";

                              //  float aMatrix2[8][8];

//                                iDCT(aMatrix1,aMatrix2);

                                idct(aMatrix2,aMatrix1,N,N);

                                cout << endl << "AFTER DCT" << endl;
                                for (int ix = 0; ix < 8; ++ix) {
                                  for (int ax = 0; ax < 8; ++ax) {
                                    //cout << '\t' << aMatrix2[ix][ax];
                                    aMatrix[ix][ax]= (number) aMatrix2[ix][ax];
                                    cout << '\t' << aMatrix[ix][ax];
                                  }
                                  cout << endl;
                                }
                                cout << "FIN";

//                                for (nx++; nx  < cx; ++nx)
//                                {
//                                    dx= pPixels[nPixelCount-1][nx][0][0]= pPixels[nPixelCount-1][nx][0][0]+dx;

//                                    cout << "FixedMatrix#" << cx << endl;
//                                    for (int kx = 0; kx < 8; ++kx)
//                                    {

//                                        for (int vx = 0; vx < 8; ++vx) {
//                                            //
//                                          pPixels[nPixelCount-1][nx][kx][vx]*= paQuantTables[dct.m_puComponents[ix].m_bDQTId][kx][vx];
//                                          cout << " " << dec  << (signed) pPixels[nPixelCount-1][nx][ix][vx];
//                                        }

//                                        cout << endl;
//                                    }
//                                }


                                cout << "Matrix#" << cx << endl;
                                for (int ix = 0; ix < 8; ++ix) {

                                    for (int vx = 0; vx < 8; ++vx) {
                                        cout << " " << dec  << (signed) aMatrix[ix][vx];
                                    }

                                    cout << endl;
                                }
qDebug()<< "end";
                            }

//                            short unsigned int dx= pPixels[nPixelCount-1][nx][0][0];

//                            for (nx++; nx  < cx; ++nx)
//                            {
//                                dx= pPixels[nPixelCount-1][nx][0][0]= pPixels[nPixelCount-1][nx][0][0]+dx;

//                                cout << "FixedMatrix#" << cx << endl;
//                                for (int kx = 0; kx < 8; ++kx)
//                                {

//                                    for (int vx = 0; vx < 8; ++vx) {
//                                        //
//                                      pPixels[nPixelCount-1][nx][kx][vx]*= paQuantTables[dct.m_puComponents[ix].m_bDQTId][kx][vx];
//                                      cout << " " << dec  << (signed) pPixels[nPixelCount-1][nx][ix][vx];
//                                    }

//                                    cout << endl;
//                                }
//                            }


                        }


                    } while (!inImage.eof());

                    number* pImage=0;
                    number* pLast=pImage;
                    int nSize=0;

                    for (int ix = 0; ix < nPixelCount; ++ix) {

                      struct
                      {
                        number** m_pY;
                        number** m_pCb;
                        number** m_pCr;
                      }Conv;

                      Conv.m_pY=pPixels[ix][0];
                      Conv.m_pCb=pPixels[ix][4];
                      Conv.m_pCr=pPixels[ix][5];

                      ofstream stream;
                      stream.open("Pic.dat",ios::out| ios::binary);

                      char R,G,B;

                      for (int c = 0; c <dct.m_puComponents[0].m_H*dct.m_puComponents[0].m_V ; ++c) {

                        for (int i = 0; i < 8; ++i) {
                          for (int j = 0; j < 8; ++j) {
                            R=Conv.m_pY[i][j] + 1.402 *(Conv.m_pCr[i][j]-128);
                            G=Conv.m_pY[i][j] - 0.34414*(Conv.m_pCb[i][j]-128) - 0.71414*(Conv.m_pCr[i][j] - 128);
                            B=Conv.m_pY[i][j] + 1.772 * (Conv.m_pCb[i][j]-128);

                            nSize+=    sizeof(int)*3;



                            ///for (int ix = 0; ix < nSize; ++ix) {
                            ///
                            stream.put(G);
                            stream.put(B);
                             stream.put(R);



                            ///}


                            //pImage=(number*) realloc(pImage,nSize);
                           // if (pLast==0)pLast=pImage;

//                            pLast[0]= R;
//                            pLast[1]= G;
//                            pLast[2]= B;

                           // pLast+= sizeof(int)*3;
                          }
                        }





                      }

                      stream.close();


//                      ofstream stream;
//                      stream.open("Pic.dat",ios::out| ios::binary);

//                      for (int ix = 0; ix < nSize; ++ix) {
//                        stream.put(pImage[ix]);

//                      }

                    }


#endif


//                    for (int px = 0; px < nPixelCount; ++px) {
//                        for (int cx = 0; cx < nDataUnitCount; ++cx) {
//                            for (int ix = 0; ix < 8; ++ix) {
//                                for (int jx = 0; jx < 8; ++jx) {
//                                    if (cx>=0&&cx<=4) {
//                                        pPixels[px][cx][ix][jx]*= paQuantTables[dct.m_puComponents[0].m_bDQTId][ix][jx];
//                                    } else if (cx==5) {
//                                        pPixels[px][cx][ix][jx]*= paQuantTables[dct.m_puComponents[1].m_bDQTId][ix][jx];
//                                    }
//                                    else {
//                                        pPixels[px][cx][ix][jx]*= paQuantTables[dct.m_puComponents[2].m_bDQTId][ix][jx];
//                                    }


//                                }
//                            }

//                        }
//                    }
                }

                break;
            default:
                {
                    pbSegment=new byte[wSegmentSize];
                    inImage.read((char*)pbSegment,sizeof(byte)*(wSegmentSize-2));
                    delete [] pbSegment;
                }
            }

        }
        //cout << inImage;
    }

    return a.exec();
}

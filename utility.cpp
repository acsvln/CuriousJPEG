#include "utility.hpp"

std::experimental::optional<std::pair<unsigned, unsigned> > AddToZigZagMatrix(
          boost::numeric::ublas::matrix<uint16_t> matrix
        , std::pair<unsigned, unsigned> position
        , uint16_t value)
{
    return {};
    //#define Up true
    //#define Down false
    //#define setQuantTableItem(bT,aT,ix,iy,iT) ({aT[iy][ix]=bT;iT++;})
    //    typedef bool Direction;

    //    static Direction fDirection=Down;
    //    static int iT=0;
    //    static int ix=0,iy=0;
    //    static int state=0;
    //    switch (state) {
    //    case 0: setQuantTableItem(bT,aTable,ix,iy,iT);/* начало функции */
    //        //return true;
    //        while (iT<cnMatrixDimension-2)
    //        {
    //            if (fDirection==Down)
    //            {
    //                if (ix<cnMatrixSide-1)++ix;else iy++;
    //                for (ix,iy;ix>=0&&iy<cnMatrixSide;ix--,iy++)
    //                {

    //                    state = 1;/* возвратиться к "case 1" */
    //                    return false;
    //                case 1: setQuantTableItem(bT,aTable,ix,iy,iT);/* продолжить выполнения после точки возврата */;/* возвратиться к "case 1" */
    //                }
    //                ix++;
    //                iy--;
    //                fDirection=Up;
    //            }
    //            else
    //            {
    //                if (iy<cnMatrixSide-1)++iy;else ++ix;
    //                for (ix,iy;ix<cnMatrixSide&&iy>=0;ix++,iy--)
    //                {
    //                    state = 2;/* возвратиться к "case 1" */
    //                    return false;
    //                case 2: setQuantTableItem(bT,aTable,ix,iy,iT);
    //                }
    //                ix--;
    //                iy++;
    //                fDirection=Down;
    //            }
    //        }
    //        ix++;
    //        state=3;
    //        return false;
    //    case 3: setQuantTableItem(bT,aTable,ix,iy,iT);
    //    }
    //    fDirection=Down;
    //    iT=0;
    //    ix=0;
    //    iy=0;
    //    state=0;
    //    return true;
    //#undef Up
    //#undef Down
    //#undef setQuantTableItem
}

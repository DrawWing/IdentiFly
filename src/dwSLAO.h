#ifndef DWSLAO_H
#define DWSLAO_H

#include "dwRclList.h"

class dwSLAO
{
public:
    dwSLAO(dwRclList &inLmkList, dwRclList &inOtlList);
    dwRclList alignOnce();
    dwRclList alignIterate();

private:
    void alignToOutline(const dwRCoordList &inRef, dwRCoordList &inLmk, dwRCoordList &inOtl);

    dwRclList lmkList;
    dwRclList otlList;

};

#endif // DWSLAO_H

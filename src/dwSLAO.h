#ifndef DWSLAO_H
#define DWSLAO_H

#include "dwRclList.h"

class dwSLAO
{
public:
    dwSLAO(dwRclList &inLmkList, dwRclList &inOtlList);
    dwRclList align();
    void align(const dwRCoordList &inRef, dwRCoordList &inLmk, dwRCoordList &inOtl);
    // dwRclList getLandmarks() const;

private:
    dwRclList lmkList;
    dwRclList otlList;

};

#endif // DWSLAO_H

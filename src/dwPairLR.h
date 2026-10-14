#ifndef DWPAIRLR_H
#define DWPAIRLR_H

#include <vector>
#include <QString>
#include <QDir>
#include "dwRclList.h"

class dwPairLR
{
public:
    dwPairLR();
    // std::vector< QString > outliers(QDir inDir);
    std::vector< QString > outliers(dwRclList rcl);
    QString findPairLR(QString inStr, QStringList inStringList, int *LRindex, int *listIndex);
    QStringList unPaird(QDir inDir);
private:

};

#endif // DWPAIRLR_H

#ifndef DWPAIRLR_H
#define DWPAIRLR_H

#include <vector>
#include <QString>
#include <QDir>

class dwPairLR
{
public:
    dwPairLR();
    std::vector< QString > outliers(QDir inDir);
    QString findPairLR(QString inStr, QStringList inStringList, int *LRindex, int *listIndex);
    QStringList unPaird(QDir inDir);
private:

};

#endif // DWPAIRLR_H

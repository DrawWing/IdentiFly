#ifndef DWFILERENAME_H
#define DWFILERENAME_H

#include <QString>
#include <QDir>

class dwFileRename
{
public:
    dwFileRename();
    void moveString(QDir inDir, unsigned size, unsigned from, unsigned to);
    void removePart(QDir inDir, unsigned size, unsigned from);
    void replaceChar(QDir inDir, QString from, QString to);
    void copyList2trash(QDir inDir, QStringList inStringList);
    void copy2trash(QFileInfo inFileInfo);
    void moveList2trash(QDir inDir, QStringList inStringList);
    void move2trash(QFileInfo inFileInfo);
    QString rename(QDir inDir);
    void addSufix(QDir inDir);
    void addDirPrefix(QDir inDir);
    void addPrefix(QDir inDir, QString prefix);
    QStringList readStrLst(QString inFileName);
private:
    void addPrefixRec(QDir inDir, QString prefix);
};

#endif // DWFILERENAME_H

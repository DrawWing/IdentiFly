#include <QTextStream>

#include "dwFileRename.h"

dwFileRename::dwFileRename()
{
}

// position is measured after 1 means after first
void dwFileRename::moveString(QDir inDir, unsigned size, unsigned from, unsigned to)
{
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0) return;

    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        QString nameBefore = localFileInfo.baseName();
        QString nameAfter = nameBefore;
        QString toMove = nameBefore.mid(from, size);
        nameAfter.insert(to, toMove);
        if(to < from)
            nameAfter.remove(from+size, size);
        else
            nameAfter.remove(from, size);
        QFile theFile(localFileInfo.filePath());
        QString fileName = localFileInfo.absoluteFilePath();
        fileName.replace(nameBefore,nameAfter); //string może być w scieżce
        theFile.rename(fileName);
        theFile.close();
    }
    return;
}

// position is measured after, 1 means after first
void dwFileRename::removePart(QDir inDir, unsigned size, unsigned from)
{
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0) return;

    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);
        QString nameBefore = localFileInfo.baseName();
        QString nameAfter = nameBefore;
        nameAfter.remove(from, size);

        QFile theFile(localFileInfo.filePath());
        QString fileName = localFileInfo.absoluteFilePath();
        fileName.replace(nameBefore,nameAfter); //problem jeśli string jest w scieżce
        theFile.rename(fileName);
        theFile.close();
    }
    return;
}

// position is measured after, 1 means after first
void dwFileRename::replaceChar(QDir inDir, QString from, QString to)
{
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0) return;

    for (int i = 0; i < localFileInfoList.size(); ++i) {
        QFileInfo localFileInfo = localFileInfoList.at(i);

        QFile theFile(localFileInfo.filePath());
        QString fileName = localFileInfo.absoluteFilePath();
        fileName.replace(from, to); // replaces also in path
        theFile.rename(fileName);
        theFile.close();
    }
    return;
}

void dwFileRename::copyList2trash(QDir inDir, QStringList inStringList)
{
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0) return;

    for(int i = 0; i < inStringList.size(); ++i)
    {
        QString inString = inStringList[i];

        for (int i = 1; i < localFileInfoList.size(); ++i) {
            QFileInfo currFileInfo = localFileInfoList.at(i);
            QString fileName = currFileInfo.fileName();

            if(fileName == inString)
                move2trash(currFileInfo);
        }
    }
}

void dwFileRename::copy2trash(QFileInfo inFileInfo)
{
    QString trashPath = inFileInfo.absolutePath() + "/trash";
    QDir trashDir(trashPath);
    if(!trashDir.exists())
    {
        QDir theDir(inFileInfo.absolutePath());
        theDir.mkdir("trash");
    }

    QString newName = inFileInfo.absolutePath() + "/trash/" + inFileInfo.fileName();

    bool ok = QFile::copy(inFileInfo.absoluteFilePath(), newName);
    if(!ok) return; //todo
    return;
}

void dwFileRename::moveList2trash(QDir inDir, QStringList inStringList)
{
    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*"), QDir::Files, QDir::Name );
    if(localFileInfoList.size() == 0) return;

    for(int i = 0; i < inStringList.size(); ++i)
    {
        QString inString = inStringList[i];

        for (int i = 1; i < localFileInfoList.size(); ++i) {
            QFileInfo currFileInfo = localFileInfoList.at(i);
            QString fileName = currFileInfo.fileName();

            if(fileName == inString)
                move2trash(currFileInfo);
        }
    }
}

void dwFileRename::move2trash(QFileInfo inFileInfo)
{
    QString trashPath = inFileInfo.absolutePath() + "/trash";
    QDir trashDir(trashPath);
    if(!trashDir.exists())
    {
        QDir theDir(inFileInfo.absolutePath());
        theDir.mkdir("trash");
    }

    QString newName = inFileInfo.absolutePath() + "/trash/" + inFileInfo.fileName();

    bool ok = QFile::rename(inFileInfo.absoluteFilePath(), newName);
    if(!ok) return; //todo
    return;
}

QString dwFileRename::rename(QDir inDir)
{
    QString inFileName = inDir.absolutePath();
    inFileName += "/rename.txt";
    QFile in_file(inFileName);
    if (!in_file.open(QIODevice::ReadOnly | QFile::Text)) {
        return  QString("Cannot open file %1:\n%2.").arg(inFileName, in_file.errorString());
    }

    QTextStream in(&in_file);
    in.setCodec("UTF-8");
    // qt6   in.setEncoding(QStringConverter::Utf8);
    QStringList oldLst;
    QStringList newLst;
    while (!in.atEnd()) {
        QString ln = in.readLine();
        QStringList lnList = ln.split('\t');
        if(lnList.size() < 2)
            continue;
        oldLst.push_back(lnList[0].trimmed());
        newLst.push_back(lnList[1].trimmed());
    }

    int duplicates = oldLst.removeDuplicates();
    if(duplicates > 0)
        return QString("Number of duplicates in old list: %1").arg(duplicates);

    duplicates = newLst.removeDuplicates();
    if(duplicates > 0)
        return QString("Number of duplicates in new list: %1").arg(duplicates);

    for(int i = 0; i < oldLst.size(); ++i)
    {
        QString oldName = oldLst[i];
        QString newName = newLst[i];

        if(oldName == newName)
            continue;
        bool ok = inDir.rename(oldName, newName); // rename file or directory
        if(!ok)
        {
            QString error = QString("Error renaming files: %1, %2").arg(oldName, newName);
            return error;
        }
    }
    return QString();
}

// add dash after directory name for addDirPrefix
//  | QDir::NoDotAndDotDot prevents recurvive loops
void dwFileRename::addSufix(QDir inDir)
{
    //    QString sufix = "-";

    QFileInfoList localFileInfoList = inDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );
    if(localFileInfoList.size() == 0) return;

    for(int i = 0; i < localFileInfoList.size(); ++i)
    {
        QFileInfo theFileInfo = localFileInfoList.at(i);
        QDir nextDir(theFileInfo.absoluteFilePath());
        addSufix(nextDir);
        QString oldName = nextDir.path();
        QString newName = nextDir.path()+"-";
        nextDir.rename(oldName, newName);
    }
}

void dwFileRename::addDirPrefix(QDir inDir)
{
    QString prefix = "";

    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*"), QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );
    if(localFileInfoList.size() == 0) return;

    for(int i = 0; i < localFileInfoList.size(); ++i)
    {
        QFileInfo theFileInfo = localFileInfoList.at(i);
        QDir nextDir(theFileInfo.absoluteFilePath());
        addPrefixRec(nextDir, prefix);
    }
}

void dwFileRename::addPrefixRec(QDir inDir, QString prefix)
{
    prefix += inDir.dirName() + "-";
    addPrefix(inDir, prefix);

    QFileInfoList localFileInfoList = inDir.entryInfoList(QStringList("*"), QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );
    if(localFileInfoList.size() == 0) return;

    for(int i = 0; i < localFileInfoList.size(); ++i)
    {
        QFileInfo theFileInfo = localFileInfoList.at(i);
        QDir nextDir(theFileInfo.absoluteFilePath());
        addPrefixRec(nextDir, prefix);
    }
}

void dwFileRename::addPrefix(QDir inDir, QString prefix)
{
    QStringList extList = (QStringList() << "*.png" << "*.jpg" << "*.bmp");
    QFileInfoList localFileInfoList = inDir.entryInfoList(extList, QDir::Files, QDir::Name );

    if(localFileInfoList.size() == 0) return;

    for(int i = 0; i < localFileInfoList.size(); ++i)
    {
        QFileInfo theFileInfo = localFileInfoList.at(i);
        QFile theFile(theFileInfo.absoluteFilePath());
        QString fileName = theFileInfo.absolutePath() + "/" + prefix + theFileInfo.fileName();
        theFile.rename(fileName);
        theFile.close();
    }
}

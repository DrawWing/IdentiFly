/*
    Copyright (C) 2026 Adam Tofilski

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

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
private:
    void addPrefixRec(QDir inDir, QString prefix);
};

#endif // DWFILERENAME_H
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

#ifndef DWOPTIONS_H
#define DWOPTIONS_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QSettings>
#include <QTabWidget>

#include "mainwindow.h"

class langTab : public QWidget
{
    Q_OBJECT

public:
    explicit langTab(QWidget *parent = 0);
    QString outLang;

private slots:
    void slotLanguageChanged(const QString & lang);

private:
    void createLanguageMenu(void);
};

class zoomTab : public QWidget
{
    Q_OBJECT

public:
    explicit zoomTab(MainWindow *parent);
    int outLoadZoom;
    int outZoom3;
    int outZoom4;

private slots:
    void radioClickked(int button);
    void setZoom3();
    void setZoom4();

private:
    MainWindow *mainWindow;
};

class edgeTab : public QWidget
{
    Q_OBJECT

public:
    explicit edgeTab(QWidget *parent = 0);
};

class dwOptions : public QDialog
{
    Q_OBJECT
public:
    dwOptions(MainWindow *parent);

private slots:
    void slotAccepted();

private:
    langTab *langtab;
    zoomTab *zoomtab;
    QTabWidget *tabWidget;
    QDialogButtonBox *buttonBox;
    MainWindow *mainWindow;
    QSettings settings;
};

#endif // DWOPTIONS_H
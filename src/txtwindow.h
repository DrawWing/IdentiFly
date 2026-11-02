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

#ifndef TXTWINDOW_H
#define TXTWINDOW_H

#include <QMainWindow>

class QAction;
class QMenu;
class QTextEdit;

class TxtWindow: public QMainWindow
{
    Q_OBJECT

public:
    TxtWindow(QWidget *parent = 0);
    TxtWindow(const QString & fileName, QWidget *parent = 0);
	void appendTxt(QString &);
    void setPlainTxt(QString & txt);
    void setCurrentFile(const QString &fileName);
    void savePdf(QString & fileName);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void documentWasModified();
    void printPdf();

private:
    void init();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    TxtWindow *findMainWindow(const QString &fileName);

    QTextEdit *textEdit;
    QString curFile;
    bool isUntitled;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *closeAct;
    //QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
//    QAction *aboutAct;
//    QAction *aboutQtAct;
};

#endif //TXTWINDOW_H
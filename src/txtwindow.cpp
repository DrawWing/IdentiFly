#include <QtWidgets>
#include <QPrinter>

#include "txtwindow.h"

TxtWindow::TxtWindow(QWidget *parent)
: QMainWindow(parent)
{
    init();
}

TxtWindow::TxtWindow(const QString & txt, QWidget *parent)
: QMainWindow(parent)
{
//    setWindowTitle("[*]"); //otherwise there are errors at output
    init();

    QString htmlTxt = txt;
    htmlTxt.replace(QString("\n"), QString("<br>"));
    textEdit->setHtml(htmlTxt);

//    setWindowModified(false);
}

void TxtWindow::appendTxt(QString & txt)
{
    textEdit->append(txt);
}

void TxtWindow::setPlainTxt(QString & txt)
{
    textEdit->setPlainText(txt);
}

void TxtWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void TxtWindow::newFile()
{
    TxtWindow *other = new TxtWindow;
    other->move(x() + 40, y() + 40);
    other->show();
}

void TxtWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        TxtWindow *existing = findMainWindow(fileName);
        if (existing) {
            existing->show();
            existing->raise();
            existing->activateWindow();
            return;
        }

        if (isUntitled && textEdit->document()->isEmpty()
                && !isWindowModified()) {
            loadFile(fileName);
        } else {
            TxtWindow *other = new TxtWindow(fileName);
            if (other->isUntitled) {
                delete other;
                return;
            }
            other->move(x() + 40, y() + 40);
            other->show();
        }
    }
}

bool TxtWindow::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool TxtWindow::saveAs()
{
    QString selectedFilter = tr("HTML (*.htm *.html)");
    QString fileName = QFileDialog::getSaveFileName(
                this,
                tr("Save as"),
                curFile,
                "HTML (*.htm *.html);;PDF (*.pdf)",
                &selectedFilter
                );


    if (fileName.isEmpty())
        return false;


    if(selectedFilter == "HTML (*.htm *.html)")
        return saveFile(fileName);
    else if(selectedFilter == "PDF (*.pdf)")
    {
        savePdf(fileName);
        return true; // add error checking
    }
    return false;
}

void TxtWindow::documentWasModified()
{
    setWindowModified(true);
}

void TxtWindow::init()
{
    setAttribute(Qt::WA_DeleteOnClose);

    isUntitled = true;

    textEdit = new QTextEdit;
    setCentralWidget(textEdit);
    textEdit->setFontFamily("Courier");

    createActions();
    createMenus();
    //createToolBars();
    createStatusBar();

    readSettings();
    setCurrentFile("");

    connect(textEdit->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
}

void TxtWindow::createActions()
{
	saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &as"), this);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    closeAct = new QAction(tr("&Close"), this);
    closeAct->setShortcut(tr("Ctrl+W"));
    closeAct->setStatusTip(tr("Close this window"));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(close()));

	cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcut(tr("Ctrl+X"));
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));

	copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcut(tr("Ctrl+C"));
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));

	pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcut(tr("Ctrl+V"));
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));

    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
}

void TxtWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAct);
    //fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
}

void TxtWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(saveAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
}

void TxtWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void TxtWindow::readSettings()
{
    QString companyName = QGuiApplication::organizationName();
    QString appName = QGuiApplication::applicationDisplayName() + " - text window";
    QSettings settings(companyName, appName);
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void TxtWindow::writeSettings()
{
    QString companyName = QGuiApplication::organizationName();
    QString appName = QGuiApplication::applicationDisplayName() + " - text window";
    QSettings settings(companyName, appName);
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

bool TxtWindow::maybeSave()
{
    if (textEdit->document()->isModified()) {
        int ret = QMessageBox::warning(this, tr("Warning"),
                     tr("Do you want to save the document?"),
                     QMessageBox::Yes | QMessageBox::Default,
                     QMessageBox::No,
                     QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Yes)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void TxtWindow::loadFile(const QString &fileName)
{

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "",
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName, file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    textEdit->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool TxtWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "",
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName, file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
//    out << textEdit->toPlainText();
    out << textEdit->toHtml();    //.toUtf8();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void TxtWindow::setCurrentFile(const QString &fileName)
{
    static int sequenceNumber = 1;

    isUntitled = fileName.isEmpty();
    if (isUntitled) {
        curFile = tr("results%1.txt").arg(sequenceNumber++);
    } else {
        curFile = fileName;
        //curFile = QFileInfo(fileName).canonicalFilePath();
    }

    textEdit->document()->setModified(false);
    setWindowModified(false);

    setWindowTitle(QString("%1[*]").arg(strippedName(curFile)));
}

QString TxtWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

TxtWindow *TxtWindow::findMainWindow(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QWidget *widget, qApp->topLevelWidgets()) {
        TxtWindow *mainWin = qobject_cast<TxtWindow *>(widget);
        if (mainWin && mainWin->curFile == canonicalFilePath)
            return mainWin;
    }
    return 0;
}

void TxtWindow::savePdf(QString & fileName)
{
#ifndef QT_NO_PRINTER
    if (QFileInfo(fileName).suffix().isEmpty())
        fileName.append(".pdf");
    QPrinter printer(QPrinter::HighResolution);
//    printer.setPageSize(QPageSize::A4);
//    printer.setPageSize(QPagedPaintDevice::A4);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    //        textEdit->document()->setPageSize(printer.pageRect().size()); // to remove page numbers but it does not work
    textEdit->document()->print(&printer);
#endif
}

void TxtWindow::printPdf()
{
#ifndef QT_NO_PRINTER
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF",
                                                    QString(), "*.pdf");
    if (!fileName.isEmpty())
    {
        savePdf(fileName);
    }
#endif
}


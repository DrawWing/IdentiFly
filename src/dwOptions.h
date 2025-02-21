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

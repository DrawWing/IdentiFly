#include <QtWidgets>

#include "dwOptions.h"

dwOptions::dwOptions(MainWindow *parent)
    : QDialog(parent)
{
    mainWindow = parent;

    tabWidget = new QTabWidget;
    zoomtab = new zoomTab(parent);
    langtab = new langTab(parent);
    tabWidget->addTab(zoomtab, tr("Zoom"));
//    tabWidget->addTab(langtab, tr("Language"));
//    tabWidget->addTab(new edgeTab(), tr("Edge"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(slotAccepted()));
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Preferences"));
}

void dwOptions::slotAccepted()
{
//    QString companyName = QGuiApplication::organizationName();
//    QString appName = QGuiApplication::applicationDisplayName();
//    QSettings settings(companyName, appName);

//    QString outLang = langtab->outLang;
//    if(!outLang.isEmpty())
//    {
//        QString savedLocale = settings.value("locale").toString();
//        if(savedLocale != outLang)
//        {
//            settings.setValue("locale", outLang);
//        }
//    }
    if(zoomtab->outLoadZoom != -1)
    {
        settings.setValue("loadZoom", zoomtab->outLoadZoom);
    }
    if(zoomtab->outZoom3 != -1)
    {
        settings.setValue("zoom3", zoomtab->outZoom3);
    }
    if(zoomtab->outZoom4 != -1)
    {
        settings.setValue("zoom4", zoomtab->outZoom4);
    }
    mainWindow->updateOptions();
}

langTab::langTab(QWidget *parent)
    : QWidget(parent)
{
    outLang = "";

    QComboBox *langComboBox = new QComboBox();

    // format systems language
    QString defaultLocale = QLocale::system().name(); // e.g. "de_DE"
    defaultLocale.truncate(defaultLocale.lastIndexOf('_')); // e.g. "de"

    QString m_langPath = QApplication::applicationDirPath();
    m_langPath.append("/languages");
    QDir dir(m_langPath);
    QStringList fileNames = dir.entryList(QStringList("identifly_*.qm"));

    langComboBox->addItem("Detect automaticaly", "auto");
    QIcon ico(QString("%1/%2.png").arg(m_langPath).arg("en"));
    langComboBox->addItem(ico, "English", "en");

    for (int i = 0; i < fileNames.size(); ++i) {
        // get locale extracted by filename
        QString locale;
        locale = fileNames[i]; // "identifly_de.qm"
        locale.truncate(locale.lastIndexOf('.')); // "identifly_de"
        locale.remove(0, locale.indexOf('_') + 1); // "de"

        QString lang = QLocale::languageToString(QLocale(locale).language());
        QIcon ico(QString("%1/%2.png").arg(m_langPath).arg(locale));

        langComboBox->addItem(ico, lang, locale);
    }

    connect(langComboBox, SIGNAL (currentIndexChanged(const QString &)),
            this, SLOT (slotLanguageChanged(const QString &)));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(langComboBox);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

// Called every time, when a menu entry of the language menu is called
void langTab::slotLanguageChanged(const QString & lang)
{
    outLang = lang;
}

zoomTab::zoomTab(MainWindow *parent)
    : QWidget(parent)
{
    mainWindow = parent;
    QString companyName = QGuiApplication::organizationName();
    QString appName = QGuiApplication::applicationDisplayName();
    QSettings settings(companyName, appName);

    outLoadZoom = -1;
    outZoom3 = -1;
    outZoom4 = -1;

    QGroupBox *groupBox = new QGroupBox(tr("Zoom at file opening"));
    QButtonGroup *buttonGroup = new QButtonGroup();

    QRadioButton *radio0 = new QRadioButton(tr("Zoom to &fit window"));
    QRadioButton *radio1 = new QRadioButton(tr("Zoom to &actual size"));
    QRadioButton *radio2 = new QRadioButton(tr("Zoom &landmarks"));
    QRadioButton *radio3 = new QRadioButton(tr("Zoom &3"));
    QRadioButton *radio4 = new QRadioButton(tr("Zoom &4"));

    int loadZoomOpt = settings.value("loadZoom").toInt();
    if(loadZoomOpt == 1)
        radio1->setChecked(true);
    else if(loadZoomOpt == 2)
        radio2->setChecked(true);
    else if(loadZoomOpt == 3)
        radio3->setChecked(true);
    else if(loadZoomOpt == 4)
        radio4->setChecked(true);
    else
        radio0->setChecked(true);

    buttonGroup->addButton(radio0, 0);
    buttonGroup->addButton(radio1, 1);
    buttonGroup->addButton(radio2, 2);
    buttonGroup->addButton(radio3, 3);
    buttonGroup->addButton(radio4, 4);
    connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(radioClickked(int)));

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(radio0);
    vbox->addWidget(radio1);
    vbox->addWidget(radio2);
    vbox->addWidget(radio3);
    vbox->addWidget(radio4);
    vbox->addStretch(1);
    groupBox->setLayout(vbox);


    QLabel *ctrl3Label = new QLabel(tr("Set zoom 3"));
    QPushButton *pushButton3 = new QPushButton(tr("&Use current zoom"));
    connect(pushButton3, SIGNAL(clicked()),this,SLOT(setZoom3()));

    QHBoxLayout *Hbox3 = new QHBoxLayout;
    Hbox3->addWidget(ctrl3Label);
    Hbox3->addWidget(pushButton3);
    Hbox3->addStretch(1);

    QLabel *ctrl4Label = new QLabel(tr("Set zoom 4"));
    QPushButton *pushButton4 = new QPushButton(tr("&Use current zoom"));
    connect(pushButton4, SIGNAL(clicked()),this,SLOT(setZoom4()));

    QHBoxLayout *Hbox4 = new QHBoxLayout;
    Hbox4->addWidget(ctrl4Label);
    Hbox4->addWidget(pushButton4);
    Hbox4->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(groupBox);
    mainLayout->addLayout(Hbox3);
    mainLayout->addLayout(Hbox4);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

void zoomTab::radioClickked(int button)
{
    outLoadZoom = button;
}

void zoomTab::setZoom3()
{
    outZoom3 = mainWindow->getZoom();
}

void zoomTab::setZoom4()
{
    outZoom4 = mainWindow->getZoom();
}

edgeTab::edgeTab(QWidget *parent)
    : QWidget(parent)
{
//    QLabel *fileNameLabel = new QLabel(tr("File Name:"));
//    QLabel *fileValueLabel = new QLabel("xxx");

//    QLabel *pathLabel = new QLabel(tr("Path:"));
//    QLabel *pathValueLabel = new QLabel("xxx");


//    QVBoxLayout *mainLayout = new QVBoxLayout;
//    mainLayout->addWidget(fileNameLabel);
//    mainLayout->addWidget(fileValueLabel);
//    mainLayout->addWidget(pathLabel);
//    mainLayout->addWidget(pathValueLabel);
//    mainLayout->addStretch(1);
//    setLayout(mainLayout);
}

// we create the menu entries dynamically, dependent on the existing translations.
void langTab::createLanguageMenu(void)
{
    QActionGroup* langGroup = new QActionGroup(this);
    langGroup->setExclusive(true);

    connect(langGroup, SIGNAL (triggered(QAction *)), this, SLOT (slotLanguageChanged(QAction *)));

    // format systems language
    QString defaultLocale = QLocale::system().name(); // e.g. "de_DE"
    defaultLocale.truncate(defaultLocale.lastIndexOf('_')); // e.g. "de"

    QString m_langPath = QApplication::applicationDirPath();
    m_langPath.append("/languages");
    QDir dir(m_langPath);
    QStringList fileNames = dir.entryList(QStringList("identifly_*.qm"));

    for (int i = 0; i < fileNames.size(); ++i) {
        // get locale extracted by filename
        QString locale;
        locale = fileNames[i]; // "identifly_de.qm"
        locale.truncate(locale.lastIndexOf('.')); // "identifly_de"
        locale.remove(0, locale.indexOf('_') + 1); // "de"

        QString lang = QLocale::languageToString(QLocale(locale).language());
        QIcon ico(QString("%1/%2.png").arg(m_langPath).arg(locale));

        QAction *action = new QAction(ico, lang, this);
        action->setCheckable(true);
        action->setData(locale);

//        ui.menuLanguage->addAction(action);
        addAction(action);
        langGroup->addAction(action);

        // set default translators and language checked
        if (defaultLocale == locale)
        {
            action->setChecked(true);
        }
    }
}

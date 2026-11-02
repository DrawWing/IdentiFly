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

#include <QtWidgets>

#include "thresholdDialog.h"

thresholdDialog::thresholdDialog(int thd, QWidget *parent)
        : QDialog(parent)
{
    canceled = true;

    label = new QLabel(
		QString(
//			tr(
			"<p><b>Choose a threshold for conversion of image to black and white.</b></p>"
			"<p>Default value %1.</p>"
//			)
		).arg(thd)
		, this);

	thdSpin = new QSpinBox;
	thdSpin->setRange(0, 255);
	thdSpin->setSingleStep(1);
	thdSpin->setValue(thd);
	//thdSpin->setSpecialValueText("Auto");
    connect( thdSpin, SIGNAL( valueChanged(int) ), parent, SLOT( threshold(int) ) );

	okButton = new QPushButton("OK");
	okButton->setDefault(true);
	connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(okButton, SIGNAL(clicked()), this, SLOT(writeSettings()));

	QVBoxLayout * layout = new QVBoxLayout;
	layout->addWidget(label);
	layout->addWidget(thdSpin);
	layout->addWidget(okButton);
	layout->addStretch(1);
	setLayout(layout);

	readSettings();
}

thresholdDialog::~thresholdDialog(void)
{
}

void thresholdDialog::readSettings()
{
    QSettings settings("DrawWing", "DrawWing - thdDialog");
    QRect rect = settings.value("geometry", QRect(200, 200, 400, 400)).toRect();
    move(rect.topLeft());
}

void thresholdDialog::writeSettings()
{
    canceled = false; // happens only if OK was clicked

    QSettings settings("DrawWing", "DrawWing - thdDialog");
    settings.setValue("geometry", frameGeometry());

}

bool thresholdDialog::wasCanceled()
{
    return canceled;
}

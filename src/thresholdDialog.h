#ifndef THDDIALOG_H
#define THDDIALOG_H

#include <QDialog>

class QLabel;
class QSpinBox;
class QPushButton;

class thresholdDialog : public QDialog
{
	Q_OBJECT

public:
	thresholdDialog(int, QWidget *parent = 0);
    bool wasCanceled();

public:
	~thresholdDialog(void);
private:
	QLabel * label;
	QSpinBox * thdSpin;
	QPushButton * okButton;
	void readSettings();
    bool canceled;

private slots:
	void writeSettings();
};

#endif //THDDIALOG_H

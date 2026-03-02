#ifndef CALIBRATIONFRAME_H
#define CALIBRATIONFRAME_H

#include <QFrame>
class QLabel;
namespace Ui {
class CalibrationFrame;
}

class CalibrationFrame : public QFrame
{
    Q_OBJECT

public:
    explicit CalibrationFrame(QWidget *parent = nullptr);
    ~CalibrationFrame();
public:
	void setCurrentLabelLocation(double x, double y);
	void setCalibrationTypeText(QString text);
	void setLRTlitText(QString text);//左右倾文本
	void setFBTlitText(QString text);//前后倾向文本
	void setNoticeText(QString text);
	void initHeightLabels();
	void initStepLabels();
	void setCurrentStep(int index);
	void setHeightOK(int index);
private:
	void setTargetLabelLocation(int step);
private:
    Ui::CalibrationFrame *ui;
	const double m_serviceCaliBoardWidth = 700.0;
	const double m_serviceCaliBoardHeight = 700.0;
	QLabel *m_heightLabelList[9];
	QLabel *m_stepLabelList[6];
};

#endif // CALIBRATIONFRAME_H

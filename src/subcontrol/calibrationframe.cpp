#include "calibrationframe.h"
#include "ui_calibrationframe.h"

//#define SET_HEIGHT_LABEL_STATE(index,value) ui->heightLabel_##index->setProperty("state",value);
#define ADD_HEIGHT_LABEL(arrayIndex,labelIndex) m_heightLabelList[arrayIndex] = ui->heightLabel_##labelIndex;
#define ADD_STEP_LABEL(arrayIndex,labelIndex) m_stepLabelList[arrayIndex] = ui->stepLabel_##labelIndex;
#include "qstyle.h"
auto repolish = [](QWidget* w) {
	w->style()->unpolish(w); 
	w->style()->polish(w);
};
CalibrationFrame::CalibrationFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CalibrationFrame)
{
    ui->setupUi(this);

	ADD_HEIGHT_LABEL(0, 1)
	ADD_HEIGHT_LABEL(1, 2)
	ADD_HEIGHT_LABEL(2, 3)
	ADD_HEIGHT_LABEL(3, 4)
	ADD_HEIGHT_LABEL(4, 5)
	ADD_HEIGHT_LABEL(5, 6)
	ADD_HEIGHT_LABEL(6, 7)
	ADD_HEIGHT_LABEL(7, 8)
	ADD_HEIGHT_LABEL(8, 9)

	ADD_STEP_LABEL(0,1)
	ADD_STEP_LABEL(1, 2)
	ADD_STEP_LABEL(2, 3)
	ADD_STEP_LABEL(3, 4)
	ADD_STEP_LABEL(4, 5)
	ADD_STEP_LABEL(5, 6)
	
}

CalibrationFrame::~CalibrationFrame()
{
    delete ui;
}

void CalibrationFrame::setCurrentLabelLocation(double x, double y)
{
	double currentBoardWidth = ui->boradLabel->width();
	double currentBoardHeight = ui->boradLabel->height();
	double currentCenterX = x / m_serviceCaliBoardWidth * currentBoardWidth + currentBoardWidth / 2.0;
	double currentCenterY = -(y / m_serviceCaliBoardHeight * currentBoardHeight )+ currentBoardHeight / 2.0;
	double currentX = currentCenterX - ui->currentLabel->width() / 2.0;
	double currentY = currentCenterY - ui->currentLabel->height() / 2.0;
	ui->currentLabel->move(currentX, currentY);
}

void CalibrationFrame::setCalibrationTypeText(QString text)
{
	ui->calibTypeLabel->setText(text);
}

void CalibrationFrame::setLRTlitText(QString text)
{
	ui->lrTiltLabel->setText(text);
}

void CalibrationFrame::setFBTlitText(QString text)
{
	ui->fbTiltLabel->setText(text);
}

void CalibrationFrame::setNoticeText(QString text)
{
	ui->noticeLabel->setText(text);
}

void CalibrationFrame::initHeightLabels()
{
	for (int i = 0; i < 9; ++i)
	{
		m_heightLabelList[i]->setProperty("state", "noOK");
		repolish(m_heightLabelList[i]);
	}
}

void CalibrationFrame::initStepLabels()
{
	for (int i = 0; i < 6; ++i)
	{
		m_stepLabelList[i]->setProperty("state", "black");
		repolish(m_heightLabelList[i]);
	}
}

void CalibrationFrame::setCurrentStep(int index)
{
	for (int i = 0; i <= index; ++i)
	{
		m_stepLabelList[i]->setProperty("state", "blue");
		repolish(m_stepLabelList[i]);
	}
	setTargetLabelLocation(index);
}

void CalibrationFrame::setHeightOK(int index)
{
	m_heightLabelList[index]->setProperty("state", "ok");
	repolish(m_heightLabelList[index]);
}

void CalibrationFrame::setTargetLabelLocation(int step)
{
	int x = (ui->boradLabel->width() - ui->targetLabel->width()) / 2;
	if (step > 0)
	{
		ui->targetLabel->move(x, x);
	}
	else {
		int offset = (175.0 / m_serviceCaliBoardWidth)*double(ui->boradLabel->width());
		ui->targetLabel->move(x-offset, x);
	}
}

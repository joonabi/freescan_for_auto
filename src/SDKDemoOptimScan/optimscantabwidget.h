#ifndef OPTIMSCANTABWIDGET_H
#define OPTIMSCANTABWIDGET_H

#include <QWidget>
#include "sn3dsdkdemoapp.h"

namespace Ui {
class OptimScanTabWidget;
}

class OptimScanTabWidget : public QWidget
{
    Q_OBJECT


public:
    explicit OptimScanTabWidget(QWidget *parent = nullptr);
    ~OptimScanTabWidget();

	void getScanSettingData(LPScanPars& param);

private slots:

    void on_alignMode_currentTextChanged(const QString &alignModText);

    void on_turntablemodeRadio_toggled(bool checked);

    void on_openGmarkerBtn_pressed();

private:
    void initUI();

Q_SIGNALS:
    void importFrameworkMakers(QString);

private:
    Ui::OptimScanTabWidget *ui;
};

#endif // OPTIMSCANTABWIDGET_H

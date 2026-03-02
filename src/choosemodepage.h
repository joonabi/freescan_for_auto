#ifndef CHOOSEMODEPAGE_H
#define CHOOSEMODEPAGE_H

#include <QWidget>
#include "windows.h"
#include "sn3dsdkcorepub.h"
namespace Ui {
class ChooseModePage;
}

class ChooseModePage : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int ScanMode READ scanmode WRITE setScanMode NOTIFY scanModeChanged)
public:
    explicit ChooseModePage(QWidget *parent = nullptr);
    ~ChooseModePage();
    int scanmode();
    void setScanMode(int iScanMode);
Q_SIGNALS:
    void scanModeChanged(int iScanMode);
private:
    Ui::ChooseModePage *ui;
    int m_iScanMode = SNSDK_SCANMODE_LASER;
};

#endif // CHOOSEMODEPAGE_H

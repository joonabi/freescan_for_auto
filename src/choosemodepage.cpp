#include "choosemodepage.h"
#include "ui_choosemodepage.h"
#include "qabstractbutton.h"

ChooseModePage::ChooseModePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChooseModePage)
{
    ui->setupUi(this);
    connect(ui->btnLaser, &QAbstractButton::clicked, [this]() {
        setScanMode(SNSDK_SCANMODE_LASER);
        this->close();
    });
    connect(ui->btnIR, &QAbstractButton::clicked, [this]() {
        setScanMode(SNSDK_SCANMODE_INFRARED);
        this->close();
    });
}

ChooseModePage::~ChooseModePage()
{
    delete ui;
}

int ChooseModePage::scanmode()
{
    return m_iScanMode;
}

void ChooseModePage::setScanMode(int iScanMode)
{
    m_iScanMode = iScanMode;
    emit scanModeChanged(iScanMode);
}

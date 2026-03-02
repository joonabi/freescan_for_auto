#include "sn3dsdkmainviewbase.h"
#include "ui_sn3dsdkmainviewbase.h"

Sn3DSDKMainViewBase::Sn3DSDKMainViewBase(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Sn3DSDKMainViewBase)
{
    ui->setupUi(this);
}

Sn3DSDKMainViewBase::~Sn3DSDKMainViewBase()
{
    delete ui;
}

int Sn3DSDKMainViewBase::deviceType()
{
    return m_iDeviceType;
}

void Sn3DSDKMainViewBase::setDeviceType(int iDeviceType)
{
    m_iDeviceType = iDeviceType;
}

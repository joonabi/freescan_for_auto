#include "deviceselectdialog.h"
#include "ui_deviceselectdialog.h"
#include "windows.h"
#include "sn3dsdkcorepub.h"

DeviceSelectDialog::DeviceSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceSelectDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, [this](){
        this->accept();
    });
    connect(ui->buttonBox, &QDialogButtonBox::rejected, [this](){
        this->reject();
    });
}

DeviceSelectDialog::~DeviceSelectDialog()
{
    delete ui;
}

int DeviceSelectDialog::deviceType()
{
    int iIndex = ui->cboDeviceType->currentIndex();
    if (iIndex == 0)
        return SNSDK_DEVTYPE_UEPRO;
    else if (iIndex == 1)
        return SNSDK_DEVTYPE_COMBO;
    else if (iIndex == 2)
        return SNSDK_DEVTYPE_5M;
    else if (iIndex == 3)
        return SNSDK_DEVTYPE_9M;
    else if (iIndex == 4)
        return SNSDK_DEVTYPE_E10;
	else if (iIndex == 5)
		return SNSDK_DEVTYPE_2X;
	else if (iIndex == 6)
		return SNSDK_DEVTYPE_TRIO;
    else if (iIndex == 7)
		return SNSDK_DEVTYPE_UEP2;
    else if (iIndex == 8)
		return SNSDK_DEVTYPE_COMBOPLUS;
    return SNSDK_DEVTYPE_UE;
}

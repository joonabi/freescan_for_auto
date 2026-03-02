#include "freescanpostprocessform.h"
#include "ui_freescanpostprocessform.h"

FreeScanPostProcessForm::FreeScanPostProcessForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FreeScanPostProcessForm)
{
    ui->setupUi(this);
}

FreeScanPostProcessForm::~FreeScanPostProcessForm()
{
    delete ui;
}

FreeScanPostProcessForm::AutoFillType FreeScanPostProcessForm::autoFillType()
{
    if (0 == ui->cboAutoFillType->currentIndex())
        return FreeScanPostProcessForm::AutoFillType::Curvate;
    else if (1 == ui->cboAutoFillType->currentIndex())
        return FreeScanPostProcessForm::AutoFillType::Tangent;
    else
        return FreeScanPostProcessForm::AutoFillType::Plane;
}

void FreeScanPostProcessForm::setAutoFillType(FreeScanPostProcessForm::AutoFillType fillType)
{
    m_autoFillType = fillType;
    emit autoFillTypeChanged(m_autoFillType);
    if (m_autoFillType == FreeScanPostProcessForm::AutoFillType::Tangent)
        ui->cboAutoFillType->setCurrentIndex(1);
    else if(m_autoFillType == FreeScanPostProcessForm::AutoFillType::Plane)
        ui->cboAutoFillType->setCurrentIndex(2);
    else
        ui->cboAutoFillType->setCurrentIndex(0);
}       

float FreeScanPostProcessForm::markerPerimeter()
{
    auto strPerimeter = ui->txtPrimeter->text();
    bool bOK = false;
    float fPerimeter = strPerimeter.toDouble(&bOK);
    if (bOK)
        return fPerimeter;
    else
        return m_fMarkerPerimeter;
}

void FreeScanPostProcessForm::setMarkerPerimeter(float fMarkerPerimeter)
{
    m_fMarkerPerimeter = fMarkerPerimeter;
    emit markerPerimeterChanged(m_fMarkerPerimeter);
    auto strPerimeter = QString("{0}").arg(m_fMarkerPerimeter);
    ui->txtPrimeter->setText(strPerimeter);
}

void FreeScanPostProcessForm::on_btnApplyAuto_clicked(bool bChecked)
{
    emit applied(autoFillType(), markerPerimeter());
}

void FreeScanPostProcessForm::on_btnConfirmAuto_clicked(bool bChecked)
{
    emit confirmed(autoFillType(), markerPerimeter());
}

void FreeScanPostProcessForm::on_btnCancelAuto_clicked(bool bChecked)
{
    emit canceled();
}

void FreeScanPostProcessForm::on_btnRollbackAuto_clicked(bool bChecked)
{
    emit rollbacked();
}

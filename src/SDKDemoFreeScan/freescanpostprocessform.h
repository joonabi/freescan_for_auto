#ifndef FREESCANPOSTPROCESSFORM_H
#define FREESCANPOSTPROCESSFORM_H

#include <QDialog>

namespace Ui {
class FreeScanPostProcessForm;
}

class FreeScanPostProcessForm : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(AutoFillType autoFillType READ autoFillType WRITE setAutoFillType NOTIFY autoFillTypeChanged)
    Q_PROPERTY(float markerPerimeter READ markerPerimeter WRITE setMarkerPerimeter NOTIFY markerPerimeterChanged)
public:
    explicit FreeScanPostProcessForm(QWidget *parent = nullptr);
    ~FreeScanPostProcessForm();
    enum class AutoFillType :int { Curvate, Tangent, Plane };
    virtual AutoFillType autoFillType();
    virtual void setAutoFillType(AutoFillType fillType);
    virtual float markerPerimeter();
    virtual void setMarkerPerimeter(float fMarkerPerimeter);
Q_SIGNALS:
    virtual void autoFillTypeChanged(AutoFillType newFillType);
    virtual void markerPerimeterChanged(float newPerimeter);
    virtual void rollbacked();
    virtual void applied(AutoFillType fillType, float fPerimeter);
    virtual void confirmed(AutoFillType fillType, float fPerimeter);
    virtual void canceled();
private:
    Ui::FreeScanPostProcessForm *ui;
    AutoFillType m_autoFillType = AutoFillType::Curvate;
    float m_fMarkerPerimeter = 80;
private Q_SLOTS:
    void on_btnApplyAuto_clicked(bool bChecked = false);
    void on_btnConfirmAuto_clicked(bool bChecked = false);
    void on_btnCancelAuto_clicked(bool bChecked = false);
    void on_btnRollbackAuto_clicked(bool bChecked = false);
};
Q_DECLARE_METATYPE(FreeScanPostProcessForm::AutoFillType);
#endif // FREESCANPOSTPROCESSFORM_H

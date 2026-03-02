#ifndef DEVICESELECTDIALOG_H
#define DEVICESELECTDIALOG_H

#include <QDialog>

namespace Ui {
class DeviceSelectDialog;
}

class DeviceSelectDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(int deviceType READ deviceType)
public:
    explicit DeviceSelectDialog(QWidget *parent = nullptr);
    ~DeviceSelectDialog();
    int deviceType();
private:
    Ui::DeviceSelectDialog *ui;
};

#endif // DEVICESELECTDIALOG_H

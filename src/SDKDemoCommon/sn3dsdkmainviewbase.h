#ifndef SN3DSDKMAINVIEWBASE_H
#define SN3DSDKMAINVIEWBASE_H

#include <QWidget>

namespace Ui {
class Sn3DSDKMainViewBase;
}

class Sn3DSDKMainViewBase : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int deviceType READ deviceType WRITE setDeviceType)
public:
    explicit Sn3DSDKMainViewBase(QWidget *parent = nullptr);
    virtual ~Sn3DSDKMainViewBase();
    virtual int deviceType();
    virtual void setDeviceType(int iDeviceType);
private:
    Ui::Sn3DSDKMainViewBase *ui;
    int m_iDeviceType = 0;
};

#endif // SN3DSDKMAINVIEWBASE_H

#pragma once
#include "qobject.h"

class ISelectEditPanel : public QObject
{
    Q_OBJECT
public:
Q_SIGNALS:
    /**
    *@brief view changed
    */
    virtual void viewChanged(int iView);
};
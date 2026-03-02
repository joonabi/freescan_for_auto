#pragma once
#include "qobject.h"

class IScanControlPanel : public QObject
{
    Q_OBJECT
public:
    virtual void scanClicked();
    virtual void newProjectClicked();
    virtual void openProjectClicked();
    virtual void deleteClicked();
    virtual void alignClicked();
    virtual void saveClicked();
    virtual void directMeshClicked();
    virtual void optimizeMeshClicked();
};

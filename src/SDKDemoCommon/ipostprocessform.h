#pragma once
#include <QObject>

class IPostProcessForm
{
    Q_PROPERTY(AutoFillType autoFillType READ autoFillType WRITE setAutoFillType NOTIFY autoFillTypeChanged)
    Q_PROPERTY(float markerRadius READ markerRadius WRITE setMarkerRadius NOTIFY markerRadiusChanged)
public:
    enum class AutoFillType:int {Curvate, Tangent, Plane};
    virtual AutoFillType autoFillType();
    virtual void setAutoFillType(AutoFillType fillType);
    virtual float markerRadius();
    virtual void setMarkerRadius(float fRadius);
/*public Q_SIGNALS:*/
    virtual void autoFillTypeChanged(AutoFillType newFillType);
    virtual void markerRadiusChanged(float newRadius);
private:
    AutoFillType m_autoFillType = AutoFillType::Curvate;
    float m_fMarkerRadius = .0;
};

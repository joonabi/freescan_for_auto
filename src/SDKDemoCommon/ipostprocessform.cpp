#include "ipostprocessform.h"



IPostProcessForm::AutoFillType IPostProcessForm::autoFillType()
{
    return m_autoFillType;
}

void IPostProcessForm::setAutoFillType(IPostProcessForm::AutoFillType fillType)
{
    m_autoFillType = fillType;
    emit autoFillTypeChanged(m_autoFillType);
}

float IPostProcessForm::markerRadius()
{
    return m_fMarkerRadius;
}

void IPostProcessForm::setMarkerRadius(float fRadius)
{
    m_fMarkerRadius = fRadius;
    emit markerRadiusChanged(m_fMarkerRadius);
}



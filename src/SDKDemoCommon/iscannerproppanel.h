#pragma once

class IScannerPropPanel
{
public:
    virtual void frameworkFileOpened(const QString& strFileName) = 0;
    virtual  ~IScannerPropPanel() = default;
    /**
    *@brief Scan type changed
    *@param iType[in] 0 scan framework, 1 scan grid/mesh
    *                 2 scan photographic
    */
    virtual void projectTypeChanged(int iType) = 0;
    virtual void pointDistChanged(double dPointDist) = 0;
    /**
    *brief Whether append framework to existing framework
    *param bIsIncrease[in] is increasing
    */
    virtual void isIncFrameChanged(bool bIsIncrease) = 0;
    /**
    *@brief High speed scan
    */
    virtual void isHighSpeedChanged(bool bIsHigh) = 0;
    /**
    *@brief Cast light type
    *param iType[in] cast light type, 0 26 laser grade, 1 7 laser grade
    *                                 2 single laser grade
    */
    virtual void castLightTypeChaned(int iType) = 0;
    virtual void objectTypeChanged(int iType) = 0;
    virtual void brightStepChanged(int iStep) = 0;
    virtual void isStrongLightChanged(bool bIsStrong) = 0;
    /**
    *@brief 是否开启质量色谱
    */
    virtual void isQualityColor(bool bIsColor) = 0;
    /**
    *@brief Ruler file list changed
    */
    virtual void rulerFileListChanged(const QStringList rulerFileList) = 0;
    /**
    *@brief setProjectType
    *@param strType "0"=framework "2"=grid
    */
    virtual void setProjectType(const QString& strType) = 0;
    /**
    *@brief setPointDist
    *@param strDist "0.5" "0.2" "0.1"
    */
    virtual void setPointDist(const QString& strDist) = 0;
    /**
    *@brief setLaserGrade
    *@param strGrade "26" "5" "7" "1"
    */
    virtual void setLaserGrade(const QString& strGrade) = 0;
    /**
    *@brief setMaterial
    *@param strMaterial "0" "1"
    */
    virtual void setMaterial(const QString& strMaterial) = 0;
    /**
    *@brief setHighSeed
    *@param strHigh "0" "1"
    */
    virtual void setHighSeed(const QString& strHigh) = 0;
    /**
    *@brief setStrongLight
    *@param strStrong "0" "1"
    */
    virtual void setStrongLight(const QString& strStrong) = 0;
    /**
    *@brief setPointDistList
    */
    virtual void setPointDistList(const QList<double>& listDist) = 0;
    /**
    *@brief setBrightnessLevel
    */
    virtual void setBrightnessLevel(const QString& strLevel) = 0;
};


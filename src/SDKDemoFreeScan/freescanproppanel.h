#ifndef FREESCANPROPPANEL_H
#define FREESCANPROPPANEL_H

#include <QWidget>
#include <QSlider>
#include <QObject>
#include <QMouseEvent>
#include "iscannerproppanel.h"

namespace Ui {
class FreeScanPropPanel;
}

class FreeScanPropPanel : public QWidget, public IScannerPropPanel
{
    Q_OBJECT
public:
    explicit FreeScanPropPanel(QWidget *parent = nullptr);
    ~FreeScanPropPanel();
	/**
	*@brief 设置扫描设备 不同设备面板控件显隐不同
	*/
	void setDeviceType(int deviceType);

Q_SIGNALS:
    void frameworkFileOpened(const QString& strFileName);
    /**
    *@brief Scan type changed
    *@param iType[in] 0 scan framework, 1 scan grid/mesh
    *                 2 scan photographic
    */
    void projectTypeChanged(int iType);
    /**
    @brief To be impleted
    */
    void pointDistChanged(double dPointDist);
    void pointDistChanged(int iIndex);
    /**
    *brief Whether append framework to existing framework
    *param bIsIncrease[in] is increasing
    */
    void isIncFrameChanged(bool bIsIncrease);
    /**
    *@brief High speed scan
    */
    void isHighSpeedChanged(bool bIsHigh);
    /**
    *@brief Cast light type
    *param iType[in] cast light type, 0 26 laser grade, 1 7 laser grade
    *                                 2 single laser grade
    */
    virtual void castLightTypeChaned(int iType);
    virtual void objectTypeChanged(int iType);
    virtual void brightStepChanged(int iStep);
	/**
	*@brief 框架点扫描、摄影测量的高中低亮度选择
	*param id[int] 选择按钮ID
	*/
	virtual void brightRadioChanged(int id);
    virtual void isStrongLightChanged(bool bIsStrong);
    /**
    *@brief 是否开启质量色谱
    */
    void isQualityColor(bool bIsColor);
    void deepHoleModeChanged(bool enable);
    void pointCloudModeChanged(bool enable);
    void rulerFileListChanged(const QStringList rulerFileList);
    void alignModeChanged(int type);
public Q_SLOTS:
    virtual void on_widRulerFile_rulerFileListChanged(const QStringList& rulerFileList);
    virtual void on_sliderPointDist_valueChanged(int iValue);
public:
    /**
    *@brief setProjectType
    *@param strType "0"=framework "2"=grid
    */
    virtual void setProjectType(const QString& strType);
    /**
    *@brief setPointDist
    *@param strDist "0.5" "0.2" "0.1"
    */
    virtual void setPointDist(const QString& strDist);
    /**
    *@brief setLaserGrade
    *@param strGrade "26" "5" "7" "1"
    */
    virtual void setLaserGrade(const QString& strGrade);
    /**
    *@brief setMaterial
    *@param strMaterial "0" "1"
    */
    virtual void setMaterial(const QString& strMaterial);
    /**
    *@brief setHighSeed
    *@param strHigh "0" "1"
    */
    virtual void setHighSeed(const QString& strHigh);
    /**
    *@brief setStrongLight
    *@param strStrong "0" "1"
    */
    virtual void setStrongLight(const QString& strStrong);
    /**
    *@brief setPointDistList
    */
    virtual void setPointDistList(const QList<double>& listDist);
    /**
    *@brief setBrightnessLevel
    */
    virtual void setBrightnessLevel(const QString& strLevel);
private:
    Ui::FreeScanPropPanel *ui;
    QList<double> m_listDist;
};
class FreeScanSlider :public QSlider
{
public:
	FreeScanSlider(QWidget* parent) :QSlider(parent) {};
	void mousePressEvent(QMouseEvent *ev);
};
#endif // FREESCANPROPPANEL_H

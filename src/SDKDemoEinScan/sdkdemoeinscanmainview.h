#ifndef SDKDEMOEINSCANMAINVIEW_H
#define SDKDEMOEINSCANMAINVIEW_H

#include <QWidget>
#include "sn3dsdkmainviewbase.h"
#include "sn3dsdkdemoapp.h"
#include <QFileDialog>
#include <QFileInfo>
#include "einscantabwidget.h"
#include <QDateTime>
#include "glog/logging.h"
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include "einscanprogressbar.h"
#define LOG_INFO_FUN_BEGIN printLog( QString("[%1] %2%3").arg(QDateTime::currentDateTime().toString(("MM-dd hh:mm:ss"))).arg(__func__).arg(" Begin"),LogType::INTERAL_LOG)
#define LOG_INFO_FUN_END   printLog( QString("[%1] %2%3").arg(QDateTime::currentDateTime().toString(("MM-dd hh:mm:ss"))).arg(__func__).arg(" End"),LogType::INTERAL_LOG)
#define PRINT_LOG(ec) printLog(QString("[%1] %2 ret 0x%3").arg(QDateTime::currentDateTime().toString(("MM-dd hh:mm:ss"))).arg(__func__).arg(ec,8,16,QChar('0')),errCode == SNSDK_ERR_OK ? LogType::RECEIVE_MESSAGE : LogType::ERROR_MESSAGE)
#define PRINT_LOG_INFO(info, logType) \
{ \
    QString tmp = QString("[%1] %2").arg(QDateTime::currentDateTime().toString(("MM-dd hh:mm:ss"))).arg(info);\
    emit updatePrintLog(tmp, logType);\
}
#define COV_QSTRING_EC(msg, ec) QString("%1 error code:%2").arg(msg).arg(ec,8,16,QChar('0'))
#define COV_QSTRING(msg, ec) QString("%1%2").arg(msg).arg(ec)
namespace Ui {
	class SDKDemoEinScanMainView;
}

class SDKDemoEinScanMainView : public Sn3DSDKMainViewBase
{
    Q_OBJECT

    enum LogType
    {
        SEND_MESSAGE,				//send to service
        RECEIVE_MESSAGE,			//receive message from service,only correct result
        ERROR_MESSAGE,				//receive message from service,only incorrect result
        INTERAL_LOG					//interal log
    };
    //Q_PROPERTY(ScanType getScanType READ getScanType WRITE setScanType NOTIFY scanTypeChanged)

	enum ProcessControl
	{
		INIT,
		CONNECT_DEVICE,
		RECONNECT_DEVICE,
		MODE_CHOOSE,
		ENTER_SCAN,		
		CREATE_SLN,
		OPEN_SLN,
		SET_SCANPARS,
		PREVIEW_SCAN,
		START_SCAN,
		PAUSE_SCAN,
		END_SCAN,
		CLEAR_SCAN,
		EXIT_SCAN,
		MESH,
		SAVE_DATA,		
		RELEASE
	};
public:
    explicit SDKDemoEinScanMainView(QWidget *parent = nullptr);
    ~SDKDemoEinScanMainView();

private slots:
    ScanType getScanType() const;
    void setScanType(ScanType newScanType);
	void btnStatesInit();

    //点击初始化
    void onInitializeBtnClicked();
	//点击连接设备
	void onConnectDeviceBtnClicked();
	//点击重连设备
	void onReConnectDeviceBtnClicked();
	//点击Release
	void onReleaseBtnClicked();
	//当前界面改变
	void onStackedWidgetIndexChanged(int cIndex);
	//关闭视频流
	void on_startProcessVideoData_clicked();
	//开启视频流
	void on_stopProcessVideoData_clicked();

	//开启与关闭背景屏蔽
	void onmaskBackGroundtoggled(int state);

	//点击选择固定扫描模式
	void onRadioButtonFixedClicked();
	//点击选择手持精细模式
	void onRadioButtonHDClicked();
	//点击选择手持快速模式 
	void onRadioButtonRapidClicked();

	//点击进入扫描
	void onEnterScanBtnClicked();
	//点击新建解决方案
	void onCreateSolutionBtnClicked();
    //点击打开解决方案
    void onOpenSolutionBtnClicked();
	//点击打开框架点文件
	void onLoadP3FileBtnClicked();
	//点击预扫
	void onPreviewScanBtnClicked();
	//点击开始扫描
	void onStartScanBtnClicked();
	//点击暂停扫描
	void onPauseScanBtnClicked();
	//点击继续扫描
	void onContinueScanBtnClicked();
	//点击结束扫描SS
	void onEndScanBtnClicked();
	//点击清除
	void onClearScanBtnClicked();
	//点击退出扫描
	void onExitScanBtnClicked();
	//点击封装
	void onMeshBtnClicked();
	//点击保存数据
	void onSaveBtnClicked();
	//点击保存数据
	void onSetScanParamBtnClicked();
	//点击全局优化
	void onGlobalOptimizationBtnClicked();
	//点击自动特征拼接
	void onPointFeatureBtnClicked();
	//点击手动特征拼接
	void onPointManualBtnClicked();
	//点击获取固定组坐标
	void onVFixPosBtnClicked();
	//点击获取浮动组坐标
	void onVFloatPosBtnClicked();


    //点击设置设备亮度
    void on_SetDeviceBrightness_clicked();
	//点击设置设备背景屏蔽度
	//void on_SetDeviceMaskBackground_clicked();

	void showProgressBarDialog();
	void hideProgressBarDialog();


    void videoData(int camId,const unsigned char* data, int  width, int height, int channel);
    void handleCameraPosition(LPSn3dCameraPosition cameraPosition);
    void handleTrackLostStatus(bool trackLostStatus);
    void handleScanDist(double scanDist);
    void handleDeviceEvent(DeviceEvent event);
    void handlePointCount(int pointCount);
    void handleFrameRate(int frameRate);
    void handleFrameCount(int frameCount);
    void handleTooFlatStatus(bool tooFlatStatus);
    void handleDeviceStatus(LPDeviceStatus deviceStatus);

	void handleFlushRangeData(const LPSn3dRangeData rangeData);
	void initMyRangeData();
	void initFloatRangeData();
	void pushBackMyRangePointData(const Sn3DRangeData* rangeData);
	void pushBackMyRangePointNormalData(const Sn3DRangeData* rangeData);
	void pushBackMyRangeMarkerData(const Sn3DRangeData* rangeData);
	void pushBackMyRangeMarkerNormalData(const Sn3DRangeData* rangeData);
	void pushBackMyRangeMarkerIDData(const Sn3DRangeData* rangeData);
	void pushBackMyRangeMarkerRadiusData(const Sn3DRangeData* rangeData);
	void pushBackMyRangeTexVertexData(const Sn3DRangeData* rangeData);
	void pushBackMyRangeRTData(const Sn3DRangeData* rangeData);
	void readRangeData(QString& sliceName);
	void saveRangeData(const Sn3DRangeData* rangeData, QString& sliceName);
	void copyMyRangeData(const Sn3DRangeData* rangeData, Sn3DRangeData** m_copyRangeData);
	void pushBackData(Sn3dPointData** dataVec, const char* pData, int pointCount);
	Sn3DMarkerId* pushBackIDData(const char* pData, int pointCount);
	Sn3DMarkerRadius* pushBackRadiusData(const char* pData, int pointCount);
	Sn3DRot* pushBackRotData(const char* pData);
	Sn3DTran* pushBackTranData(const char* pData);
	void handleRangeDataFinish();
	void handleFloatRangeData(Sn3DRangeData* floatRangeData);

	void handleProjectInfo(LPSn3dProjectInfo projectInfo);

	void handleBrightnessSliderChange();
	void handleMaskBackGroundSliderChange();

    void onUpdatePrintLog(QString msg, int type);
    void onUpdateBtnStates(int process);

	void clearOnceWhenOpenSolution();

signals:
    void sigGetVideoData(int camId,const unsigned char* data, int  width, int height, int channel);
    void sigGetWholePointCloud(LPSn3dPointCloud wholePointCloud);
    void sigGetCurrentPointCloud(LPSn3dPointCloud currentPointCloud);
    void sigGetIncreasePointCloud(LPSn3dIncreasePointCloud increasePointCloud);
    void sigGetCameraPosition(LPSn3dCameraPosition cameraPosition);
    void sigGetTrackLostStatus(bool trackLostStatus);
    void sigGetScanDist(double scanDist);
    void sigGetDeviceEvent(DeviceEvent event);
    void sigGetPointCount(int pointCount);
    void sigGetFrameRate(int frameRate);
    void sigGetFrameCount(int frameCount);
    void sigGetTooFlatStatus(bool tooFlatStatus);
    void sigGetDeviceStatus(bool offline);
    void updatePrintLog(QString msg, int type);
    void updateBtnStates(int process);

private:
    //日志输出
    void printLog(QString msg, LogType type);
	//设置界面使能
	void setEnabledRecursive(QWidget *widget, bool enabled);
private:
    Ui::SDKDemoEinScanMainView *ui;
	EinscanProgressBar *progressBarWidget;
    Sn3DSDKDemoApp* m_demoApp = nullptr;
    EinScanTabWidget* m_scanPanel = nullptr;
	ScanType m_scanType = ST_RAPID;
	OpenOrCreateSln m_slnParms;
	QFutureWatcher<int> watcher;
	bool m_hasTexture = false;
    QString m_slnPath = "";
    QString m_savePath = "";
	QString m_p3dPath = "";
    const int TRY_CNT = 7;
	LPSn3dRangeData m_rangeData = nullptr;
	LPSn3dRangeData m_floatRangeData = nullptr;
	std::string m_curFloatPrjName = {};
	float* m_curRot = nullptr;
	float* m_curTran = nullptr;
	QList<Sn3DProjectInfo> m_projectInfoList;
	int m_vfixPosCount = 0;
	int m_vfloatPosCount = 0;
	int m_flushProcess = -1;
	LPSn3dPointData m_vfixPos = nullptr;
	LPSn3dPointData m_vfloatPos = nullptr;
	bool m_turTableChecked = false;
	int m_curGroup = 1;
	bool m_deviceOffline = true;
	bool m_openSolution = false;
	QString m_lastAlignMod = "";
	QString m_curAlignMod = "";
	std::mutex m_mtx;
};

#endif // SDKDEMOEINSCANMAINVIEW_H

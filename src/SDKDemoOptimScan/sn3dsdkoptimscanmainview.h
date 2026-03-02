#ifndef SN3DSDKOPTIMSCANMAINVIEW_H
#define SN3DSDKOPTIMSCANMAINVIEW_H

#include "sn3dsdkmainviewbase.h"
#include "sn3dsdkdemoapp.h"
#include <QFileDialog>
#include <QFileInfo>
#include "optimscantabwidget.h"
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

#define LOG_INFO_FUN_BEGIN printLog( QString("%1%2").arg(__FUNCTION__).arg(" Begin"),LogType::INTERAL_LOG)
#define LOG_INFO_FUN_END   printLog( QString("%1%2").arg(__FUNCTION__).arg(" End"),LogType::INTERAL_LOG)
#define PRINT_LOG(funcName,errCode) printLog(QString("%1 ret 0x%2").arg(funcName).arg(errCode,8,16,QChar('0')),errCode == SNSDK_ERR_OK ? LogType::RECEIVE_MESSAGE : LogType::ERROR_MESSAGE)



namespace Ui {
	class Sn3DSDKOptimScanMainView;
}

class Sn3DSDKOptimScanMainView : public Sn3DSDKMainViewBase
{
	Q_OBJECT

    enum LogType
	{
		SEND_MESSAGE,				//send to service
		RECEIVE_MESSAGE,			//receive message from service,only correct result
		ERROR_MESSAGE,				//receive message from service,only incorrect result
		INTERAL_LOG					//interal log
    };
	enum AppStep {
		AS_DeviceAdjust,
		AS_Calibration,
		AS_Scan,
		AS_PostProcess,
		AS_MeasurePage,
		AS_Null
    };

public:
	explicit Sn3DSDKOptimScanMainView(QWidget *parent = nullptr);
	~Sn3DSDKOptimScanMainView();

	//视频回调
	void videoCallBack(int camId, const unsigned char * data, int width, int height, int channel);

private slots:
	//打开服务
	void on_openServiceBtn_pressed();
	//新建工程
	void on_newPrjBtn_pressed();
	//打开工程
	void on_openPrjBtn_pressed();
	//设置扫描参数
	void on_setBtn_pressed();
	//设置亮度
	void on_brightnessSlider_valueChanged(int value);
	//进入背景屏蔽
	void on_backGroundSlider_valueChanged(int value);
	//开始扫描
	void on_scanBtn_pressed();
	//全局优化
	void on_optimBtn_pressed();
	//保存
	void on_saveBtn_pressed();
	//封装
	void on_meshBtn_pressed();
	//进入标定
	void on_enterCaliBtn_pressed();
	//标定采图
    void on_snapImgBtn_pressed();
	//标定计算
    void on_CaleBtn_pressed();
	//设备重连
    void on_pushButton_pressed();
	//释放
	void on_releaseBtn_pressed();



    //void on_imporFrameMarkerBtn_pressed();

    void on_socketInitialBtn_pressed();

    void on_socketReleaseBtn_pressed();

    void on_socketConnectBtn_pressed();

    void on_disconnectBtn_pressed();

    void on_sendBtn_pressed();

    void onShowMsg(QString msg, int type);
private:
	//日志输出
	void printLog(QString msg, LogType type);
	////切换界面
	//void switchUIPage(AppStep index);
	////标定采图
	//void caliSnapImg();
 //   //
 //   void hideUIComponent();


Q_SIGNALS:
	void sigGetVideoData(int camId, const unsigned char* data, int  width, int height, int channel);

private:
	Ui::Sn3DSDKOptimScanMainView *ui;

	Sn3DSDKDemoApp* m_demoApp = nullptr;
	bool m_deviceStatus = SNSDK_DEVSTAT_OFFLINE;
	QFutureWatcher<int> watcher;
};

#endif // SN3DSDKOPTIMSCANMAINVIEW_H

#ifndef SN3DSDKFREESCANMAINVIEW_H
#define SN3DSDKFREESCANMAINVIEW_H

#include "sn3dsdkmainviewbase.h"
#include "windows.h"
#include "qevent.h"
#include "iscannerproppanel.h"
#include "scanstep.h"
#include <QSettings>
#include "freescanpostprocessform.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Sn3DSDKFreeScanMainView; }
QT_END_NAMESPACE

class Sn3DSDKDemoApp;
class CalibrationFrame;
class ChooseModePage;

namespace sn3dsdk_c_wrapper
{
    enum DeviceStatus;
}


enum LogType
{
    SEND_MESSAGE,				//send to service
    RECEIVE_MESSAGE,			//receive message from service,only correct result
    ERROR_MESSAGE,				//receive message from service,only incorrect result
    INTERAL_LOG					//interal log
};
class Sn3DSDKFreeScanMainView : public Sn3DSDKMainViewBase
{
    Q_OBJECT
public:
    Sn3DSDKFreeScanMainView(QWidget *parent = nullptr);
    ~Sn3DSDKFreeScanMainView();
    void closeEvent(QCloseEvent *event);
	void videoCallBack(int camId, const unsigned char* data, int  width, int height, int channel);
	void sceneDataCallBack();
Q_SIGNALS:
	void sigGetVideoData(int camId,const unsigned char* data, int  width, int height, int channel);
private:
    Ui::Sn3DSDKFreeScanMainView *ui;
    CalibrationFrame* m_pCalibFrame = nullptr;
	Sn3DSDKDemoApp* m_demoApp;
	unsigned char* m_lastImagePtr;
    std::unique_ptr<IScannerPropPanel> m_pScannerPropPanel = nullptr;
    ScanStep m_scanStep;
    ChooseModePage* m_pChooseModePage = nullptr;
	std::atomic<bool> m_sceneDataProcessFlag = false;
    QList<double> m_listDist;
	QSettings* save_config;
	bool m_bShowCurrentRT = false;
private:
    void showScannerPropPanel(bool bShow);
    /**
    *@brief Update widget from device type
    */
    void updateScanWidget();
    /**
    *@brief Load scan software setting
    */
    void loadScanSoftSetting();
	/**
   *@brief switch to postprocess ui
   */
	void switchToPostProcessPage();
    /**
    *@brief update scan prop panel
    */
    void updateScanProp();

    bool m_bModbusInitialized = { false };
private Q_SLOTS:
    /**
    *@brief 点击CheckDevice按钮
    *@note 暂时不用
    */
    void on_btnCheckDevice_clicked(bool bChecked = false);
    /**
    *@brief 点击OpenService按钮
    */
    void on_btnOpenService_clicked(bool bChecked = false);
    /**
    *@brief 点击CloseService按钮
    */
    void on_btnCloseService_clicked(bool bChecked = false);
    void on_btnEnterScan_clicked(bool bChecked = false);
    /**
    *@brief 点击New Project按钮
    */
    void on_btnNewProject_clicked(bool bChecked = false);
	/**
	*@brief 点击Open Project按钮
	*/
	void on_btnOpenProject_clicked(bool bChecked = false);
	void on_btnMesh_clicked(bool bChecked = false);
	void on_btnOptimizedMesh_clicked(bool bChecked = false);
    /**
    *@brief 点击Scan按钮，用于启动扫描
    */
    void on_btnScan_clicked(bool bChecked = false);
    /**
    *@brief 在消息框内显示消息
    */
    void slot_printInfo(QString info, LogType type = LogType::INTERAL_LOG);
    /**
    *@brief 点击Save PointCloud按钮
    */
    void on_btnSavePointCloud_clicked(bool bChecked = false);
    /**
    *@brief 点击EnterCalib按钮
    */
    void on_btnEnterCalib_clicked(bool bChecked = false);
    /**
    *@brief 点击StartCalib按钮
    */
    void on_btnStartCalib_clicked(bool bChecked = false);
    /**
    *@brief 收到Sn3DSDKDemoApp信号
    */
    void onSnMessageReceived(QVariantMap mapParam);
    /**
    *@brief change scan state
    */
    void onScanStateChange();
    /**
    *@brief click end scan button
    */
    void on_btnEndScan_clicked(bool bChecked = false);
    /**
    *@brief click cancel scan button
    */
    void on_btnCancelScan_clicked(bool bChecked = false);
	/**
	*@brief socketmode initail
	*/
	void on_socketInitialBtn_pressed();
	/**
	*@brief socketmode release
	*/
	void on_socketReleaseBtn_pressed();
	/**
	*@brief socketmode connect
	*/
	void on_socketConnectBtn_pressed();
	/**
	*@brief socketmode disconnect
	*/
	void on_disconnectBtn_pressed();
	/**
	*@brief socketmode send
	*/
	void on_sendBtn_pressed();
    /**
    *@brief init / release modbus
    */
    void on_btnInitializeModbus_clicked(bool checked = false);
	void on_btnReleaseModbus_clicked(bool checked = false);
    void on_btnPostProcess_clicked(bool checked = false);

    void onPostProcessApplied();
    void onPostProcessConfirmed();
    void onPostProcessCanceled();
    void onPostProcessRollbacked();

    void applyPostProcess(FreeScanPostProcessForm::AutoFillType fillType, float fPerimeter);
    void confirmPostProcess(FreeScanPostProcessForm::AutoFillType fillType, float fPerimeter);
    void cancelPostProcess();
    void rollbackPostProcess();
private:
    FreeScanPostProcessForm* m_pPostProcessForm = nullptr;
};
#endif // SN3DSDKFREESCANMAINVIEW_H

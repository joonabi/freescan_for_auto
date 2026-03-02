#include "sdkdemoeinscanmainview.h"
#include "ui_sdkdemoeinscanmainview.h"
#include <QScrollBar>
#include "glog/logging.h"
#include <TlHelp32.h>
#include <windows.h>
#include <string>
bool exitApp(std::wstring appName)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 stProcInfo;
	stProcInfo.dwSize = sizeof(stProcInfo);
	Process32First(hSnapShot, &stProcInfo);
	do
	{
		auto wstrSzExeFile = std::wstring(stProcInfo.szExeFile);
		std::wstring wstrProName = appName;
		if (wstrSzExeFile == wstrProName)
		{
			auto hProcess = ::OpenProcess(PROCESS_TERMINATE, NULL, stProcInfo.th32ProcessID);
			if (nullptr == hProcess)
			{
			}
			bool bRet = ::TerminateProcess(hProcess, NULL);
			if (!bRet)
			{
				if (NULL == hProcess)
				{
					return true;
				}

				typedef DWORD(_stdcall* NtTerminateProcessFunc)(HANDLE, UINT);
				NtTerminateProcessFunc fpNtNtTerminateProcess = NULL;
				HINSTANCE hModule = LoadLibrary(L"ntdll.dll");
				if (hModule != NULL)
				{
					fpNtNtTerminateProcess = (NtTerminateProcessFunc)GetProcAddress(hModule, "NtTerminateProcess");
					if (fpNtNtTerminateProcess)
					{
						fpNtNtTerminateProcess((HANDLE)hProcess, 1);
						return true;
					}
				}

				return false;
			}
			if (bRet && hProcess)
			{
				if (INVALID_HANDLE_VALUE == hProcess)
					return false;

				const int MAX_WAIT_MILLI = 4000;
				DWORD dwWait = ::WaitForSingleObject((HANDLE)hProcess, MAX_WAIT_MILLI);
				return (dwWait != WAIT_OBJECT_0 ? false : true);
			}

			if (hProcess != nullptr)
				::CloseHandle(hProcess);
			break;
		}
	} while (Process32Next(hSnapShot, &stProcInfo));
}
void CALLBACK SDKWrapperEinScanVideoCallback(int camId, const unsigned char * data, int width, int height, int channel, void* pUserData)
{
	SDKDemoEinScanMainView* pWrapper = (SDKDemoEinScanMainView*)pUserData;
	if (pWrapper)
		pWrapper->sigGetVideoData(camId, data, width, height, channel);
}

static auto destructVPos = [&](LPSn3dPointData data)
{
	if (data) { delete[] data; data = nullptr; }
};

SDKDemoEinScanMainView::SDKDemoEinScanMainView(QWidget *parent)
	: Sn3DSDKMainViewBase(parent)
	, ui(new Ui::SDKDemoEinScanMainView)
	, m_demoApp(static_cast<Sn3DSDKDemoApp *>qApp)

{
	ui->setupUi(this);
	progressBarWidget = new EinscanProgressBar(this);
	progressBarWidget->setAttribute(Qt::WA_DeleteOnClose, false);
	progressBarWidget->hide();
	ui->stackedWidget->setCurrentIndex(0);

	connect(ui->brightnessSlider, &QSlider::sliderReleased, this, &SDKDemoEinScanMainView::handleBrightnessSliderChange);
	connect(ui->brightnessSlider, &QSlider::valueChanged, this, [this]()
	{
		if (!ui->brightnessSlider->isSliderDown())
			handleBrightnessSliderChange();
	});
	connect(ui->maskBackgroundSlider, &QSlider::sliderReleased, this, &SDKDemoEinScanMainView::handleMaskBackGroundSliderChange);
	connect(ui->maskBackgroundSlider, &QSlider::valueChanged, this, [this]()
	{
		if (!ui->maskBackgroundSlider->isSliderDown())
			handleMaskBackGroundSliderChange();
	});
	connect(this, &SDKDemoEinScanMainView::sigGetVideoData, this, &SDKDemoEinScanMainView::videoData);
	connect(m_demoApp, &Sn3DSDKDemoApp::deviceStatusChanged, this, &SDKDemoEinScanMainView::handleDeviceStatus);
	connect(m_demoApp, &Sn3DSDKDemoApp::flushRangeData, this, &SDKDemoEinScanMainView::handleFlushRangeData);
	connect(m_demoApp, &Sn3DSDKDemoApp::revealProjectInfo, this, &SDKDemoEinScanMainView::handleProjectInfo);
	connect(m_demoApp, &Sn3DSDKDemoApp::pointCountChanged, this, &SDKDemoEinScanMainView::handlePointCount);
	connect(m_demoApp, &Sn3DSDKDemoApp::frameCountChanged, this, &SDKDemoEinScanMainView::handleFrameCount);
	connect(m_demoApp, &Sn3DSDKDemoApp::frameRateChanged, this, &SDKDemoEinScanMainView::handleFrameRate);
	connect(m_demoApp, &Sn3DSDKDemoApp::cameraPositionChanged, this, &SDKDemoEinScanMainView::handleCameraPosition);
	connect(m_demoApp, &Sn3DSDKDemoApp::trackLostStatusChanged, this, &SDKDemoEinScanMainView::handleTrackLostStatus);
	connect(m_demoApp, &Sn3DSDKDemoApp::scanDistChanged, this, &SDKDemoEinScanMainView::handleScanDist);
	connect(m_demoApp, &Sn3DSDKDemoApp::deviceEventChanged, this, &SDKDemoEinScanMainView::handleDeviceEvent);
	connect(m_demoApp, &Sn3DSDKDemoApp::tooFlatStatusChanged, this, &SDKDemoEinScanMainView::handleTooFlatStatus);
	connect(this, &SDKDemoEinScanMainView::updatePrintLog, this, &SDKDemoEinScanMainView::onUpdatePrintLog);
	connect(this, &SDKDemoEinScanMainView::updateBtnStates, this, &SDKDemoEinScanMainView::onUpdateBtnStates);
	QObject::connect(&watcher, &QFutureWatcher<int>::finished, [&]() {
		hideProgressBarDialog();
	});
	//on_radioButtonRapid_clicked();
	btnStatesInit();


	//点击初始化界面
	connect(ui->radioButtonInitializePage, &QRadioButton::clicked, this, [=]() {
		ui->stackedWidget->setCurrentIndex(0);
	});
	//点击扫描界面
	connect(ui->radioButtonScanPage, &QRadioButton::clicked, this, [=]() {
		ui->stackedWidget->setCurrentIndex(1);
	});
	//界面改变
	connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, &SDKDemoEinScanMainView::onStackedWidgetIndexChanged);
	//点击初始化
	connect(ui->initializeBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onInitializeBtnClicked);
	//点击连接设备
	connect(ui->connectDeviceBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onConnectDeviceBtnClicked);
	//点击重连设备
	connect(ui->reConnectDeviceBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onReConnectDeviceBtnClicked);
	//点击Release
	connect(ui->releaseBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onReleaseBtnClicked);

	//点击选择固定扫描模式
	connect(ui->radioButtonFixed, &QRadioButton::clicked, this, &SDKDemoEinScanMainView::onRadioButtonFixedClicked);
	//点击选择手持精细模式
	connect(ui->radioButtonHD, &QRadioButton::clicked, this, &SDKDemoEinScanMainView::onRadioButtonHDClicked);
	//点击选择手持快速模式
	connect(ui->radioButtonRapid, &QRadioButton::clicked, this, &SDKDemoEinScanMainView::onRadioButtonRapidClicked);

	//点击进入扫描
	connect(ui->enterScanBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onEnterScanBtnClicked);
	connect(ui->enterScanBtn_fixed, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onEnterScanBtnClicked);
	//点击新建解决方案
	connect(ui->createSolutionBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onCreateSolutionBtnClicked);
	connect(ui->createSolutionBtn_fixed, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onCreateSolutionBtnClicked);
	//点击打开解决方案
	connect(ui->openSolutionBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onOpenSolutionBtnClicked);
	connect(ui->openSolutionBtn_fixed, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onOpenSolutionBtnClicked);
	//点击打开框架点文件
	connect(ui->loadP3Btn_fixed, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onLoadP3FileBtnClicked);
	//点击预扫
	connect(ui->previewScanBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onPreviewScanBtnClicked);
	//点击开始扫描
	connect(ui->startScanBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onStartScanBtnClicked);
	connect(ui->startScanBtn_fixed, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onStartScanBtnClicked);
	//点击暂停扫描
	connect(ui->pauseScanBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onPauseScanBtnClicked);
	connect(ui->pauseScanBtn_fixed, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onPauseScanBtnClicked);
	//点击继续扫描
	connect(ui->continueScanBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onContinueScanBtnClicked);
	//点击结束扫描
	connect(ui->endScanBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onEndScanBtnClicked);
	//点击清除
	connect(ui->clearScanBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onClearScanBtnClicked);
	connect(ui->clearScanBtn_fixed, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onClearScanBtnClicked);
	//点击退出扫描
	connect(ui->exitScanBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onExitScanBtnClicked);
	connect(ui->exitScanBtn_fixed, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onExitScanBtnClicked);
	//点击封装
	connect(ui->meshBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onMeshBtnClicked);
	connect(ui->meshBtn_fixed, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onMeshBtnClicked);
	//点击保存数据
	connect(ui->saveBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onSaveBtnClicked);
	connect(ui->saveBtn_fixed, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onSaveBtnClicked);
	//点击设置扫描参数
	connect(ui->setScanParamBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onSetScanParamBtnClicked);
	//点击全局优化
	connect(ui->globalOptimizationBtn, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onGlobalOptimizationBtnClicked);
	//点击特征点自动拼接
	connect(ui->pointFeatureAlignBtn_fixed, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onPointFeatureBtnClicked);
	//点击获取固定组坐标
	connect(ui->vfixPosBtn_fixed, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onVFixPosBtnClicked);
	//点击获取浮动组坐标
	connect(ui->vfloatPosBtn_fixed, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onVFloatPosBtnClicked);
	//点击特征点手动拼接
	connect(ui->pointManualAlignBtn_fixed, &QPushButton::clicked, this, &SDKDemoEinScanMainView::onPointManualBtnClicked);
	//点击选择是否屏蔽背景
	connect(ui->maskBackground, &QCheckBox::stateChanged, this, &SDKDemoEinScanMainView::onmaskBackGroundtoggled);

}

SDKDemoEinScanMainView::~SDKDemoEinScanMainView()
{
	LOG_INFO_FUN_BEGIN;
	int errCode = m_demoApp->Sn3DRelease();
	PRINT_LOG(errCode);
	LOG_INFO_FUN_END;
	exitApp(L"Sn3DSDKDemo.exe");
	delete ui;
	delete progressBarWidget;
}

void SDKDemoEinScanMainView::printLog(QString msg, LogType type)
{
	QColor color = Qt::white;
	switch (type) {
	case LogType::SEND_MESSAGE:
		color = Qt::blue;
		break;
	case LogType::RECEIVE_MESSAGE:
		color = Qt::green;
		break;
	case LogType::ERROR_MESSAGE:
		color = Qt::red;
		break;
	case LogType::INTERAL_LOG:
		color = Qt::black;
		break;
	}
	ui->logTextEdit->setTextColor(color);
	ui->logTextEdit->append(msg);
	QScrollBar *scrollBar = ui->logTextEdit->verticalScrollBar();
	scrollBar->setValue(scrollBar->maximum());
	VLOG(google::GLOG_INFO) << __func__ << msg.toStdString();
}

ScanType SDKDemoEinScanMainView::getScanType() const
{
	return m_scanType;
}

void SDKDemoEinScanMainView::setScanType(ScanType newScanType)
{
	if (m_scanType == newScanType)
		return;
	m_scanType = newScanType;
}

void SDKDemoEinScanMainView::btnStatesInit()
{
	setEnabledRecursive(this, false);
	ui->initializeBtn->setEnabled(true);

	//扫描模式 0 固定, 1 手持高清, 2 手持快速
	m_slnParms.iScanMode = 1;
	//是否纹理
	m_slnParms.hasTexture = false;
	//点距
	m_slnParms.fPointDis = 0.5;
	//Feature ：0；Texture：1； Marker：2；GlobalMarker ：3；EncodeMarker ：4；Turntable ：5；
	m_slnParms.iAlignType = 0;
	//是否非刚体
	m_slnParms.useNonrigid = false;
	//0:新建空解决方案; 1:新建单个工程; 2: 新建解决方案
	m_slnParms.iNeedLimit = 2;
}

//点击初始化
void SDKDemoEinScanMainView::onInitializeBtnClicked()
{
	PRINT_LOG_INFO("Sn3DInitialize begin.", INTERAL_LOG);
	showProgressBarDialog();
	QFuture<int> future = QtConcurrent::run([&]()->int {
		int errCode = m_demoApp->Sn3DInitialize(deviceType());
		if (errCode == SNSDK_ERR_OK) {
			emit updateBtnStates(ProcessControl::INIT);
			PRINT_LOG_INFO(COV_QSTRING("Sn3DInitialize end errCode:", errCode), INTERAL_LOG);
		}
		else
		{
			PRINT_LOG_INFO(COV_QSTRING_EC("Sn3DInitialize failed", errCode), ERROR_MESSAGE);
		}
		return errCode;
	});
	watcher.setFuture(future);

}

//点击release
void SDKDemoEinScanMainView::onReleaseBtnClicked()
{
	PRINT_LOG_INFO("Sn3DRelease begin.", INTERAL_LOG);
	showProgressBarDialog();
	QFuture<int> future = QtConcurrent::run([&]()->int {
		int errCode = m_demoApp->Sn3DRelease();
		if (errCode == SNSDK_ERR_OK) {
			emit updateBtnStates(ProcessControl::RELEASE);
			PRINT_LOG_INFO(COV_QSTRING("Sn3DRelease end errCode:", errCode), INTERAL_LOG);
		}
		else
		{
			PRINT_LOG_INFO(COV_QSTRING_EC("Release failed.", errCode), ERROR_MESSAGE);
		}
		return errCode;
	});
	watcher.setFuture(future);

}

void SDKDemoEinScanMainView::onStackedWidgetIndexChanged(int cIndex)
{
	setEnabledRecursive(this, false);

	if (0 == cIndex) {//初始化界面
		ui->releaseBtn->setEnabled(true);
		ui->radioButtonScanPage->setEnabled(true);
	}
	else if (1 == cIndex) {//扫描界面
		ui->radioButtonFixed->setEnabled(true);
		ui->radioButtonHD->setEnabled(true);
		ui->radioButtonRapid->setEnabled(true);
		ui->radioButtonInitializePage->setEnabled(true);

		ui->maskBackground->setChecked(false);
		ui->radioButtonFixed->setChecked(true);
		ui->radioButtonFixed->clicked();
	}
}

//点击连接设备
void SDKDemoEinScanMainView::onConnectDeviceBtnClicked()
{
	PRINT_LOG_INFO("connectDevice begin.", INTERAL_LOG);
	showProgressBarDialog();
	QFuture<int> future = QtConcurrent::run([&]()->int {
		int errCode = m_demoApp->connectDevice();
		if (errCode == SNSDK_ERR_OK) {
			emit updateBtnStates(ProcessControl::CONNECT_DEVICE);
			PRINT_LOG_INFO(COV_QSTRING("connectDevice end errCode:", errCode), INTERAL_LOG);
		}
		else
		{
			PRINT_LOG_INFO(COV_QSTRING_EC("connectDevice failed.", errCode), ERROR_MESSAGE);
		}
		return errCode;
	});
	watcher.setFuture(future);

}

//点击重连设备
void SDKDemoEinScanMainView::onReConnectDeviceBtnClicked()
{
	PRINT_LOG_INFO("reconnectDevice begin.", INTERAL_LOG);
	showProgressBarDialog();
	QFuture<int> future = QtConcurrent::run([&]()->int {
		int errCode = m_demoApp->reconnectDevice();
		if (errCode == SNSDK_ERR_OK) {
			emit updateBtnStates(ProcessControl::RECONNECT_DEVICE);
			PRINT_LOG_INFO(COV_QSTRING("reconnectDevice end errCode:", errCode), INTERAL_LOG);
		}
		else
		{
			PRINT_LOG_INFO(COV_QSTRING_EC("reconnectDevice failed.", errCode), ERROR_MESSAGE);
		}
		return errCode;
	});
	watcher.setFuture(future);
}


//点击打开框架点文件
void SDKDemoEinScanMainView::onLoadP3FileBtnClicked()
{
	PRINT_LOG_INFO("Sn3DLoadP3File begin.", INTERAL_LOG)
	if (m_scanType == ST_FIXED)
		m_p3dPath = QFileDialog::getOpenFileName(this, "Load P3 File", QDir::homePath() + "/Desktop", "*.p3");
	if (!m_p3dPath.isEmpty()) {
		if (m_slnParms.iScanMode == -1) {
			printLog("Please select the mode first!", ERROR_MESSAGE);
			return;
		}
		//showProgressBarDialog();

		QFuture<int> future = QtConcurrent::run([&]()->int {
			int errCode = m_demoApp->Sn3DLoadP3File(m_p3dPath);
			if (errCode == SNSDK_ERR_OK) {
				ui->fixUuid->clear();
				ui->floatUuid->clear();
				m_projectInfoList.clear();
				m_flushProcess = -1;
				PRINT_LOG_INFO("Load p3 file end.", INTERAL_LOG);
			}
			else {
				PRINT_LOG_INFO(COV_QSTRING_EC("Sn3DLoadP3File failed.", errCode), ERROR_MESSAGE);
			}
			return errCode;
		});
		watcher.setFuture(future);
	}
	else
	{
		PRINT_LOG_INFO("Sn3DLoadP3File:Not select any folder!", ERROR_MESSAGE);
	}
}


//点击打开解决方案
void SDKDemoEinScanMainView::onOpenSolutionBtnClicked()
{
	if (m_scanType == ST_RAPID)
		m_slnPath = QFileDialog::getOpenFileName(this, "Open Solution", QDir::homePath() + "/Desktop", "*.sln_rap");
	else if (m_scanType == ST_HD)
		m_slnPath = QFileDialog::getOpenFileName(this, "Open Solution", QDir::homePath() + "/Desktop", "*.sln_shd");
	else if (m_scanType == ST_FIXED)
		m_slnPath = QFileDialog::getOpenFileName(this, "Open Solution", QDir::homePath() + "/Desktop", "*.sln_fix");

	if (!m_slnPath.isEmpty()) {
		showProgressBarDialog();
		//slnPath += "ProjectGroup";
		//判断是否选择扫描模式初始化参数
		if (m_slnParms.iScanMode == -1) {
			printLog("Please select the mode first!", ERROR_MESSAGE);
			return;
		}

		PRINT_LOG_INFO("Sn3DOpenSolution begin.", INTERAL_LOG);

		QFuture<int> future = QtConcurrent::run([=]()->int {
			//是否新建
			bool isCreate = false;
			//扫描模式 0 固定, 1 手持高清, 2 手持快速
			int   iScanMode = m_slnParms.iScanMode;
			//是否纹理
			bool  hasTexture = m_slnParms.hasTexture;
			//点距
			float fPointDis = m_slnParms.fPointDis;
			//Feature ：0；Texture：1； Marker：2；GlobalMarker ：3；EncodeMarker ：4；Turntable ：5；
			int iAlignType = m_slnParms.iAlignType;
			//是否非刚体
			bool useNonrigid = m_slnParms.useNonrigid;
			//0:新建空解决方案; 1:新建单个工程; 2: 新建解决方案
			int iNeedLimit = m_slnParms.iNeedLimit;
			int errCode = m_demoApp->Sn3DOpenOrCreateSolution(m_slnPath, isCreate, iScanMode, hasTexture, fPointDis, iAlignType, useNonrigid, iNeedLimit);
			if (errCode == SNSDK_ERR_OK) {
				emit updateBtnStates(ProcessControl::OPEN_SLN);
				m_openSolution = true;
				if (!m_deviceOffline) {
					PRINT_LOG_INFO("Start camera video1...", INTERAL_LOG);
					int cnt = 0;
					while (cnt < TRY_CNT)
					{
						errCode = m_demoApp->videoProcessControl(true);
						if (SNSDK_ERR_OK == errCode)
						{
							PRINT_LOG_INFO(COV_QSTRING("videoProcessControl end errCode:", errCode), INTERAL_LOG);
							break;
						}
						Sleep(1000);
						cnt++;
					}
					if (cnt >= TRY_CNT)
					{
						PRINT_LOG_INFO(COV_QSTRING_EC("videoProcessControl failed. ", errCode), ERROR_MESSAGE);
					}
				}
				PRINT_LOG_INFO("Sn3DOpenSolution end.", INTERAL_LOG);
			}
			else
			{
				PRINT_LOG_INFO(COV_QSTRING_EC("Sn3DOpenOrCreateSolution failed.", errCode), ERROR_MESSAGE);
			}
			//PRINT_LOG_INFO("Open solution end.", INTERAL_LOG);
			return errCode;
		});
		watcher.setFuture(future);

	}
	else
	{
		PRINT_LOG_INFO("Sn3DOpenOrCreateSolution:Not select any folder!", ERROR_MESSAGE);
	}
}

//点击新建解决方案
void SDKDemoEinScanMainView::onCreateSolutionBtnClicked()
{
	//路径选择
	PRINT_LOG_INFO("Sn3DCreateSolution begin.", INTERAL_LOG);
	m_slnPath = QFileDialog::getExistingDirectory(this, "new Solution", QDir::homePath() + "/Desktop");
	if (!m_slnPath.isEmpty()) {
		if (m_slnParms.iScanMode == -1) {
			printLog("Please select the mode first!", ERROR_MESSAGE);
			return;
		}
		showProgressBarDialog();

		//设置一下亮度,防止默认亮度过暗导致图无效
		int level = 6;
		if (m_scanType == ScanType::ST_FIXED)
			level = 1;
		ui->brightnessSlider->setValue(level);
		m_demoApp->setBrightness(level);

		// 初始化下背景屏蔽应该关闭,背景屏蔽level仿bright level
		int mask_level = 6;
		if (m_scanType == ScanType::ST_FIXED)
			mask_level = 1;
		m_demoApp->setMaskBackGround(mask_level, false);

		QFuture<int> future = QtConcurrent::run([=]()->int {
			//是否新建		
			bool isCreate = true;
			//扫描模式 0 固定, 1 手持高清, 2 手持快速
			int   iScanMode = m_slnParms.iScanMode;
			//是否纹理
			bool  hasTexture = m_slnParms.hasTexture;
			//点距
			float fPointDis = m_slnParms.fPointDis;
			//Feature ：0；Texture：1； Marker：2；GlobalMarker ：3；EncodeMarker ：4；Turntable ：5；
			int iAlignType = m_slnParms.iAlignType;
			//是否非刚体
			bool useNonrigid = m_slnParms.useNonrigid;
			//0:新建空解决方案; 1:新建单个工程; 2: 新建解决方案
			int iNeedLimit = m_slnParms.iNeedLimit;

			int errCode = m_demoApp->Sn3DOpenOrCreateSolution(m_slnPath, isCreate, iScanMode, hasTexture, fPointDis, iAlignType, useNonrigid, iNeedLimit);
			if (errCode == SNSDK_ERR_OK) {
				emit updateBtnStates(ProcessControl::CREATE_SLN);
				if (!m_deviceOffline) {
					int cnt = 0;
					PRINT_LOG_INFO("Start camera video2...", INTERAL_LOG);
					while (cnt < TRY_CNT)
					{
						errCode = m_demoApp->videoProcessControl(true);
						if (SNSDK_ERR_OK == errCode)
						{
							PRINT_LOG_INFO(COV_QSTRING("videoProcessControl end errCode:", errCode), INTERAL_LOG);
							break;
						}
						Sleep(1000);
						cnt++;
					}
					if (cnt >= TRY_CNT)
					{
						PRINT_LOG_INFO(COV_QSTRING_EC("videoProcessControl failed. ", errCode), ERROR_MESSAGE);
					}
				}
				PRINT_LOG_INFO(COV_QSTRING("Sn3DOpenOrCreateSolution end errCode:", errCode), INTERAL_LOG);
			}
			else
			{
				PRINT_LOG_INFO(COV_QSTRING_EC("Sn3DOpenOrCreateSolution failed.", errCode), INTERAL_LOG);
			}
			//PRINT_LOG_INFO(COV_QSTRING("Sn3DOpenOrCreateSolution end errCode:", errCode), INTERAL_LOG);
			return errCode;
		});
		watcher.setFuture(future);
	}
	else
	{
		PRINT_LOG_INFO("CreateSolution:Not select any folder!", ERROR_MESSAGE);
	}
}

//点击设置设备亮度
void SDKDemoEinScanMainView::on_SetDeviceBrightness_clicked()
{
	LOG_INFO_FUN_BEGIN;
	DeviceBrightness devBrightness;
	devBrightness.iStep = 10;
	devBrightness.szDeviceType = "";
	devBrightness.brightAlignMode = SNSDK_BRIGHT_A_POINTCLOUD;
	devBrightness.scanObject = SNSDK_SCANOBJECT_NORMAL;
	devBrightness.laserGrade = SNSDK_LASERGRADE_OFF;
	devBrightness.hasFilter = false;
	devBrightness.bFilterSwitch = false;
	int errCode = m_demoApp->setDeviceBrightness(&devBrightness);
	PRINT_LOG(errCode);
	LOG_INFO_FUN_END;
}

//点击进入扫描
void SDKDemoEinScanMainView::onEnterScanBtnClicked()
{
	PRINT_LOG_INFO("enterScan begin.", INTERAL_LOG);
	showProgressBarDialog();

	//判断是否选择扫描模式初始化参数
	if (m_slnParms.iScanMode == -1) {
		printLog("Please select the mode first!", ERROR_MESSAGE);
		return;
	}
	ScanType scanType = getScanType();

	QFuture<int> future = QtConcurrent::run([&]()->int {
		int errCode = m_demoApp->enterScan(scanType);
		if (errCode == SNSDK_ERR_OK) {
			//注册视频流回调
			m_demoApp->installGetImagasCallBack(SDKWrapperEinScanVideoCallback, this);
			emit updateBtnStates(ProcessControl::ENTER_SCAN);
			PRINT_LOG_INFO(COV_QSTRING("enterScan end errCode:", errCode), INTERAL_LOG);
		}
		else {
			PRINT_LOG_INFO(COV_QSTRING_EC("enterScan failed.", errCode), ERROR_MESSAGE);
		}

		return errCode;
	});
	watcher.setFuture(future);
}

//点击预扫描
void SDKDemoEinScanMainView::onPreviewScanBtnClicked()
{
	PRINT_LOG_INFO("previewScan begin.", INTERAL_LOG);
	showProgressBarDialog();
	QFuture<int> future = QtConcurrent::run([&]()->int {
		int errCode = m_demoApp->previewScan();
		if (errCode == SNSDK_ERR_OK) {
			emit updateBtnStates(ProcessControl::PREVIEW_SCAN);
			PRINT_LOG_INFO(COV_QSTRING("previewScan end errCode:", errCode), INTERAL_LOG);
		}
		else
		{
			PRINT_LOG_INFO(COV_QSTRING_EC("previewScan failed.", errCode), ERROR_MESSAGE);
		}
		return errCode;
	});
	watcher.setFuture(future);
}

void SDKDemoEinScanMainView::showProgressBarDialog()
{
	//设置弹窗位置
	if (progressBarWidget->parentWidget()) {
		QPoint parentPos = progressBarWidget->parentWidget()->mapToGlobal(QPoint(0, 0));
		/*	int x = parentPos.x() + (progressBarWidget->parentWidget()->width() - progressBarWidget->width()) / 2;
			int y = parentPos.y() + (progressBarWidget->parentWidget()->height() - progressBarWidget->height()) / 3;*/
		int x = (progressBarWidget->parentWidget()->width() - progressBarWidget->width()) / 2;
		int y = (progressBarWidget->parentWidget()->height() - progressBarWidget->height()) / 3;
		progressBarWidget->move(x, y);
	}
	if (progressBarWidget)
		progressBarWidget->show();
	this->setEnabled(false);
}

void SDKDemoEinScanMainView::hideProgressBarDialog()
{
	if (progressBarWidget)
		progressBarWidget->hide();
	this->setEnabled(true);
}

//点击开始扫描
void SDKDemoEinScanMainView::onStartScanBtnClicked()
{
	PRINT_LOG_INFO("Sn3DStartScan begin.", INTERAL_LOG);
	QFuture<int> future = QtConcurrent::run([&]()->int {
		int errCode = m_demoApp->Sn3DStartScan();
		if (errCode == SNSDK_ERR_OK) {
			emit updateBtnStates(ProcessControl::START_SCAN);
			PRINT_LOG_INFO(COV_QSTRING("Sn3DStartScan end errCode:", errCode), INTERAL_LOG);
		}
		else {
			PRINT_LOG_INFO(COV_QSTRING_EC("Sn3DStartScan failed.", errCode), ERROR_MESSAGE);
		}
		return errCode;
	});
	watcher.setFuture(future);

}

//点击暂停扫描
void SDKDemoEinScanMainView::onPauseScanBtnClicked()
{
	PRINT_LOG_INFO("Sn3DPauseScan begin.", INTERAL_LOG);
	QFuture<int> future = QtConcurrent::run([&]()->int {
		int errCode = m_demoApp->Sn3DPauseScan();
		if (errCode == SNSDK_ERR_OK) {
			emit updateBtnStates(ProcessControl::PAUSE_SCAN);
			PRINT_LOG_INFO(COV_QSTRING("Sn3DPauseScan end errCode:", errCode), INTERAL_LOG);
		}
		else
		{
			PRINT_LOG_INFO(COV_QSTRING_EC("Sn3DPauseScan failed.", errCode), ERROR_MESSAGE);
		}
		return errCode;
	});
	watcher.setFuture(future);
}

void SDKDemoEinScanMainView::onContinueScanBtnClicked()
{
	PRINT_LOG_INFO("Sn3DContinueScan begin.", INTERAL_LOG);
	QFuture<int> future = QtConcurrent::run([&]()->int {
		int errCode = m_demoApp->Sn3DContinueScan();
		if (errCode == SNSDK_ERR_OK) {
			PRINT_LOG_INFO(COV_QSTRING("Sn3DContinueScan end errCode:", errCode), INTERAL_LOG);
		}
		else
		{
			PRINT_LOG_INFO(COV_QSTRING_EC("Sn3DContinueScan failed.", errCode), ERROR_MESSAGE);
		}
		return errCode;
	});
	watcher.setFuture(future);
}


//点击停止扫描
void SDKDemoEinScanMainView::onEndScanBtnClicked()
{
	PRINT_LOG_INFO("endScan begin.", INTERAL_LOG);
	showProgressBarDialog();

	Sn3DWholePointCloudCallBack wholeCallback = nullptr;
	QFuture<int> future = QtConcurrent::run([&]()->int {
		bool globalOptimize = true;
		double pointDist = 0.1;
		int errCode = m_demoApp->endScan(globalOptimize, pointDist, wholeCallback);
		if (errCode == SNSDK_ERR_OK) {
			emit updateBtnStates(ProcessControl::END_SCAN);
			PRINT_LOG_INFO(COV_QSTRING("endScan end errCode:", errCode), INTERAL_LOG);
		}
		else
		{
			PRINT_LOG_INFO(COV_QSTRING_EC("endScan failed.", errCode), ERROR_MESSAGE);
		}
		return errCode;
	});
	watcher.setFuture(future);
}

//点击退出扫描
void SDKDemoEinScanMainView::onExitScanBtnClicked()
{
	PRINT_LOG_INFO("exitScan begin.", INTERAL_LOG);
	showProgressBarDialog();
	QFuture<int> future = QtConcurrent::run([&]()->int {
		int errCode = m_demoApp->exitScan();
		if (errCode == SNSDK_ERR_OK) {

			emit updateBtnStates(ProcessControl::EXIT_SCAN);
			PRINT_LOG_INFO(COV_QSTRING("exitScan end errCode:", errCode), INTERAL_LOG);
		}
		else
		{
			PRINT_LOG_INFO(COV_QSTRING_EC("exitScan failed.", errCode), ERROR_MESSAGE);
		}
		return errCode;
	});
	watcher.setFuture(future);
}

//点击保存数据
void SDKDemoEinScanMainView::onSaveBtnClicked()
{
	PRINT_LOG_INFO("Sn3DSaveData begin.", INTERAL_LOG);

	m_savePath = QFileDialog::getSaveFileName(this, "save", QDir::homePath(), "*.asc;;*.ply;;*.stl;;*.obj");
	if (!m_savePath.isEmpty()) {
		showProgressBarDialog();
		QFileInfo fileInfo(m_savePath);

		QFuture<int> future = QtConcurrent::run([=]()->int {
			int errCode = m_demoApp->Sn3DSaveData(fileInfo.fileName(), m_savePath, fileInfo.suffix());
			if (errCode == SNSDK_ERR_OK) {
				emit updateBtnStates(ProcessControl::SAVE_DATA);
				PRINT_LOG_INFO(COV_QSTRING("Sn3DSaveData end errCode:", errCode), INTERAL_LOG);
			}
			else
			{
				PRINT_LOG_INFO(COV_QSTRING_EC("Sn3DSaveData failed.", errCode), ERROR_MESSAGE);
			}
			return errCode;
		});
		watcher.setFuture(future);
	}
}

//点击清空扫描数据
void SDKDemoEinScanMainView::onClearScanBtnClicked()
{
	PRINT_LOG_INFO("clearScan begin.", INTERAL_LOG);
	showProgressBarDialog();
	//是否清空框架点信息
	bool isCancelCurrentProjectFramerMark = true;
	QFuture<int> future = QtConcurrent::run([&]()->int {
		int errCode = m_demoApp->clearScan(isCancelCurrentProjectFramerMark);
		if (errCode == SNSDK_ERR_OK) {
			emit updateBtnStates(ProcessControl::CLEAR_SCAN);
			ui->fixUuid->clear();
			ui->floatUuid->clear();
			m_projectInfoList.clear();
			m_flushProcess = -1;
			PRINT_LOG_INFO(COV_QSTRING("clearScan end errCode:", errCode), INTERAL_LOG);
		}
		else
		{
			PRINT_LOG_INFO(COV_QSTRING_EC("clearScan failed.", errCode), ERROR_MESSAGE);
		}
		return errCode;
	});
	watcher.setFuture(future);
}

//点击网格封装按钮
void SDKDemoEinScanMainView::onMeshBtnClicked()
{
	PRINT_LOG_INFO("Sn3DMesh begin.", INTERAL_LOG);
	showProgressBarDialog();
	QFuture<int> future = QtConcurrent::run([&]()->int {
		//设置封装参数
		LPMeshPars meshPars = new MeshPars();
		meshPars->meshType = 0;
		meshPars->filterLevel = 0;
		meshPars->smoothLevel = 0;
		meshPars->pointDis = 0;
		meshPars->removeSmall = 1;
		meshPars->maxFace = true;
		meshPars->faceLimit = 20000000;
		meshPars->fillMarkerHole = true;
		meshPars->smallHolePerimeter = 10;
		meshPars->spikeSensitivity = true;
		meshPars->fillMarkerHole = true;
		int errCode = m_demoApp->Sn3DMesh(meshPars, nullptr, nullptr);
		if (errCode == SNSDK_ERR_OK) {
			emit updateBtnStates(ProcessControl::MESH);
			PRINT_LOG_INFO(COV_QSTRING("Sn3DMesh end errCode:", errCode), INTERAL_LOG);
		}
		else
		{
			PRINT_LOG_INFO(COV_QSTRING_EC("Sn3DMesh failed.", errCode), ERROR_MESSAGE);
		}
		delete meshPars;
		meshPars = nullptr;
		return errCode;
	});
	watcher.setFuture(future);
}



void SDKDemoEinScanMainView::videoData(int camId, const unsigned char *data, int width, int height, int channel)
{
	QImage image(data, width, height, QImage::Format_RGB888);
	if (m_scanType == ST_FIXED) 
	{
		if (camId != 2) {
			QMatrix qMatrix;
			qMatrix.rotate(180);//  图像旋转
			//qMatrix.rotate(-90);//  手持扫描需要将黑白相机逆时针90度图像才能正
			image = image.transformed(QTransform(qMatrix));
		}
	}
	int widgetWidth;
	int widgetHeight;
	switch (camId)
	{
	case 0:
		widgetWidth = ui->leftCameraWidget->width();
		widgetHeight = ui->leftCameraWidget->height();
		ui->leftCameraWidget->updateImage(QPixmap::fromImage(image).scaled(widgetWidth, widgetHeight));
		break;
	case 1:
		widgetWidth = ui->rightCameraWidget->width();
		widgetHeight = ui->rightCameraWidget->height();
		ui->rightCameraWidget->updateImage(QPixmap::fromImage(image).scaled(widgetWidth, widgetHeight));
		break;
	case 2:
		widgetWidth = ui->textTureCameraWidget->width();
		widgetHeight = ui->textTureCameraWidget->height();
		ui->textTureCameraWidget->updateImage(QPixmap::fromImage(image).scaled(widgetWidth, widgetHeight));
		break;
	default:
		break;
	}
}

void SDKDemoEinScanMainView::handleCameraPosition(LPSn3dCameraPosition cameraPosition) {
	if (cameraPosition)
	{
		QString posStr = QString("position:[%1, %2, %3] ").arg(cameraPosition->position->x, 6, 'f', 2).arg(cameraPosition->position->y, 6, 'f', 2).arg(cameraPosition->position->z, 6, 'f', 2);
		QString centerStr = QString("  center:[%1, %2, %3] ").arg(cameraPosition->center->x, 6, 'f', 2).arg(cameraPosition->center->y, 6, 'f', 2).arg(cameraPosition->center->z, 6, 'f', 2);
		QString upStr = QString("      up:[%1, %2, %3]").arg(cameraPosition->up->x, 6, 'f', 2).arg(cameraPosition->up->y, 6, 'f', 2).arg(cameraPosition->up->z, 6, 'f', 2);
		ui->cameraPositionLabel->setText(posStr + " " + centerStr + " " + upStr);
	}
}
void SDKDemoEinScanMainView::handleTrackLostStatus(bool trackLostStatus) {
	if (trackLostStatus) {
		ui->trackLostLabel->setStyleSheet("color:red");
		ui->trackLostLabel->setText("TrackLost");
	}
	else
		ui->trackLostLabel->setText("");
}
void SDKDemoEinScanMainView::handleScanDist(double scanDist) {
	ui->scanDistanceLabel->setText(QString::number(scanDist));
}
void SDKDemoEinScanMainView::handleDeviceEvent(DeviceEvent event) {
	QString devEvent = "";
	switch (event)
	{
	case DE_NULL:devEvent = "";
		break;
	case DE_DOUBLECLICK:devEvent = "DOUBLECLICK";
		break;
	case DE_CLICK:devEvent = "CLICK";
		break;
	case DE_UP_CLICK:devEvent = "UP_CLICK";
		break;
	case DE_RIGHT_CLICK:devEvent = "RIGHT_CLICK";
		break;
	case DE_LEFT_CLICK:devEvent = "LEFT_CLICK";
		break;
	case DE_SMALL_CLICK:devEvent = "SMALL_CLICK";
		break;
	case DE_LARGE_CLICK:devEvent = "LARGE_CLICK";
		break;
	default:
		break;
	}
	ui->deviceEventLabel->setText(devEvent);
}
void SDKDemoEinScanMainView::handlePointCount(int pointCount) {
	ui->pointCountLabel->setText(QString::number(pointCount));
}
void SDKDemoEinScanMainView::handleFrameRate(int frameRate) {
	ui->fpsLabel->setText(QString::number(frameRate));
}
void SDKDemoEinScanMainView::handleFrameCount(int frameCount) {
	ui->frameCountLabel->setText(QString::number(frameCount));
}
void SDKDemoEinScanMainView::handleTooFlatStatus(bool tooFlatStatus) {
	if (tooFlatStatus) {
		ui->tooFlatStatusLabel->setStyleSheet("color:red");
		ui->tooFlatStatusLabel->setText("TooFlat");
	}

	else
		ui->tooFlatStatusLabel->setText("");
}
void SDKDemoEinScanMainView::handleDeviceStatus(LPDeviceStatus deviceStatus) {
	QString strInfo = QString::fromUtf8("online").toUtf8();
	if (SNSDK_DEVSTAT_OFFLINE == deviceStatus->offline) {
		strInfo = QString::fromUtf8("offline").toUtf8();
		ui->deviceStatusLabel->setStyleSheet("color:red");
		PRINT_LOG_INFO(strInfo, LogType::ERROR_MESSAGE)
			ui->reConnectDeviceBtn->setEnabled(true);
	}
	else {
		//m_demoApp->videoProcessControl(true);
		ui->deviceStatusLabel->setStyleSheet("color:#00ff00");
		ui->reConnectDeviceBtn->setEnabled(false);
		PRINT_LOG_INFO(strInfo, LogType::RECEIVE_MESSAGE)
	}
	ui->deviceStatusLabel->setText(strInfo);

	//是否有纹理
	m_hasTexture = deviceStatus->hasTexture;
	m_deviceOffline = deviceStatus->offline;
}

void SDKDemoEinScanMainView::initMyRangeData()
{
	std::unique_lock<std::mutex> lock(m_mtx);
	m_rangeData = new Sn3DRangeData;
	m_rangeData->dataHead.dataType = DT_END;
	m_rangeData->dataHead.rawDateType = RDT_CHAR;
	m_rangeData->dataHead.totalCount = 0;
	m_rangeData->dataHead.currentPackDataCount = 0;

	m_rangeData->m_bufferLen = 0;
	m_rangeData->m_recDataLen = 0;
	m_rangeData->m_recValiDataLen = 0;
	m_rangeData->m_recPos = 0;
	m_rangeData->m_curRecPackDataLen = 0;

	m_rangeData->rangepoint_count = 0;
	m_rangeData->rangepointnormal_count = 0;
	m_rangeData->rangemarker_count = 0;
	m_rangeData->rangemarkernormal_count = 0;
	m_rangeData->rangemarkerradius_count = 0;
	m_rangeData->rangemarkerid_count = 0;
	m_rangeData->rangetexvertex_count = 0;

	m_rangeData->m_pBuffer = nullptr;
	m_rangeData->m_pData = nullptr;
	m_rangeData->m_tmpBuffer = nullptr;
	m_rangeData->rangepoint = nullptr;
	m_rangeData->rangepointnormal = nullptr;
	m_rangeData->rangemarker = nullptr;
	m_rangeData->rangemarkernormal = nullptr;
	m_rangeData->rangemarkerid = nullptr;
	m_rangeData->rangemarkerradius = nullptr;
	m_rangeData->rangetexvertex = nullptr;
	m_rangeData->rot = new Sn3DRot;
	m_rangeData->tran = new Sn3DTran;
}

void SDKDemoEinScanMainView::initFloatRangeData()
{
	m_floatRangeData = new Sn3DRangeData;
	m_floatRangeData->dataHead.dataType = DT_END;
	m_floatRangeData->dataHead.rawDateType = RDT_CHAR;
	m_floatRangeData->dataHead.totalCount = 0;
	m_floatRangeData->dataHead.currentPackDataCount = 0;

	m_floatRangeData->m_bufferLen = 0;
	m_floatRangeData->m_recDataLen = 0;
	m_floatRangeData->m_recValiDataLen = 0;
	m_floatRangeData->m_recPos = 0;
	m_floatRangeData->m_curRecPackDataLen = 0;

	m_floatRangeData->rangepoint_count = 0;
	m_floatRangeData->rangepointnormal_count = 0;
	m_floatRangeData->rangemarker_count = 0;
	m_floatRangeData->rangemarkernormal_count = 0;
	m_floatRangeData->rangemarkerradius_count = 0;
	m_floatRangeData->rangemarkerid_count = 0;
	m_floatRangeData->rangetexvertex_count = 0;

	m_floatRangeData->m_pBuffer = nullptr;
	m_floatRangeData->m_pData = nullptr;
	m_floatRangeData->m_tmpBuffer = nullptr;
	m_floatRangeData->rangepoint = nullptr;
	m_floatRangeData->rangepointnormal = nullptr;
	m_floatRangeData->rangemarker = nullptr;
	m_floatRangeData->rangemarkernormal = nullptr;
	m_floatRangeData->rangemarkerid = nullptr;
	m_floatRangeData->rangemarkerradius = nullptr;
	m_floatRangeData->rangetexvertex = nullptr;
	m_floatRangeData->rot = new Sn3DRot;
	m_floatRangeData->tran = new Sn3DTran;
}

void SDKDemoEinScanMainView::pushBackMyRangePointData(const Sn3DRangeData* rangeData)
{
	m_rangeData->rangepoint_count = rangeData->rangepoint_count;
	pushBackData(&m_rangeData->rangepoint, (char*)rangeData->rangepoint, m_rangeData->rangepoint_count);
}

void SDKDemoEinScanMainView::pushBackMyRangePointNormalData(const Sn3DRangeData* rangeData)
{
	m_rangeData->rangepointnormal_count = rangeData->rangepointnormal_count;
	pushBackData(&m_rangeData->rangepointnormal, (char*)rangeData->rangepointnormal, m_rangeData->rangepointnormal_count);
}

void SDKDemoEinScanMainView::pushBackMyRangeMarkerData(const Sn3DRangeData* rangeData)
{
	m_rangeData->rangemarker_count = rangeData->rangemarker_count;
	pushBackData(&m_rangeData->rangemarker, (char*)rangeData->rangemarker, m_rangeData->rangemarker_count);
}

void SDKDemoEinScanMainView::pushBackMyRangeMarkerNormalData(const Sn3DRangeData* rangeData)
{
	m_rangeData->rangemarkernormal_count = rangeData->rangemarkernormal_count;
	pushBackData(&m_rangeData->rangemarkernormal, (char*)rangeData->rangemarkernormal, m_rangeData->rangemarkernormal_count);
}

void SDKDemoEinScanMainView::pushBackMyRangeMarkerIDData(const Sn3DRangeData* rangeData)
{
	m_rangeData->rangemarkerid_count = rangeData->rangemarkerid_count;
	m_rangeData->rangemarkerid = pushBackIDData((char*)rangeData->rangemarkerid, m_rangeData->rangemarkerid_count);
}

void SDKDemoEinScanMainView::pushBackMyRangeMarkerRadiusData(const Sn3DRangeData* rangeData)
{
	m_rangeData->rangemarkerradius_count = rangeData->rangemarkerradius_count;
	m_rangeData->rangemarkerradius = pushBackRadiusData((char*)rangeData->rangemarkerradius, m_rangeData->rangemarkerradius_count);
}

void SDKDemoEinScanMainView::pushBackMyRangeTexVertexData(const Sn3DRangeData* rangeData)
{
	m_rangeData->rangetexvertex_count = rangeData->rangetexvertex_count;
	pushBackData(&m_rangeData->rangetexvertex, (char*)rangeData->rangetexvertex, m_rangeData->rangetexvertex_count);
}

void SDKDemoEinScanMainView::pushBackMyRangeRTData(const Sn3DRangeData* rangeData)
{
	m_rangeData->rot = pushBackRotData((char*)rangeData->rot);
	m_rangeData->tran = pushBackTranData((char*)rangeData->tran);
}

void SDKDemoEinScanMainView::handleFloatRangeData(Sn3DRangeData* floatRangeData)
{
	auto destructMySn3DRangeData = [&](LPSn3dRangeData data)
	{
		if (data)
		{
			if (data->m_pBuffer) { delete[] data->m_pBuffer; data->m_pBuffer = nullptr; }
			if (data->m_pData) { delete[] data->m_pData; data->m_pData = nullptr; }
			if (data->m_tmpBuffer) { delete[] data->m_tmpBuffer; data->m_tmpBuffer = nullptr; }
			if (data->rangepoint) { delete[] data->rangepoint; data->rangepoint = nullptr; }
			if (data->rangepointnormal) { delete[] data->rangepointnormal; data->rangemarkernormal = nullptr; }
			if (data->rangemarker) { delete[] data->rangemarker; data->rangemarker = nullptr; }
			if (data->rangemarkernormal) { delete[] data->rangemarkernormal; data->rangemarkernormal = nullptr; }
			if (data->rot) { delete[] data->rot; data->rot = nullptr; }
			if (data->tran) { delete[] data->tran; data->tran = nullptr; }
			if (data->rangemarkerid) { delete[] data->rangemarkerid; data->rangemarkerid = nullptr; }
			if (data->rangemarkerradius) { delete[] data->rangemarkerradius; data->rangemarkerradius = nullptr; }
			if (data->rangetexvertex) { delete[] data->rangetexvertex; data->rangetexvertex = nullptr; }

			data = nullptr;
		}
	};

	for (int i = 0; i < floatRangeData->rangepoint_count; ++i) {
		float x = floatRangeData->rangepoint[i].x;
		float y = floatRangeData->rangepoint[i].y;
		float z = floatRangeData->rangepoint[i].z;
		floatRangeData->rangepoint[i].x = m_curRot[0] * x + m_curRot[1] * y + m_curRot[2] * z + m_curTran[0];
		floatRangeData->rangepoint[i].y = m_curRot[3] * x + m_curRot[4] * y + m_curRot[5] * z + m_curTran[1];
		floatRangeData->rangepoint[i].z = m_curRot[6] * x + m_curRot[7] * y + m_curRot[8] * z + m_curTran[2];
		x = floatRangeData->rangepointnormal[i].x;
		y = floatRangeData->rangepointnormal[i].y;
		z = floatRangeData->rangepointnormal[i].z;
		floatRangeData->rangepointnormal[i].x = m_curRot[0] * x + m_curRot[1] * y + m_curRot[2] * z;
		floatRangeData->rangepointnormal[i].y = m_curRot[3] * x + m_curRot[4] * y + m_curRot[5] * z;
		floatRangeData->rangepointnormal[i].z = m_curRot[6] * x + m_curRot[7] * y + m_curRot[8] * z;
	}
	
	QString folderName = "project_rangeData";
	QDir folderDir(QDir::currentPath());
	if (!folderDir.exists(folderName)) folderDir.mkdir(folderName);
	folderName = QString(QDir::currentPath() + "/" + folderName + "/");
	std::string pre = m_curFloatPrjName + "_delta";
	QString prefix = QString::fromStdString(pre);

	/// 保存点云
	if (floatRangeData->rangepoint && floatRangeData->rangepointnormal)
	{
		QFile file(QString(folderName + "PointCloud" + "_" + prefix + ".asc"));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		QTextStream out(&file);
		for (int i = 0; i < floatRangeData->rangepoint_count; i++)
		{
			out << floatRangeData->rangepoint[i].x << "\t"
				<< floatRangeData->rangepoint[i].y << "\t"
				<< floatRangeData->rangepoint[i].z << "\t"
				<< floatRangeData->rangepointnormal[i].x << "\t"
				<< floatRangeData->rangepointnormal[i].y << "\t"
				<< floatRangeData->rangepointnormal[i].z << "\t"
				<< "\n";
		}
		file.close();
	}

	destructMySn3DRangeData(floatRangeData);
}

void SDKDemoEinScanMainView::readRangeData(QString& sliceName) 
{
	QString folderName = "project_rangeData";
	QDir folderDir(QDir::currentPath());
	if (!folderDir.exists(folderName))
		return;
	folderName = QString(QDir::currentPath() + "/" + folderName + "/");

	/// 读取点云
	QFile file(QString(folderName + "PointCloud" + "_" + sliceName + ".asc"));
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	int idx = 0;
	QTextStream in(&file);
	while (!in.atEnd()) {
		QString line = in.readLine();
		QStringList tokens = line.split("\t");
		if (tokens.size() != 7)
			return;

		idx++;
	}
	m_floatRangeData->rangepoint_count = m_floatRangeData->rangepointnormal_count = idx;
	file.seek(0);
	in.seek(0);
	idx = 0;

	m_floatRangeData->rangepoint = new Sn3dPointData[m_floatRangeData->rangepoint_count];
	m_floatRangeData->rangepointnormal = new Sn3dPointData[m_floatRangeData->rangepointnormal_count];
	while (!in.atEnd()) {
		QString line = in.readLine();
		QStringList tokens = line.split("\t");

		m_floatRangeData->rangepoint[idx].x = tokens[0].toFloat();
		m_floatRangeData->rangepoint[idx].y = tokens[1].toFloat();
		m_floatRangeData->rangepoint[idx].z = tokens[2].toFloat();
		m_floatRangeData->rangepointnormal[idx].x = tokens[3].toFloat();
		m_floatRangeData->rangepointnormal[idx].y = tokens[4].toFloat();
		m_floatRangeData->rangepointnormal[idx].z = tokens[5].toFloat();
		idx++;
	}
	file.close();
}

void SDKDemoEinScanMainView::saveRangeData(const Sn3DRangeData* rangeData, QString& sliceName)
{
	QString folderName = "project_rangeData";
	QDir folderDir(QDir::currentPath());
	if (!folderDir.exists(folderName)) folderDir.mkdir(folderName);
	folderName = QString(QDir::currentPath() + "/" + folderName + "/");
	QString prefix = sliceName;

	/// 保存点云
	if (rangeData->rangepoint && rangeData->rangepointnormal)
	{
		QFile file(QString(folderName + "PointCloud" + "_" + prefix + ".asc"));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		QTextStream out(&file);
		for (int i = 0; i < rangeData->rangepoint_count; i++)
		{
			out << rangeData->rangepoint[i].x << "\t"
				<< rangeData->rangepoint[i].y << "\t"
				<< rangeData->rangepoint[i].z << "\t"
				<< rangeData->rangepointnormal[i].x << "\t"
				<< rangeData->rangepointnormal[i].y << "\t"
				<< rangeData->rangepointnormal[i].z << "\t"
				<< "\n";
		}
		file.close();
	}

	/// 保存标志点
	if (rangeData->rangemarker && rangeData->rangemarkernormal)
	{
		QFile file(QString(folderName + "Marker" + "_" + prefix + ".asc"));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		QTextStream out(&file);
		for (int i = 0; i < rangeData->rangemarker_count; i++)
		{
			out << rangeData->rangemarker[i].x << "\t"
				<< rangeData->rangemarker[i].y << "\t"
				<< rangeData->rangemarker[i].z << "\t"
				<< rangeData->rangemarkernormal[i].x << "\t"
				<< rangeData->rangemarkernormal[i].y << "\t"
				<< rangeData->rangemarkernormal[i].z << "\t"
				<< "\n";
		}
		file.close();
	}

	/// 保存标志点id
	if (rangeData->rangemarkerid)
	{
		QFile file(QString(folderName + "MarkerID" + "_" + prefix + ".asc"));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		QTextStream out(&file);
		for (int i = 0; i < rangeData->rangemarkerid_count; i++)
		{
			out << rangeData->rangemarkerid[i].x << "\t"
				<< "\n";
		}
		file.close();
	}

	/// 保存标志点半径
	if (rangeData->rangemarkerradius)
	{
		QFile file(QString(folderName + "MarkerRadius" + "_" + prefix + ".asc"));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		QTextStream out(&file);
		for (int i = 0; i < rangeData->rangemarkerradius_count; i++)
		{
			out << rangeData->rangemarkerradius[i].x << "\t"
				<< "\n";
		}
		file.close();
	}

	/// 保存纹理
	if (rangeData->rangetexvertex)
	{
		QFile file(QString(folderName + "Texture" + "_" + prefix + ".asc"));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		QTextStream out(&file);
		for (int i = 0; i < rangeData->rangetexvertex_count; i++)
		{
			out << rangeData->rangetexvertex[i].x << "\t"
				<< rangeData->rangetexvertex[i].y << "\t"
				<< rangeData->rangetexvertex[i].z << "\t"
				<< "\n";
		}
		file.close();
	}

	/// 保存RT
	if (rangeData->rot && rangeData->tran)
	{
		QFile file(QString(folderName + "RT" + "_" + prefix + ".asc"));
		if (file.exists())
		{
			if (!file.open(QIODevice::Append | QIODevice::Text))
				return;
		}
		else
		{
			if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
				return;
		}
		QTextStream out(&file);
		auto rt = rangeData->rot;
		auto tn = rangeData->tran;
		out << rt[0].x << "\t"
			<< rt[1].x << "\t"
			<< rt[2].x << "\t"
			<< rt[3].x << "\t"
			<< rt[4].x << "\t"
			<< rt[5].x << "\t"
			<< rt[6].x << "\t"
			<< rt[7].x << "\t"
			<< rt[8].x << "\t"
			<< tn[0].x << "\t"
			<< tn[1].x << "\t"
			<< tn[2].x << "\t"
			<< "\n";
	}
}

void SDKDemoEinScanMainView::pushBackData(Sn3dPointData** dataVec, const char* pData, int pointCount)
{
	*dataVec = new Sn3dPointData[pointCount];
	int dataLen = pointCount * 3 * sizeof(float);
	float *data = (float*)pData;
	memcpy(*dataVec, data, dataLen);
}

Sn3DMarkerId* SDKDemoEinScanMainView::pushBackIDData(const char* pData, int pointCount)
{
	Sn3DMarkerId* dataVec = new Sn3DMarkerId[pointCount];
	int *data = (int*)pData;
	memcpy(dataVec, data, sizeof(data));
	return dataVec;
}

Sn3DMarkerRadius* SDKDemoEinScanMainView::pushBackRadiusData(const char* pData, int pointCount)
{
	Sn3DMarkerRadius* dataVec = new Sn3DMarkerRadius[pointCount];
	int *data = (int*)pData;
	memcpy(dataVec, data, sizeof(data));
	return dataVec;
}

Sn3DRot* SDKDemoEinScanMainView::pushBackRotData(const char* pData)
{
	int dataSize = sizeof(float) * m_rangeData->ROT_NUM;
	Sn3DRot* dataVec = new Sn3DRot[m_rangeData->ROT_NUM];
	float *data = (float*)pData;
	memcpy(dataVec, data, dataSize);
	return dataVec;
}

Sn3DTran* SDKDemoEinScanMainView::pushBackTranData(const char* pData)
{
	int dataSize = sizeof(float) * m_rangeData->TRAN_NUM;
	Sn3DTran* dataVec = new Sn3DTran[m_rangeData->TRAN_NUM];
	float *data = (float*)pData;
	memcpy(dataVec, data, dataSize);
	return dataVec;
}

void SDKDemoEinScanMainView::copyMyRangeData(const Sn3DRangeData* rangeData, Sn3DRangeData** m_copyRangeData)
{
	std::unique_lock<std::mutex> lock(m_mtx);
	(*m_copyRangeData) = new Sn3DRangeData;
	(*m_copyRangeData)->dataHead.dataType = DT_END;
	(*m_copyRangeData)->dataHead.rawDateType = RDT_CHAR;
	(*m_copyRangeData)->dataHead.totalCount = 0;
	(*m_copyRangeData)->dataHead.currentPackDataCount = 0;

	(*m_copyRangeData)->m_bufferLen = 0;
	(*m_copyRangeData)->m_recDataLen = 0;
	(*m_copyRangeData)->m_recValiDataLen = 0;
	(*m_copyRangeData)->m_recPos = 0;
	(*m_copyRangeData)->m_curRecPackDataLen = 0;

	(*m_copyRangeData)->rangepoint_count = 0;
	(*m_copyRangeData)->rangepointnormal_count = 0;
	(*m_copyRangeData)->rangemarker_count = 0;
	(*m_copyRangeData)->rangemarkernormal_count = 0;
	(*m_copyRangeData)->rangemarkerradius_count = 0;
	(*m_copyRangeData)->rangemarkerid_count = 0;
	(*m_copyRangeData)->rangetexvertex_count = 0;

	(*m_copyRangeData)->m_pBuffer = nullptr;
	(*m_copyRangeData)->m_pData = nullptr;
	(*m_copyRangeData)->m_tmpBuffer = nullptr;
	(*m_copyRangeData)->rangepoint = nullptr;
	(*m_copyRangeData)->rangepointnormal = nullptr;
	(*m_copyRangeData)->rangemarker = nullptr;
	(*m_copyRangeData)->rangemarkernormal = nullptr;
	(*m_copyRangeData)->rangemarkerid = nullptr;
	(*m_copyRangeData)->rangemarkerradius = nullptr;
	(*m_copyRangeData)->rangetexvertex = nullptr;
	(*m_copyRangeData)->rot = new Sn3DRot;
	(*m_copyRangeData)->tran = new Sn3DTran;

	(*m_copyRangeData)->rangepoint_count = rangeData->rangepoint_count;
	pushBackData(&(*m_copyRangeData)->rangepoint, (char*)rangeData->rangepoint, (*m_copyRangeData)->rangepoint_count);
	(*m_copyRangeData)->rangepointnormal_count = rangeData->rangepointnormal_count;
	pushBackData(&(*m_copyRangeData)->rangepointnormal, (char*)rangeData->rangepointnormal, (*m_copyRangeData)->rangepointnormal_count);
	(*m_copyRangeData)->rangemarker_count = rangeData->rangemarker_count;
	pushBackData(&(*m_copyRangeData)->rangemarker, (char*)rangeData->rangemarker, (*m_copyRangeData)->rangemarker_count);
	(*m_copyRangeData)->rangemarkernormal_count = rangeData->rangemarkernormal_count;
	pushBackData(&(*m_copyRangeData)->rangemarkernormal, (char*)rangeData->rangemarkernormal, (*m_copyRangeData)->rangemarkernormal_count);
	(*m_copyRangeData)->rangemarkerid_count = rangeData->rangemarkerid_count;
	if (rangeData->rangemarkerid_count > 0) {
		(*m_copyRangeData)->rangemarkerid = pushBackIDData((char*)rangeData->rangemarkerid, rangeData->rangemarkerid_count);
	}
	(*m_copyRangeData)->rangemarkerradius_count = rangeData->rangemarkerradius_count;
	if (rangeData->rangemarkerradius_count > 0) {
		(*m_copyRangeData)->rangemarkerradius = pushBackRadiusData((char*)rangeData->rangemarkerradius, rangeData->rangemarkerradius_count);
	}
	(*m_copyRangeData)->rangetexvertex_count = rangeData->rangetexvertex_count;
	if (rangeData->rangetexvertex_count) {
		pushBackData(&(*m_copyRangeData)->rangetexvertex, (char*)rangeData->rangetexvertex, (*m_copyRangeData)->rangetexvertex_count);
	}
	(*m_copyRangeData)->rot = pushBackRotData((char*)rangeData->rot);
	(*m_copyRangeData)->tran = pushBackTranData((char*)rangeData->tran);
}

static auto destructSn3DRangeData = [&](LPSn3dRangeData data)
{
	if (data)
	{
		if (data->rangepoint) delete[] data->rangepoint;
		if (data->rangepointnormal) delete[] data->rangepointnormal;
		if (data->rangemarker) delete[] data->rangemarker;
		if (data->rangemarkernormal) delete[] data->rangemarkernormal;
		if (data->rot) delete[] data->rot;
		if (data->tran) delete[] data->tran;
		if (data->rangemarkerid) delete[] data->rangemarkerid;
		if (data->rangemarkerradius) delete[] data->rangemarkerradius;
		if (data->rangetexvertex) delete[] data->rangetexvertex;
	}
};

void SDKDemoEinScanMainView::handleFlushRangeData(const LPSn3dRangeData rangeData)
{
	DeserializeErrorCode ret = DeserializeErrorCode::EC_DeserNoError;
	switch (rangeData->dataHead.dataType)
	{
	case DT_POINT_VERTEX:
	{
		initMyRangeData();
		pushBackMyRangePointData(rangeData);
	}
	break;
	case DT_POINT_NORMAL:
	{
		pushBackMyRangePointNormalData(rangeData);
	}
	break;
	case DT_MARKER_VERTEX:
	{
		pushBackMyRangeMarkerData(rangeData);
	}
	break;
	case DT_MARKER_NORMAL:
	{
		pushBackMyRangeMarkerNormalData(rangeData);
	}
	break;
	case DT_MARKER_ID:
	{
		pushBackMyRangeMarkerIDData(rangeData);
	}
	break;
	case DT_MARKER_RADIUS:
	{
		pushBackMyRangeMarkerRadiusData(rangeData);
	}
	break;
	case DT_COLOR_0_1:
	{
		pushBackMyRangeTexVertexData(rangeData);
	}
	break;
	case DT_RT:
	{
		pushBackMyRangeRTData(rangeData);
	}
	break;
	case DT_END:
	{
		m_flushProcess += 1;
		QtConcurrent::run([&](Sn3DRangeData* myData, int sliceNumber) {

			auto destructMySn3DRangeData = [&](LPSn3dRangeData data)
			{
				if (data)
				{
					std::unique_lock<std::mutex> lock(m_mtx);
					if (data->m_pBuffer) { delete[] data->m_pBuffer; data->m_pBuffer = nullptr; }
					if (data->m_pData) { delete[] data->m_pData; data->m_pData = nullptr; }
					if (data->m_tmpBuffer) { delete[] data->m_tmpBuffer; data->m_tmpBuffer = nullptr; }
					if (data->rangepoint) { delete[] data->rangepoint; data->rangepoint = nullptr; }
					if (data->rangepointnormal) { delete[] data->rangepointnormal; data->rangemarkernormal = nullptr; }
					if (data->rangemarker) { delete[] data->rangemarker; data->rangemarker = nullptr; }
					if (data->rangemarkernormal) { delete[] data->rangemarkernormal; data->rangemarkernormal = nullptr; }
					if (data->rot) { delete[] data->rot; data->rot = nullptr; }
					if (data->tran) { delete[] data->tran; data->tran = nullptr; }
					if (data->rangemarkerid) { delete[] data->rangemarkerid; data->rangemarkerid = nullptr; }
					if (data->rangemarkerradius) { delete[] data->rangemarkerradius; data->rangemarkerradius = nullptr; }
					if (data->rangetexvertex) { delete[] data->rangetexvertex; data->rangetexvertex = nullptr; }

					data = nullptr;
				}
			};

			LPSn3dRangeData m_copyRangeData = nullptr;
			copyMyRangeData(myData, &m_copyRangeData);

			destructMySn3DRangeData(myData);

			QString curSliceName = QString::fromStdString(m_projectInfoList.at(sliceNumber).name);
			saveRangeData(m_copyRangeData, curSliceName);
			destructMySn3DRangeData(m_copyRangeData);
		}, m_rangeData, m_flushProcess);
		//destructSn3DRangeData(m_rangeData);
	}
	break;
	default:
		break;
	}
	handleRangeDataFinish();
}

// 处理下一次单片扫描数据
void SDKDemoEinScanMainView::handleRangeDataFinish()
{
	m_demoApp->rangeDataFinish();
}

void SDKDemoEinScanMainView::clearOnceWhenOpenSolution() {
	if (m_openSolution) {
		m_openSolution = false;
		ui->fixUuid->clear();
		ui->floatUuid->clear();
		m_projectInfoList.clear();
		m_flushProcess = -1;
	}
}

void SDKDemoEinScanMainView::handleProjectInfo(LPSn3dProjectInfo projectInfo)
{
	Sn3DProjectInfo element;
	element.dataID = projectInfo->dataID;
	element.name = projectInfo->name;
	element.prjName = projectInfo->prjName;
	clearOnceWhenOpenSolution();
	m_projectInfoList.append(element);
	std::string strInfo = "dataID: " + element.dataID + " prjName: " + element.prjName + " name:" + element.name;
	ui->fixUuid->addItem(QString::fromStdString(element.name));
	ui->floatUuid->addItem(QString::fromStdString(element.name));
	QString qstrInfo = QString::fromStdString(strInfo);
	PRINT_LOG_INFO(qstrInfo, LogType::INTERAL_LOG);
	delete projectInfo;
	projectInfo = nullptr;
}

//点击开始处理视频数据
void SDKDemoEinScanMainView::on_startProcessVideoData_clicked()
{
	m_demoApp->videoProcessControl(true);
}

//点击停止处理视频数据
void SDKDemoEinScanMainView::on_stopProcessVideoData_clicked()
{
	m_demoApp->videoProcessControl(false);
}

void SDKDemoEinScanMainView::handleBrightnessSliderChange()
{
	int level = ui->brightnessSlider->sliderPosition();
	QString strInfo = QString("set brightness level : %1").arg(level);
	m_demoApp->setBrightness(level);
	PRINT_LOG_INFO(strInfo, LogType::INTERAL_LOG);
}

void SDKDemoEinScanMainView::onmaskBackGroundtoggled(int state)
{
	if (state == Qt::Checked) {
		int level = ui->maskBackgroundSlider->sliderPosition();
		ui->maskBackgroundSlider->setEnabled(true);
		m_demoApp->setMaskBackGround(level, true);
	}
	else {
		ui->maskBackgroundSlider->setEnabled(false);
		m_demoApp->setMaskBackGround(0, false);
	}
}

//处理背景屏蔽的滑动条, @todo>> 设置弹窗提示?
void SDKDemoEinScanMainView::handleMaskBackGroundSliderChange()
{
	int level = ui->maskBackgroundSlider->sliderPosition();
	QString strInfo = QString("set mask background level : %1").arg(level);
	m_demoApp->setMaskBackGround(level, true);
	PRINT_LOG_INFO(strInfo, LogType::INTERAL_LOG);
}

void SDKDemoEinScanMainView::onRadioButtonFixedClicked()
{
	ui->stackedWidget_2->setCurrentIndex(1);
	ScanType scanType = ST_FIXED;
	setScanType(scanType);
	QString info = "current scan type is: %1";
	info = info.arg(scanType);
	PRINT_LOG_INFO(info, INTERAL_LOG);

	//设置手持精细初始化参数

	//扫描模式 0 固定, 1 手持高清, 2 手持快速
	m_slnParms.iScanMode = m_scanType;
	//是否纹理
	m_slnParms.hasTexture = m_hasTexture;
	//点距
	m_slnParms.fPointDis = 0.5;
	//Feature ：0；Texture：1； Marker：2；GlobalMarker ：3；EncodeMarker ：4；Turntable ：5；
	m_slnParms.iAlignType = 0;
	//是否非刚体
	m_slnParms.useNonrigid = false;
	//0:新建空解决方案; 1:新建单个工程; 2: 新建解决方案
	m_slnParms.iNeedLimit = 2;

	onUpdateBtnStates(ProcessControl::MODE_CHOOSE);
}


void SDKDemoEinScanMainView::onRadioButtonHDClicked()
{
	ui->stackedWidget_2->setCurrentIndex(0);
	ScanType scanType = ST_HD;
	setScanType(scanType);
	QString info = "current scan type is: %1";
	info = info.arg(scanType);
	PRINT_LOG_INFO(info, INTERAL_LOG);

	//设置手持精细初始化参数

	//扫描模式 0 固定, 1 手持高清, 2 手持快速
	m_slnParms.iScanMode = m_scanType;
	//是否纹理
	m_slnParms.hasTexture = m_hasTexture;
	//点距
	m_slnParms.fPointDis = 0.5;
	//Feature ：0；Texture：1； Marker：2；GlobalMarker ：3；EncodeMarker ：4；Turntable ：5；
	m_slnParms.iAlignType = 0;
	//是否非刚体
	m_slnParms.useNonrigid = false;
	//0:新建空解决方案; 1:新建单个工程; 2: 新建解决方案
	m_slnParms.iNeedLimit = 2;

	onUpdateBtnStates(ProcessControl::MODE_CHOOSE);
}

void SDKDemoEinScanMainView::onRadioButtonRapidClicked()
{
	ui->stackedWidget_2->setCurrentIndex(0);
	ScanType scanType = ST_RAPID;
	setScanType(scanType);
	QString info = "current scan type is: %1";
	info = info.arg(scanType);
	PRINT_LOG_INFO(info, INTERAL_LOG);
	//设置手持快速初始化参数

	//扫描模式 0 固定, 1 手持高清, 2 手持快速
	m_slnParms.iScanMode = m_scanType;
	//是否纹理
	m_slnParms.hasTexture = m_hasTexture;
	//点距
	m_slnParms.fPointDis = 1.0;
	//Feature ：0；Texture：1； Marker：2；GlobalMarker ：3；EncodeMarker ：4；Turntable ：5；
	m_slnParms.iAlignType = 0;
	//是否非刚体
	m_slnParms.useNonrigid = false;
	//0:新建空解决方案; 1:新建单个工程; 2: 新建解决方案
	m_slnParms.iNeedLimit = 2;

	onUpdateBtnStates(ProcessControl::MODE_CHOOSE);

}

void SDKDemoEinScanMainView::onUpdatePrintLog(QString msg, int type)
{
	QColor color = Qt::white;
	switch (type) {
	case LogType::SEND_MESSAGE:
		color = Qt::blue;
		break;
	case LogType::RECEIVE_MESSAGE:
		color = Qt::green;
		break;
	case LogType::ERROR_MESSAGE:
		color = Qt::red;
		break;
	case LogType::INTERAL_LOG:
		color = Qt::black;
		break;
	}
	ui->logTextEdit->setTextColor(color);
	ui->logTextEdit->append(msg);
	QScrollBar *scrollBar = ui->logTextEdit->verticalScrollBar();
	scrollBar->setValue(scrollBar->maximum());
	VLOG(google::GLOG_INFO) << __func__ << msg.toStdString();
}

void SDKDemoEinScanMainView::onUpdateBtnStates(int process)
{
	setEnabledRecursive(this, false);
	switch (process)
	{
	case SDKDemoEinScanMainView::INIT:
		ui->connectDeviceBtn->setEnabled(true);
		ui->releaseBtn->setEnabled(true);
		break;
	case SDKDemoEinScanMainView::CONNECT_DEVICE:
		ui->releaseBtn->setEnabled(true);
		ui->radioButtonScanPage->setEnabled(true);
		break;
	case SDKDemoEinScanMainView::RECONNECT_DEVICE:
		ui->releaseBtn->setEnabled(true);
		ui->radioButtonScanPage->setEnabled(true);
		break;
	case SDKDemoEinScanMainView::RELEASE:
		ui->initializeBtn->setEnabled(true);
		ui->reConnectDeviceBtn->setEnabled(false);
		break;
	case SDKDemoEinScanMainView::MODE_CHOOSE:
		ui->radioButtonInitializePage->setEnabled(true);
		ui->radioButtonFixed->setEnabled(true);
		ui->radioButtonHD->setEnabled(true);
		ui->radioButtonRapid->setEnabled(true);
		if (m_scanType == ScanType::ST_FIXED) {
			ui->enterScanBtn_fixed->setEnabled(true);
		}
		else {
			ui->enterScanBtn->setEnabled(true);
		}
		break;
	case SDKDemoEinScanMainView::ENTER_SCAN:
		if (m_scanType == ScanType::ST_FIXED) {
			ui->exitScanBtn_fixed->setEnabled(true);
			ui->openSolutionBtn_fixed->setEnabled(true);
			ui->createSolutionBtn_fixed->setEnabled(true);
		}
		else {
			ui->exitScanBtn->setEnabled(true);
			ui->createSolutionBtn->setEnabled(true);
			ui->openSolutionBtn->setEnabled(true);
		}
		break;
	case SDKDemoEinScanMainView::EXIT_SCAN:
		ui->radioButtonInitializePage->setEnabled(true);
		ui->radioButtonFixed->setEnabled(true);
		ui->radioButtonHD->setEnabled(true);
		ui->radioButtonRapid->setEnabled(true);
		if (m_scanType == ScanType::ST_FIXED) {
			ui->enterScanBtn_fixed->setEnabled(true);
			ui->fixUuid->clear();
			ui->floatUuid->clear();
			m_projectInfoList.clear();
			m_flushProcess = -1;
		}
		else {
			ui->enterScanBtn->setEnabled(true);
		}
		break;
	case SDKDemoEinScanMainView::CREATE_SLN:
		ui->startProcessVideoData->setEnabled(true);
		ui->stopProcessVideoData->setEnabled(true);
		ui->brightnessSlider->setEnabled(true);
		if (m_scanType == ScanType::ST_FIXED) {
			ui->exitScanBtn_fixed->setEnabled(true);
			ui->maskBackground->setEnabled(true);
			ui->openSolutionBtn_fixed->setEnabled(true);
			ui->createSolutionBtn_fixed->setEnabled(true);
			ui->setScanParamBtn->setEnabled(true);
			ui->fixUuid->clear();
			ui->floatUuid->clear();
			m_projectInfoList.clear();
			m_flushProcess = -1;
		}
		else {
			ui->exitScanBtn->setEnabled(true);
			ui->createSolutionBtn->setEnabled(true);
			ui->openSolutionBtn->setEnabled(true);
			ui->previewScanBtn->setEnabled(true);
		}
		break;
	case SDKDemoEinScanMainView::OPEN_SLN:
		ui->startProcessVideoData->setEnabled(true);
		ui->stopProcessVideoData->setEnabled(true);
		ui->brightnessSlider->setEnabled(true);
		if (m_scanType == ScanType::ST_FIXED) {
			ui->exitScanBtn_fixed->setEnabled(true);
			ui->maskBackground->setEnabled(true);
			ui->openSolutionBtn_fixed->setEnabled(true);
			ui->createSolutionBtn_fixed->setEnabled(true);
			ui->setScanParamBtn->setEnabled(true);
		}
		else {
			ui->exitScanBtn->setEnabled(true);
			ui->createSolutionBtn->setEnabled(true);
			ui->openSolutionBtn->setEnabled(true);
			ui->previewScanBtn->setEnabled(true);
		}
		if (1 == ui->stackedWidget_2->currentIndex()) {// fix page
			foreach(QObject *child, ui->stackedWidget_2->currentWidget()->children()) {
				if (QPushButton *subChild = qobject_cast<QPushButton*>(child)) {
					if ("enterScanBtn_fixed" != subChild->objectName())
						subChild->setEnabled(true);
				}
				if (QComboBox *subChild = qobject_cast<QComboBox*>(child)) {
					subChild->setEnabled(true);
				}
				if (QLineEdit *subChild = qobject_cast<QLineEdit*>(child)) {
					subChild->setEnabled(true);
				}
			}
			ui->maskBackground->setEnabled(true);
		}
		else {// handle page
			foreach(QObject *child, ui->stackedWidget_2->currentWidget()->children()) {
				if (QPushButton *subChild = qobject_cast<QPushButton*>(child)) {
					if ("enterScanBtn" != subChild->objectName())
						subChild->setEnabled(true);
				}
			}
		}
		break;
	default:
		ui->startProcessVideoData->setEnabled(true);
		ui->stopProcessVideoData->setEnabled(true);
		ui->brightnessSlider->setEnabled(true);

		if (1 == ui->stackedWidget_2->currentIndex()) {// fix page
			foreach(QObject *child, ui->stackedWidget_2->currentWidget()->children()) {
				if (QPushButton *subChild = qobject_cast<QPushButton*>(child)) {
					if ("enterScanBtn_fixed" != subChild->objectName())
						subChild->setEnabled(true);
				}
				if (QComboBox *subChild = qobject_cast<QComboBox*>(child)) {
					subChild->setEnabled(true);
				}
				if (QLineEdit *subChild = qobject_cast<QLineEdit*>(child)) {
					subChild->setEnabled(true);
				}
			}
			ui->maskBackground->setEnabled(true);
		}
		else {// handle page
			foreach(QObject *child, ui->stackedWidget_2->currentWidget()->children()) {
				if (QPushButton *subChild = qobject_cast<QPushButton*>(child)) {
					if ("enterScanBtn" != subChild->objectName())
						subChild->setEnabled(true);
				}
			}
		}
		break;
	}
}

void SDKDemoEinScanMainView::onSetScanParamBtnClicked()
{
	//LOG_INFO_FUN_BEGIN;
	PRINT_LOG_INFO("Sn3DSetScanPars begin.", INTERAL_LOG);
	LPScanPars param = new ScanPars();

	QSettings settings(QCoreApplication::applicationDirPath() + "/fixedScanSetting.ini", QSettings::IniFormat);
	QString alignStr = settings.value("alignMod", "turntable").toString();
	QByteArray byteArray = alignStr.toUtf8();
	param->alignMod = byteArray.constData();
	param->scanPointCloud = settings.value("scanPointCloud", false).toBool();
	param->scanMarkers = settings.value("scanMarkers", false).toBool();
	param->addGlobalMarkers = settings.value("addGlobalMarkers", false).toBool();
	param->turtableChecked = settings.value("turntableChecked", true).toBool();
	m_turTableChecked = param->turtableChecked;
	param->turntableSteps = settings.value("turntableSteps", 8).toInt();
	param->turntableSpeed = settings.value("turntableSpeed", 6).toInt();
	param->isRoundChecked = settings.value("isRoundChecked", true).toBool();
	param->useExistVerifyData = settings.value("useExistVerifyData", false).toBool();
	param->HDR = settings.value("HDR", false).toBool();
	param->openScanEdit = settings.value("openScanEdit", false).toBool();

	VLOG(google::GLOG_INFO) << "alignMod:" << param->alignMod;
	VLOG(google::GLOG_INFO) << "scanPointCloud:" << param->scanPointCloud;
	VLOG(google::GLOG_INFO) << "scanMarkers:" << param->scanMarkers;
	VLOG(google::GLOG_INFO) << "addGlobalMarkers:" << param->addGlobalMarkers;
	VLOG(google::GLOG_INFO) << "turtableChecked:" << param->turtableChecked;
	VLOG(google::GLOG_INFO) << "turntableSteps:" << param->turntableSteps;
	VLOG(google::GLOG_INFO) << "turntableSpeed:" << param->turntableSpeed;
	VLOG(google::GLOG_INFO) << "isRoundChecked:" << param->isRoundChecked;
	VLOG(google::GLOG_INFO) << "useExistVerifyData:" << param->useExistVerifyData;
	VLOG(google::GLOG_INFO) << "HDR:" << param->HDR;
	VLOG(google::GLOG_INFO) << "openScanEdit:" << param->openScanEdit;
	int errCode = m_demoApp->Sn3DSetScanPars(param);
	if (errCode != SNSDK_ERR_OK) {
		PRINT_LOG_INFO(COV_QSTRING_EC("Sn3DSetScanPars failed.", errCode), ERROR_MESSAGE);
	}
	else
	{
		m_lastAlignMod = m_curAlignMod;
		m_curAlignMod = param->alignMod;
		if (m_lastAlignMod == "globalmarkers" && m_curAlignMod != "globalmarkers") {
			ui->fixUuid->clear();
			ui->floatUuid->clear();
			m_projectInfoList.clear();
			m_flushProcess = -1;
		}
		if (m_lastAlignMod != "globalmarkers" && m_curAlignMod == "globalmarkers") {
			ui->fixUuid->clear();
			ui->floatUuid->clear();
			m_projectInfoList.clear();
			m_flushProcess = -1;
		}
		PRINT_LOG_INFO(COV_QSTRING("Sn3DSetScanPars end errCode:", errCode), INTERAL_LOG);
	}
	delete param;
	param = nullptr;
	emit updateBtnStates(ProcessControl::SET_SCANPARS);
}

void SDKDemoEinScanMainView::onGlobalOptimizationBtnClicked()
{
	//LOG_INFO_FUN_BEGIN;
	PRINT_LOG_INFO("Sn3DGlobalOptimization begin.", INTERAL_LOG);

	QFuture<int> future = QtConcurrent::run([&]()->int {
		int errCode = m_demoApp->Sn3DGlobalOptimization();
		if (errCode != SNSDK_ERR_OK) {
			PRINT_LOG_INFO(COV_QSTRING_EC("Sn3DGlobalOptimization failed.", errCode), ERROR_MESSAGE);
		}
		else
		{
			//LOG_INFO_FUN_END;
			PRINT_LOG_INFO(COV_QSTRING("Sn3DGlobalOptimization end errCode:", errCode), INTERAL_LOG);
			return errCode;
		}
	});
	watcher.setFuture(future);
}

void SDKDemoEinScanMainView::setEnabledRecursive(QWidget *widget, bool enabled)
{
	//set button radio slider checkbox
	if (QPushButton *w = qobject_cast<QPushButton*>(widget)) {
		//不控制重连按钮
		if ("reConnectDeviceBtn" != w->objectName())
			w->setEnabled(enabled);
	}
	else if (QRadioButton *w = qobject_cast<QRadioButton*>(widget)) {
		w->setEnabled(enabled);
	}
	else if (QSlider *w = qobject_cast<QSlider*>(widget)) {
		w->setEnabled(enabled);
	}
	else if (QCheckBox *w = qobject_cast<QCheckBox*>(widget)) {
		w->setEnabled(enabled);
	}
	else if (QLineEdit *w = qobject_cast<QLineEdit*>(widget)) {
		w->setEnabled(enabled);
	}
	else if (QComboBox *w = qobject_cast<QComboBox*>(widget)) {
		w->setEnabled(enabled);
	}
	// 递归遍历子控件
	foreach(QObject *child, widget->children()) {
		if (QWidget *childWidget = qobject_cast<QWidget*>(child)) {
			setEnabledRecursive(childWidget, enabled);
		}
	}
}

void SDKDemoEinScanMainView::onPointFeatureBtnClicked()
{
	//LOG_INFO_FUN_BEGIN;
	PRINT_LOG_INFO("Sn3DPointFeatureAlign begin.", INTERAL_LOG);

	std::string selectedFixed = ui->fixUuid->currentText().toStdString();
	std::string selectedFloat = ui->floatUuid->currentText().toStdString();

	if (selectedFixed.size() == 0 || selectedFloat.size() == 0) {
		PRINT_LOG_INFO("Sn3DPointFeatureAlign failed, no uuid.", ERROR_MESSAGE);
		return;
	}
	QFuture<int> future = QtConcurrent::run([&]()->int {

		auto func = [&](const std::string& name)->std::string {
			QList<Sn3DProjectInfo>::const_iterator it;
			for (it = m_projectInfoList.begin(); it != m_projectInfoList.end(); ++it) {
				if (it->name == name)
					return it->dataID;
			}
		};

		auto func_rot = [&](float* rot)->QString {
			m_curRot = new float[9];
			QString res = "ROT: ";
			for (int idx = 0; idx < 9; idx++) {
				m_curRot[idx] = rot[idx];
				std::string prefix = std::to_string(rot[idx]);
				res = res + QString::fromStdString(prefix) + " ";
			}
			return res;
		};

		auto func_tran = [&](float* tran)->QString {
			m_curTran = new float[3];
			QString res = "TRAN: ";
			for (int idx = 0; idx < 3; idx++) {
				m_curTran[idx] = tran[idx];
				std::string prefix = std::to_string(tran[idx]);
				res = res + QString::fromStdString(prefix) + " ";
			}
			return res;
		};


		std::string selectedFixedText = ui->fixUuid->currentText().toStdString();
		std::string selectedFloatText = ui->floatUuid->currentText().toStdString();

		std::string fixKey = func(selectedFixedText);
		std::string floatKey = func(selectedFloatText);

		m_curFloatPrjName = selectedFloatText;

		LPPointFeatureAlign pointFeatureAlign = new PointFeatureAlign;
		pointFeatureAlign->alignType = AlignType::Point_Feature;
		pointFeatureAlign->fixKey = fixKey;
		pointFeatureAlign->floatKey = floatKey;
		pointFeatureAlign->isTurnTableScan = m_turTableChecked;

		float* curRot = new float[9];
		float* curTran = new float[3];
		memset(curRot, 0, sizeof(float) * 9);
		memset(curTran, 0, sizeof(float) * 3);
		int errCode = m_demoApp->Sn3DPointFeatureAlign(pointFeatureAlign, curRot, curTran);
		if (errCode == SNSDK_ERR_OK) {
			QString strInfoRot = func_rot(curRot);
			QString strInfoTran = func_tran(curTran);
			PRINT_LOG_INFO(strInfoRot, INTERAL_LOG);
			PRINT_LOG_INFO(strInfoTran, INTERAL_LOG);
			PRINT_LOG_INFO(COV_QSTRING("Sn3DPointFeatureAlign end errCode:", errCode), INTERAL_LOG);

			//initFloatRangeData();
			//readRangeData(QString::fromStdString(m_curFloatPrjName));
			//handleFloatRangeData(m_floatRangeData);
		}
		else {
			PRINT_LOG_INFO(COV_QSTRING_EC("Sn3DPointFeatureAlign falied.", errCode), ERROR_MESSAGE);
		}

		if (curRot) { delete[] curRot; curRot = nullptr; }
		if (curTran) { delete[] curTran; curTran = nullptr; }
		if (m_curRot) { delete[] m_curRot; m_curRot = nullptr; }
		if (m_curTran) { delete[] m_curTran; m_curTran = nullptr; }
		delete pointFeatureAlign;
		pointFeatureAlign = nullptr;
		return errCode;
	});

	watcher.setFuture(future);

	//LOG_INFO_FUN_END;
}

void SDKDemoEinScanMainView::onVFixPosBtnClicked()
{
	LOG_INFO_FUN_BEGIN;

	if (m_vfixPos)
		destructVPos(m_vfixPos);

	m_vfixPos = new Sn3dPointData[9];
	Sn3dPointData point;
	point.x = point.y = point.z = 0.0f;
	QString vfixPosInput = ui->vfixPos->text();
	if (vfixPosInput.endsWith(';'))
		vfixPosInput.chop(1);
	if (vfixPosInput == "")
		return;
	auto func = [&](QStringList data) {
		point.x = data[0].toFloat();
		point.y = data[1].toFloat();
		point.z = data[2].toFloat();
	};
	int idx = 0;
	QStringList coordinates = vfixPosInput.split(";");
	for (const QString& coordinate : coordinates) {
		QStringList parts = coordinate.split(",");
		func(parts);
		m_vfixPos[idx++] = point;
	}
	m_vfixPosCount = idx;

	LOG_INFO_FUN_END;
}

void SDKDemoEinScanMainView::onVFloatPosBtnClicked()
{
	LOG_INFO_FUN_BEGIN;

	if (m_vfloatPos)
		destructVPos(m_vfloatPos);

	m_vfloatPos = new Sn3dPointData[9];
	Sn3dPointData point;
	point.x = point.y = point.z = 0.0f;
	QString vfloatPosInput = ui->vfloatPos->text();
	if (vfloatPosInput == "")
		return;
	if (vfloatPosInput.endsWith(';'))
		vfloatPosInput.chop(1);
	auto func = [&](QStringList data) {
		point.x = data[0].toFloat();
		point.y = data[1].toFloat();
		point.z = data[2].toFloat();
	};
	int idx = 0;
	QStringList coordinates = vfloatPosInput.split(";");
	for (const QString& coordinate : coordinates) {
		QStringList parts = coordinate.split(",");
		func(parts);
		m_vfloatPos[idx++] = point;
	}
	m_vfloatPosCount = idx;

	LOG_INFO_FUN_END;
}

void SDKDemoEinScanMainView::onPointManualBtnClicked()
{
	//LOG_INFO_FUN_BEGIN;
	PRINT_LOG_INFO("Sn3DPointManualAlign begin.", INTERAL_LOG);

	std::string selectedFixed = ui->fixUuid->currentText().toStdString();
	std::string selectedFloat = ui->floatUuid->currentText().toStdString();

	if (selectedFixed.size() == 0 || selectedFloat.size() == 0) {
		PRINT_LOG_INFO("Sn3DPointManualAlign failed, no uuid.", ERROR_MESSAGE);
		return;
	}

	QFuture<int> future = QtConcurrent::run([&]()->int {
		auto func = [&](const std::string& name)->std::string {
			QList<Sn3DProjectInfo>::const_iterator it;
			for (it = m_projectInfoList.begin(); it != m_projectInfoList.end(); ++it) {
				if (it->name == name)
					return it->dataID;
			}
		};

		auto func_rot = [&](float* rot)->QString {
			m_curRot = new float[9];
			QString res = "ROT: ";
			for (int idx = 0; idx < 9; idx++) {
				m_curRot[idx] = rot[idx];
				std::string prefix = std::to_string(rot[idx]);
				res = res + QString::fromStdString(prefix) + " ";
			}
			return res;
		};

		auto func_tran = [&](float* tran)->QString {
			m_curTran = new float[3];
			QString res = "TRAN: ";
			for (int idx = 0; idx < 3; idx++) {
				m_curTran[idx] = tran[idx];
				std::string prefix = std::to_string(tran[idx]);
				res = res + QString::fromStdString(prefix) + " ";
			}
			return res;
		};

		std::string selectedFixedText = ui->fixUuid->currentText().toStdString();
		std::string selectedFloatText = ui->floatUuid->currentText().toStdString();

		std::string fixKey = func(selectedFixedText);
		std::string floatKey = func(selectedFloatText);

		m_curFloatPrjName = selectedFloatText;

		LPPointManualAlign pointManualAlign = new PointManualAlign;
		pointManualAlign->alignType = AlignType::Point_Manual;
		pointManualAlign->fixKey = fixKey;
		pointManualAlign->floatKey = floatKey;
		pointManualAlign->isTurnTableScan = m_turTableChecked;
		pointManualAlign->v_fixPos = new Sn3dPointData[m_vfixPosCount];
		pointManualAlign->v_floatPos = new Sn3dPointData[m_vfloatPosCount];
		for (int i = 0; i < m_vfixPosCount; i++) {
			pointManualAlign->v_fixPos[i].x = m_vfixPos[i].x;
			pointManualAlign->v_fixPos[i].y = m_vfixPos[i].y;
			pointManualAlign->v_fixPos[i].z = m_vfixPos[i].z;
		}
		for (int i = 0; i < m_vfloatPosCount; i++) {
			pointManualAlign->v_floatPos[i].x = m_vfloatPos[i].x;
			pointManualAlign->v_floatPos[i].y = m_vfloatPos[i].y;
			pointManualAlign->v_floatPos[i].z = m_vfloatPos[i].z;
		}
		pointManualAlign->v_fixPosCount = m_vfixPosCount;
		pointManualAlign->v_floatPosCount = m_vfloatPosCount;

		float* rot = new float[9];
		float* tran = new float[3];
		memset(rot, 0, sizeof(float) * 9);
		memset(tran, 0, sizeof(float) * 3);
		int errCode = m_demoApp->Sn3DPointManualAlign(pointManualAlign, rot, tran);
		if (errCode == SNSDK_ERR_OK) {
			QString strInfoRot = func_rot(rot);
			QString strInfoTran = func_tran(tran);
			PRINT_LOG_INFO(strInfoRot, INTERAL_LOG);
			PRINT_LOG_INFO(strInfoTran, INTERAL_LOG);
			PRINT_LOG_INFO(COV_QSTRING("Sn3DPointManualAlign end errCode:", errCode), INTERAL_LOG);

			//initFloatRangeData();
			//readRangeData(QString::fromStdString(m_curFloatPrjName));
			//handleFloatRangeData(m_floatRangeData);
		}
		else {
			PRINT_LOG_INFO(COV_QSTRING_EC("Sn3DPointManualAlign falied.", errCode), ERROR_MESSAGE);
		}
		if (rot) { delete[] rot; rot = nullptr; }
		if (tran) { delete[] tran; tran = nullptr; }
		if (m_curRot) { delete[] m_curRot; m_curRot = nullptr; }
		if (m_curTran) { delete[] m_curTran; m_curTran = nullptr; }
		if (pointManualAlign) {
			if (pointManualAlign->v_fixPos) { delete[] pointManualAlign->v_fixPos; pointManualAlign->v_fixPos = nullptr; }
			if (pointManualAlign->v_floatPos) { delete[] pointManualAlign->v_floatPos; pointManualAlign->v_floatPos = nullptr; }
			pointManualAlign = nullptr;
		}
		return errCode;
	});

	watcher.setFuture(future);

	//LOG_INFO_FUN_END;

}
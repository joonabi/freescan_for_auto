#include "sn3dsdkoptimscanmainview.h"
#include "ui_sn3dsdkoptimscanmainview.h"
#include "glog/logging.h"
#include <QSettings>

void CALLBACK SDKWrapperOptimVideoCallback(int camId, const unsigned char * data, int width, int height, int channel, void* pUserData)
{
	Sn3DSDKOptimScanMainView* pWrapper = (Sn3DSDKOptimScanMainView*)pUserData;
	if (pWrapper)
		pWrapper->videoCallBack(camId, data, width, height, channel);
}

Sn3DSDKOptimScanMainView::Sn3DSDKOptimScanMainView(QWidget *parent) :
    Sn3DSDKMainViewBase(parent),
    ui(new Ui::Sn3DSDKOptimScanMainView)
{
    ui->setupUi(this);
	m_demoApp = static_cast<Sn3DSDKDemoApp *>qApp;
	//ui->scanPanel->setVisible(false);

	connect(this, &Sn3DSDKOptimScanMainView::sigGetVideoData, this, [this](int camId, const unsigned char * data, int width, int height, int channel) 
	{
		QImage image(data, width, height, QImage::Format_RGB888);
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
		default:
			break;
		}
	}, Qt::AutoConnection);

	connect(m_demoApp, &Sn3DSDKDemoApp::deviceStatusChanged, this, [this](LPDeviceStatus status) // sn3dsdk_c_wrapper::DeviceStatus
	{ 
		m_deviceStatus = status->offline;
		QString strInfo = "device online";
		if (SNSDK_DEVSTAT_OFFLINE == status->offline)
				strInfo = "device offline";
		else 
		{
			m_demoApp->installGetImagasCallBack(SDKWrapperOptimVideoCallback, this);
			QThread::msleep(200);
			m_demoApp->videoProcessControl(true);
		}
		this->printLog(strInfo, LogType::RECEIVE_MESSAGE);
    }, Qt::QueuedConnection);


	connect(ui->scanPanel, &OptimScanTabWidget::importFrameworkMakers, [&](QString filePath)
	{
		if (!filePath.isEmpty()) {
			QString fPath = filePath;
			QFuture<int> future = QtConcurrent::run([&]()->int {
				return m_demoApp->Sn3DImproFramePoint(fPath);
			});

			watcher.setFuture(future);
		}
	});

	QObject::connect(&watcher, &QFutureWatcher<int>::finished, [&]() {
		PRINT_LOG("call func finished", watcher.result());
	});

    connect(m_demoApp, &Sn3DSDKDemoApp::showMsg, this, &Sn3DSDKOptimScanMainView::onShowMsg, Qt::QueuedConnection);
}


Sn3DSDKOptimScanMainView::~Sn3DSDKOptimScanMainView()
{
    on_socketReleaseBtn_pressed();
    delete ui;
}

void Sn3DSDKOptimScanMainView::printLog(QString msg, LogType type)
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
}


void Sn3DSDKOptimScanMainView::videoCallBack(int camId, const unsigned char * data, int width, int height, int channel)
{
	emit sigGetVideoData(camId, data, width, height, channel);
}

//打开服务
void Sn3DSDKOptimScanMainView::on_openServiceBtn_pressed()
{
	LOG_INFO_FUN_BEGIN;

	QFuture<int> future =  QtConcurrent::run([&]()->int {
		return m_demoApp->Sn3DInitialize(deviceType());
	});

	watcher.setFuture(future);

	LOG_INFO_FUN_END;
}

//新建工程
void Sn3DSDKOptimScanMainView::on_newPrjBtn_pressed()
{
	LOG_INFO_FUN_BEGIN;

	QString slnPath = QFileDialog::getExistingDirectory(this, "new", QDir::homePath());
	if (!slnPath.isEmpty()) {
		QFuture<int> future = QtConcurrent::run([&]()->int {
			return m_demoApp->Sn3DOpenOrCreateSolution(slnPath, true, 1, false, 0.1f);
		});

		watcher.setFuture(future);
	}

	LOG_INFO_FUN_END;
}

//打开工程
void Sn3DSDKOptimScanMainView::on_openPrjBtn_pressed()
{
	LOG_INFO_FUN_BEGIN;

	QString slnPath = QFileDialog::getOpenFileName(this, "open", QDir::homePath(), "*.sln_fix_optim");
	if (!slnPath.isEmpty()) {
		QFuture<int> future = QtConcurrent::run([&]()->int {
			return m_demoApp->Sn3DOpenOrCreateSolution(slnPath, false, 1, false, 0.1f);
		});

		watcher.setFuture(future);
	}

	LOG_INFO_FUN_END;
}

//释放
void Sn3DSDKOptimScanMainView::on_releaseBtn_pressed()
{
	LOG_INFO_FUN_BEGIN;
	m_demoApp->videoProcessControl(false);
	int errCode = m_demoApp->Sn3DRelease();
	PRINT_LOG("Sn3DRelease", errCode);

	LOG_INFO_FUN_END;
}

//开始扫描
void Sn3DSDKOptimScanMainView::on_scanBtn_pressed()
{
	LOG_INFO_FUN_BEGIN;

	if (m_deviceStatus == SNSDK_DEVSTAT_OFFLINE)
		return;

	QFuture<int> future = QtConcurrent::run([&]()->int {
		//LPScanPars param = new ScanPars();
		//ui->scanPanel->getScanSettingData(param);
		//int errCode = m_demoApp->Sn3DSetScanPars(param);
		//delete param;
		//param = nullptr;
		//if (errCode == SNSDK_ERR_OK)
		//	return m_demoApp->Sn3DStartScan();
		//else
		//	return errCode;
			return m_demoApp->Sn3DStartScan();
	});

	watcher.setFuture(future);

	LOG_INFO_FUN_END;
}

//全局优化
void Sn3DSDKOptimScanMainView::on_optimBtn_pressed()
{
	LOG_INFO_FUN_BEGIN;

	QFuture<int> future = QtConcurrent::run([&]()->int {
		return m_demoApp->Sn3DGlobalOptimization();
	});

	watcher.setFuture(future);

	LOG_INFO_FUN_END;
}

//保存
void Sn3DSDKOptimScanMainView::on_saveBtn_pressed()
{
	LOG_INFO_FUN_BEGIN;

	static QString savePath = QFileDialog::getSaveFileName(this, "save", QDir::homePath(), "*.asc;;*.p3;;*.stl");
	if (!savePath.isEmpty()) {

		QFileInfo fileInfo(savePath);
		QFuture<int> future = QtConcurrent::run([&]()->int {
			return m_demoApp->Sn3DSaveData(fileInfo.baseName(), fileInfo.path(), fileInfo.suffix());
		});

		watcher.setFuture(future);
	}

	LOG_INFO_FUN_END;
}

//封装
void Sn3DSDKOptimScanMainView::on_meshBtn_pressed()
{
	LOG_INFO_FUN_BEGIN;

	QFuture<int> future = QtConcurrent::run([&]()->int {
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
		delete meshPars;
		meshPars = nullptr;
		return errCode;
	});
	watcher.setFuture(future);

	LOG_INFO_FUN_END;
}

void Sn3DSDKOptimScanMainView::on_snapImgBtn_pressed()
{
	LOG_INFO_FUN_BEGIN;

	QFuture<int> future = QtConcurrent::run([&]()->int {
		return m_demoApp->Sn3DCaliSnapImage();
	});

	watcher.setFuture(future);

	LOG_INFO_FUN_END;
}


void Sn3DSDKOptimScanMainView::on_CaleBtn_pressed()
{
	LOG_INFO_FUN_BEGIN;

	QFuture<int> future = QtConcurrent::run([&]()->int {
		return m_demoApp->Sn3DCaliCalculate();
	});

	watcher.setFuture(future);

	LOG_INFO_FUN_END;
}

void Sn3DSDKOptimScanMainView::on_socketInitialBtn_pressed()
{
    LOG_INFO_FUN_BEGIN;
    if (m_demoApp) {
       int ret = m_demoApp->Sn3DInitializeSocket(deviceType());
       if (SNSDK_ERR_OK == ret)
       {
           ui->socketReleaseBtn->setEnabled(true);
           ui->socketConnectBtn->setEnabled(true);
           ui->socketInitialBtn->setEnabled(false);
       }
    }
    LOG_INFO_FUN_END;
}

void Sn3DSDKOptimScanMainView::on_socketReleaseBtn_pressed()
{
    LOG_INFO_FUN_BEGIN;
    if (m_demoApp) {
        int ret = m_demoApp->Sn3DRecleaseSocket();
        if (SNSDK_ERR_OK == ret) {
            ui->socketReleaseBtn->setEnabled(false);
            ui->socketConnectBtn->setEnabled(false);
            ui->disconnectBtn->setEnabled(false);
            ui->socketInitialBtn->setEnabled(true);
        }
      
    }
    LOG_INFO_FUN_END;
}

void Sn3DSDKOptimScanMainView::on_socketConnectBtn_pressed()
{
    LOG_INFO_FUN_BEGIN;
    if (m_demoApp) {
       int ret = m_demoApp->Sn3DConnectSocket(ui->sockeIPLEdit->text(), ui->socketPortLEdit->text().toInt());
       if (SNSDK_ERR_OK == ret) {
           ui->disconnectBtn->setEnabled(true);
           ui->socketConnectBtn->setEnabled(false);
       }
    }
    LOG_INFO_FUN_END;
}

void Sn3DSDKOptimScanMainView::on_disconnectBtn_pressed()
{
    LOG_INFO_FUN_BEGIN;
    if (m_demoApp) {
        int ret = m_demoApp->Sn3DDisconnectSocket();
        if (SNSDK_ERR_OK == ret) {
            ui->socketConnectBtn->setEnabled(true);
            ui->disconnectBtn->setEnabled(false);
        }
    }
    LOG_INFO_FUN_END;
}

void Sn3DSDKOptimScanMainView::on_sendBtn_pressed()
{
    LOG_INFO_FUN_BEGIN;
    if (m_demoApp) {
        QString msg = ui->socketSendTEdit->toPlainText();
        onShowMsg(msg, Sn3DSDKOptimScanMainView::SEND_MESSAGE);
        m_demoApp->Sn3DSendSocket(msg);
    }
    LOG_INFO_FUN_END;

}


void Sn3DSDKOptimScanMainView::onShowMsg(QString msg, int type)
{
	//这个主要是让demo界面按钮状态 无其他实际作用
	if (msg.contains("releaseFinish"))
		on_socketReleaseBtn_pressed();

    QDateTime dateTime = QDateTime::currentDateTime();
    QString flagStr = " ";
    switch (type)
    {
    case Sn3DSDKOptimScanMainView::SEND_MESSAGE: {
        QString timeStr =QString( "[" + dateTime.toString("yyyy-MM-dd hh:mm:ss:zzz") + "] Send :%1b >>>").arg(msg.size());
        ui->socketReceiveTEdit->setTextColor(Qt::blue);
        ui->socketReceiveTEdit->append(timeStr);
        ui->socketReceiveTEdit->append(msg);
    }
        break;
    case Sn3DSDKOptimScanMainView::RECEIVE_MESSAGE: {
        QString timeStr = QString("[" + dateTime.toString("yyyy-MM-dd hh:mm:ss:zzz") + "] Recv :%1b <<< ").arg(msg.size());
        ui->socketReceiveTEdit->setTextColor(QColor("#009b09"));
        ui->socketReceiveTEdit->append(timeStr);
        ui->socketReceiveTEdit->append(msg);
    }
        break;
    case Sn3DSDKOptimScanMainView::ERROR_MESSAGE:
        break;
    case Sn3DSDKOptimScanMainView::INTERAL_LOG: {
        QString timeStr = QString("[" + dateTime.toString("yyyy-MM-dd hh:mm:ss:zzz") + "] ");
        ui->socketReceiveTEdit->setTextColor(Qt::black);
        ui->socketReceiveTEdit->append(timeStr + msg);
    }
        break;
    default:
        break;
    }

}

void Sn3DSDKOptimScanMainView::on_enterCaliBtn_pressed()
{
    LOG_INFO_FUN_BEGIN;

    QFuture<int> future = QtConcurrent::run([&]()->int {
        return m_demoApp->Sn3DEnterCali();
    });

    watcher.setFuture(future);

    LOG_INFO_FUN_END;
}


void Sn3DSDKOptimScanMainView::on_pushButton_pressed()
{
    LOG_INFO_FUN_BEGIN;

    QFuture<int> future = QtConcurrent::run([&]()->int {
        return m_demoApp->reconnectDevice();
    });

    watcher.setFuture(future);

    LOG_INFO_FUN_END;
}


void Sn3DSDKOptimScanMainView::on_brightnessSlider_valueChanged(int value)
{
    LOG_INFO_FUN_BEGIN;

	m_demoApp->setBrightness(value);

    LOG_INFO_FUN_END;
}


void Sn3DSDKOptimScanMainView::on_backGroundSlider_valueChanged(int value)
{
	LOG_INFO_FUN_BEGIN;

	m_demoApp->setMaskBackGround(value, true);

	LOG_INFO_FUN_END;
}


//void Sn3DSDKOptimScanMainView::on_imporFrameMarkerBtn_pressed()
//{
//    LOG_INFO_FUN_BEGIN;
//
//	QString filePath = QFileDialog::getOpenFileName(this, "open", QDir::homePath(), "*.p3;;*.txt;;*.asc;;");
//	if (!filePath.isEmpty()) {
//		QFuture<int> future = QtConcurrent::run([&]()->int {
//			return m_demoApp->Sn3DImproFramePoint(filePath);
//		});
//
//		watcher.setFuture(future);
//	}
//
//    LOG_INFO_FUN_END;
//}


void Sn3DSDKOptimScanMainView::on_setBtn_pressed()
{
	LOG_INFO_FUN_BEGIN;

	if (m_deviceStatus == SNSDK_DEVSTAT_OFFLINE)
		return;

	QFuture<int> future = QtConcurrent::run([&]()->int {
		LPScanPars param = new ScanPars();
		ui->scanPanel->getScanSettingData(param);
		VLOG(google::GLOG_INFO) << "scanPointCloud:" << param->scanPointCloud;
		VLOG(google::GLOG_INFO) << "scanMarkers:" << param->scanMarkers;
		VLOG(google::GLOG_INFO) << "addGlobalMarkers:" << param->addGlobalMarkers;
		VLOG(google::GLOG_INFO) << "alignMod:" << param->alignMod;
		VLOG(google::GLOG_INFO) << "HDR:" << param->HDR;
		VLOG(google::GLOG_INFO) << "turtableChecked:" << param->turtableChecked;
		VLOG(google::GLOG_INFO) << "turntableAlignMod:" << param->turntableAlignMod;
		VLOG(google::GLOG_INFO) << "turntableSteps:" << param->turntableSteps;
		VLOG(google::GLOG_INFO) << "turntableSpeed:" << param->turntableSpeed;
		VLOG(google::GLOG_INFO) << "isRoundChecked:" << param->isRoundChecked;
		VLOG(google::GLOG_INFO) << "useExistVerifyData:" << param->useExistVerifyData;
		VLOG(google::GLOG_INFO) << "openScanEdit:" << param->openScanEdit;
		int errCode = m_demoApp->Sn3DSetScanPars(param);

		delete[] param->alignMod;
		param->alignMod = nullptr;
		delete param;
		param = nullptr;
		return errCode;
	});

	watcher.setFuture(future);

	LOG_INFO_FUN_END;
}


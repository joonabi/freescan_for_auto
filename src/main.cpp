#include "SDKDemoOperator/OperatorMainView.h"
#include "SDKDemoOperator/PresetManager.h"
#include "glog/logging.h"

#include "sn3dsdkdemoapp.h"
#include "windows.h"
#include "sn3dsdkcorepub.h"
#include <QDateTime>
#include <QDir>
#include <QPixmap>
#include <QSplashScreen>
#include <QThread>

int main(int argc, char *argv[])
{
    qRegisterMetaType<LPSn3dCameraPosition>("LPSn3dCameraPosition");
    qRegisterMetaType<DeviceEvent>("DeviceEvent");
    qRegisterMetaType<LPSn3dPointData>("LPSn3dPointData");
    qRegisterMetaType<LPDeviceStatus>("LPDeviceStatus");
    qRegisterMetaType<LPSn3dRangeData>("LPSn3dRangeData");
    qRegisterMetaType<LPSn3dProjectInfo>("LPSn3dProjectInfo");

    Sn3DSDKDemoApp a(argc, argv);

    // ── 로그 초기화 ──────────────────────────────────────────
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::GLOG_INFO, "log_sdk\\Sn3DSDKDemo");
    QString sdkLogDir = QCoreApplication::applicationDirPath()
                        + QDir::separator() + "log_sdk";
    QDir dir;
    if (!dir.exists(sdkLogDir))
        dir.mkpath(sdkLogDir);

    QDateTime dateTime = QDateTime::currentDateTime();
    QString timeStr = dateTime.toString("yyyyMMdd_hhmmss_zzz");
    google::SetLogFilenameExtension(
        QString("_" + timeStr + ".log").toStdString().c_str());
    FLAGS_stop_logging_if_full_disk = true;
    FLAGS_logbufsecs   = 0;
    FLAGS_v            = 99;
    FLAGS_timestamp_in_logfile_name = false;
    VLOG(google::GLOG_INFO) << "Last Build Date: " << __DATE__ << " " << __TIME__;

    // ── 스플래시 화면 ─────────────────────────────────────────
    QPixmap splashPix(":/images/company_logo.png");
    QSplashScreen* splash = nullptr;
    if (!splashPix.isNull()) {
        splash = new QSplashScreen(
            splashPix.scaled(600, 300, Qt::KeepAspectRatio,
                             Qt::SmoothTransformation));
        splash->show();
        a.processEvents();
        QThread::sleep(2);   // 2초 표시
    }

    // ── 프리셋 로드 ───────────────────────────────────────────
    // 파일이 없으면 기본 프리셋 1개가 이미 메모리에 있음
    PresetManager::instance().load();

    // ── 메인 창 표시 ─────────────────────────────────────────
    OperatorMainView w;
    w.show();

    if (splash) {
        splash->finish(&w);
        delete splash;
    }

    return a.exec();
}

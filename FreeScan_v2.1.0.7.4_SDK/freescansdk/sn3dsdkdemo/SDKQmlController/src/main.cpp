#include "FreeScanController.h"
#include "DataManager.h"
#include "sn3dsdkdemoapp.h"

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QSurfaceFormat>
#include <QIcon>
#include <QFont>

int main(int argc, char* argv[])
{
    // HiDPI 지원
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    // Sn3DSDKDemoApp: QApplication 서브클래스 (SDK 콜백 포함)
    Sn3DSDKDemoApp app(argc, argv);
    app.setOrganizationName("SND");
    app.setApplicationName("3D FOR AUTOMATION");
    app.setApplicationVersion("2.1.0");

    // 기본 폰트 (Malgun Gothic)
    QFont font("Malgun Gothic");
    font.setPixelSize(16);
    app.setFont(font);

    // Qt Quick Controls 2: Material Dark 테마
    QQuickStyle::setStyle("Material");

    // 컨텍스트 객체 생성
    FreeScanController* scannerController = new FreeScanController(&app);
    DataManager*        dataManager       = new DataManager(&app);

    // QML 엔진 생성 및 컨텍스트 프로퍼티 등록
    QQmlApplicationEngine engine;

    // QRC 내 qmldir 기반 singleton/module 로드를 위한 import path 추가
    engine.addImportPath(QStringLiteral("qrc:/"));

    QQmlContext* ctx = engine.rootContext();
    ctx->setContextProperty("scannerController", scannerController);
    ctx->setContextProperty("dataManager",       dataManager);

    // QML 로드
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

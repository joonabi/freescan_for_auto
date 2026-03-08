import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import "components"
import "styles"

ApplicationWindow {
    id: root
    visible: true
    width: 1920
    height: 1080
    minimumWidth: 1024
    minimumHeight: 600
    title: "3D FOR AUTOMATION"

    // Material 테마 설정
    Material.theme: Material.Dark
    Material.accent: Material.Blue

    // 전역 상태
    property string currentStep: "workplace"
    property string clientName: {
        var s = dataManager.loadSettings()
        return s.clientName || "J3D"
    }
    property string clientLogo: ""
    property bool emergencyStop: false

    // 설정 저장 이벤트 처리
    function handleClientChange(name, logo) {
        clientName = name
        clientLogo = logo || ""
        var s = dataManager.loadSettings()
        s.clientName = name
        dataManager.saveSettings(s)
    }

    function handleEmergencyStop() {
        emergencyStop = true
        scannerController.stopScan()
        console.log("긴급 정지! 모든 장비 동작이 중지되었습니다.")
    }

    function handleResume() {
        emergencyStop = false
        console.log("시스템이 재시작되었습니다.")
    }

    // 배경색
    color: Colors.bgPrimary

    // 메인 레이아웃
    RowLayout {
        anchors.fill: parent
        spacing: 0

        // 좌측 사이드바
        Sidebar {
            id: sidebar
            Layout.preferredWidth: {
                if (root.width < 768) return 192
                if (root.width < 1024) return 256
                return 320
            }
            Layout.fillHeight: true

            currentStep: root.currentStep
            emergencyStop: root.emergencyStop

            onStepChanged: function(step) {
                root.currentStep = step
            }

            onEmergencyStopClicked: {
                root.handleEmergencyStop()
            }

            onResumeClicked: {
                root.handleResume()
            }
        }

        // 우측 컨텐츠 영역
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // 헤더
            Header {
                id: header
                Layout.fillWidth: true
                Layout.preferredHeight: {
                    if (root.width < 768) return 80
                    return 100
                }

                clientName: root.clientName
                clientLogo: root.clientLogo
                emergencyStop: root.emergencyStop

                onEmergencyStopClicked: {
                    root.handleEmergencyStop()
                }

                onResumeClicked: {
                    root.handleResume()
                }
            }

            // 메인 컨텐츠 (동적 로드)
            Loader {
                id: contentLoader
                Layout.fillWidth: true
                Layout.fillHeight: true

                source: {
                    switch(root.currentStep) {
                        case "workplace": return "pages/WorkplaceManagement.qml"
                        case "prepare":   return "pages/PreparationStep.qml"
                        case "start":     return "pages/ScanStartStep.qml"
                        case "settings":  return "pages/SettingsStep.qml"
                        case "logs":      return "pages/LogViewer.qml"
                        case "manual":    return "pages/ManualStep.qml"
                        default:          return "pages/WorkplaceManagement.qml"
                    }
                }

                onLoaded: {
                    if (item) {
                        item.emergencyStop = Qt.binding(function() {
                            return root.emergencyStop
                        })

                        if (root.currentStep === "workplace" && item.onClientChange) {
                            item.onClientChange.connect(root.handleClientChange)
                        }
                    }
                }
            }
        }
    }

    // 로딩 오버레이
    Rectangle {
        id: loadingOverlay
        anchors.fill: parent
        color: "#80000000"
        visible: false
        z: 999

        BusyIndicator {
            anchors.centerIn: parent
            running: parent.visible
            width: 100
            height: 100
            Material.accent: Material.Blue
        }
    }
}

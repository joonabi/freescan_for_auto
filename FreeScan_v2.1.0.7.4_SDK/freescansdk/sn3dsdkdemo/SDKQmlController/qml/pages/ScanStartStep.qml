import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../components"
import "../styles"

Item {
    id: root
    width: parent ? parent.width : 0
    height: parent ? parent.height : 0

    property bool emergencyStop: false

    // 작업장 정보 (DataManager에서 로드)
    property var sysSettings: dataManager.loadSettings()
    property string workspace: (sysSettings.selectedLocation || "1공장") + "_" +
                               (sysSettings.selectedTask || "외관검사") + "_" +
                               (sysSettings.selectedWorker || "작업자")

    // 스캔 설정 (DataManager에서 로드)
    property var scanSettings: dataManager.loadScanSettings()

    // 스캔 시작 함수
    function doStartScan() {
        var settings = dataManager.loadScanSettings()
        var sysSett  = dataManager.loadSettings()

        // FreeScanController.startScan(preset) 호출
        scannerController.startScan({
            "saveDirectory": settings.saveDirectory || "C:/3DScan/Results",
            "workspace":     (sysSett.selectedLocation || "1공장") + "_" +
                             (sysSett.selectedTask     || "외관검사") + "_" +
                             (sysSett.selectedWorker   || "작업자"),
            "lightSource":   settings.lightSource  || "Laser",
            "scanMode":      settings.scanMode     || "mesh",
            "laserLine":     settings.laserLine    || "cross",
            "material":      settings.material     || "normal",
            "filterLevel":   settings.filterLevel  || "2",
            "smoothLevel":   settings.smoothLevel  || "1",
            "fillMarkerHoles": settings.fillMarkerHoles !== undefined ? settings.fillMarkerHoles : true,
            "fillSmallHoles":  settings.fillSmallHoles  !== undefined ? settings.fillSmallHoles  : true,
            "removeAnomalies": settings.removeAnomalies !== undefined ? settings.removeAnomalies : true,
            "removeSpikes":    settings.removeSpikes    !== undefined ? settings.removeSpikes    : true,
            "fileFormat":    settings.fileFormat   || "stl"
        })
    }

    ScrollView {
        anchors.fill: parent
        clip: true

        ColumnLayout {
            width: parent.parent.width
            spacing: 32
            padding: 32

            // 제목
            Text {
                text: "AUTO 3D스캔 시작"
                color: Colors.textPrimary
                font.pixelSize: 32
                font.bold: true
            }

            // 2열 그리드
            GridLayout {
                Layout.fillWidth: true
                columns: parent.width > 1200 ? 2 : 1
                columnSpacing: 32
                rowSpacing: 32

                // === 좌측: 스캔 진행 상황 ===
                Rectangle {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 700
                    color: Colors.bgCard
                    radius: 16

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 32
                        spacing: 24

                        Text {
                            text: "스캔 진행 상황"
                            color: Colors.textPrimary
                            font.pixelSize: 24
                            font.bold: true
                        }

                        // 진행률 표시
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 16
                            visible: scannerController.isScanning || scannerController.status === "completed"

                            RowLayout {
                                Layout.fillWidth: true

                                Text {
                                    text: "진행 상태"
                                    color: Colors.textPrimary
                                    font.pixelSize: 20
                                    font.bold: true
                                }

                                Item { Layout.fillWidth: true }

                                Text {
                                    text: scannerController.progress + "%"
                                    color: Colors.info
                                    font.pixelSize: 36
                                    font.bold: true
                                }
                            }

                            // 프로그레스 바
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 32
                                color: Colors.bgTertiary
                                radius: 16

                                Rectangle {
                                    width: parent.width * (scannerController.progress / 100)
                                    height: parent.height
                                    radius: parent.radius

                                    gradient: Gradient {
                                        orientation: Gradient.Horizontal
                                        GradientStop {
                                            position: 0.0
                                            color: {
                                                if (scannerController.status === "completed") return Colors.gradientGreenStart
                                                if (scannerController.status === "paused")    return Colors.gradientYellowStart
                                                return Colors.gradientBlueStart
                                            }
                                        }
                                        GradientStop {
                                            position: 1.0
                                            color: {
                                                if (scannerController.status === "completed") return Colors.gradientGreenEnd
                                                if (scannerController.status === "paused")    return Colors.gradientYellowEnd
                                                return Colors.gradientBlueEnd
                                            }
                                        }
                                    }

                                    Behavior on width { NumberAnimation { duration: 300 } }
                                }
                            }

                            // 현재 단계
                            Text {
                                Layout.fillWidth: true
                                text: scannerController.currentStep
                                color: Colors.textSecondary
                                font.pixelSize: 18
                                wrapMode: Text.WordWrap
                            }
                        }

                        // 스캔 대기 메시지 (실제 스캐너 모드: 사용자가 FreeScan에서 직접 제어)
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 120
                            visible: scannerController.isScanning && scannerController.progress === 0
                            color: Qt.rgba(Colors.info.r, Colors.info.g, Colors.info.b, 0.1)
                            radius: 12
                            border.color: Colors.info
                            border.width: 2

                            ColumnLayout {
                                anchors.centerIn: parent
                                spacing: 8

                                Text {
                                    Layout.alignment: Qt.AlignHCenter
                                    text: "스캐너에서 스캔을 시작하세요"
                                    color: Colors.textPrimary
                                    font.pixelSize: 20
                                    font.bold: true
                                }

                                Text {
                                    Layout.alignment: Qt.AlignHCenter
                                    text: "FreeScan 화면에서 스캔 버튼을 누르면 자동으로 처리됩니다"
                                    color: Colors.textTertiary
                                    font.pixelSize: 15
                                }
                            }
                        }

                        // 시간 정보
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 120
                            visible: scannerController.isScanning || scannerController.status === "completed"
                            color: Colors.bgTertiary
                            radius: 12

                            GridLayout {
                                anchors.fill: parent
                                anchors.margins: 20
                                columns: 2
                                rowSpacing: 12
                                columnSpacing: 20

                                Text { text: "시작 시간:"; color: Colors.textTertiary; font.pixelSize: 16 }
                                Text {
                                    text: scannerController.startTime || "-"
                                    color: Colors.textPrimary
                                    font.pixelSize: 18
                                    font.bold: true
                                }

                                Text { text: "총 소요시간:"; color: Colors.textTertiary; font.pixelSize: 16 }
                                Text {
                                    text: scannerController.formatElapsedTime()
                                    color: Colors.textPrimary
                                    font.pixelSize: 18
                                    font.bold: true

                                    // 1초마다 갱신
                                    Timer {
                                        interval: 1000
                                        running: scannerController.isScanning
                                        repeat: true
                                        onTriggered: parent.text = scannerController.formatElapsedTime()
                                    }
                                }
                            }
                        }

                        Item { Layout.fillHeight: true }

                        // 제어 버튼들
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 16

                            // 시작 버튼 (idle 상태)
                            Button {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 80
                                visible: !scannerController.isScanning && scannerController.status !== "completed"
                                text: "▶ 스캔 시작"
                                enabled: scannerController.isConnected && !root.emergencyStop

                                background: Rectangle {
                                    color: parent.enabled ? Colors.btnGreen : Colors.btnGray
                                    radius: 12
                                }

                                contentItem: Text {
                                    text: parent.text
                                    color: Colors.textPrimary
                                    font.pixelSize: 24
                                    font.bold: true
                                    horizontalAlignment: Text.AlignHCenter
                                }

                                onClicked: root.doStartScan()
                            }

                            // 스캔 중 버튼 행
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 16
                                visible: scannerController.isScanning

                                Button {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 80
                                    text: scannerController.status === "paused" ? "▶ 재개" : "⏸ 일시정지"
                                    enabled: !root.emergencyStop

                                    background: Rectangle {
                                        color: {
                                            if (!parent.enabled) return Colors.btnGray
                                            return scannerController.status === "paused" ? Colors.btnGreen : Colors.btnYellow
                                        }
                                        radius: 12
                                    }

                                    contentItem: Text {
                                        text: parent.text
                                        color: Colors.textPrimary
                                        font.pixelSize: 20
                                        font.bold: true
                                        horizontalAlignment: Text.AlignHCenter
                                    }

                                    onClicked: {
                                        if (scannerController.status === "paused")
                                            scannerController.resumeScan()
                                        else
                                            scannerController.pauseScan()
                                    }
                                }

                                Button {
                                    Layout.preferredWidth: 100
                                    Layout.preferredHeight: 80
                                    text: "⏹"
                                    enabled: !root.emergencyStop

                                    background: Rectangle {
                                        color: parent.enabled ? Colors.btnRed : Colors.btnGray
                                        radius: 12
                                    }

                                    contentItem: Text {
                                        text: parent.text
                                        font.pixelSize: 32
                                        horizontalAlignment: Text.AlignHCenter
                                    }

                                    onClicked: stopConfirmDialog.open()
                                }
                            }

                            // 완료 상태
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 150
                                visible: scannerController.status === "completed"

                                gradient: Gradient {
                                    GradientStop { position: 0.0; color: Colors.gradientGreenStart }
                                    GradientStop { position: 1.0; color: Colors.gradientGreenEnd }
                                }
                                radius: 16

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 24
                                    spacing: 16

                                    Text { text: "✅"; font.pixelSize: 60 }

                                    ColumnLayout {
                                        Layout.fillWidth: true

                                        Text {
                                            text: "스캔 완료!"
                                            color: Colors.textPrimary
                                            font.pixelSize: 28
                                            font.bold: true
                                        }

                                        Text {
                                            text: "파일이 저장되었습니다."
                                            color: Colors.textPrimary
                                            font.pixelSize: 16
                                        }
                                    }

                                    // 새 스캔 버튼
                                    Button {
                                        Layout.preferredWidth: 140
                                        Layout.preferredHeight: 60
                                        text: "새 스캔"
                                        enabled: !root.emergencyStop

                                        background: Rectangle {
                                            color: Colors.btnBlue
                                            radius: 10
                                        }

                                        contentItem: Text {
                                            text: parent.text
                                            color: Colors.textPrimary
                                            font.pixelSize: 18
                                            font.bold: true
                                            horizontalAlignment: Text.AlignHCenter
                                        }

                                        onClicked: root.doStartScan()
                                    }
                                }
                            }
                        }
                    }
                }

                // === 우측: 현재 설정 정보 ===
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 24

                    // 3D 스캐너 프리셋 정보
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.minimumHeight: 480
                        color: Colors.bgCard
                        radius: 16

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 32
                            spacing: 20

                            Text {
                                text: "스캔 설정 정보"
                                color: Colors.textPrimary
                                font.pixelSize: 24
                                font.bold: true
                            }

                            // 작업장 정보
                            InfoRow { label: "작업장"; value: root.workspace }
                            InfoRow { label: "저장 위치"; value: scanSettings.saveDirectory || "C:/3DScan/Results" }

                            Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 2; color: Colors.borderSecondary }

                            Text { text: "스캔 설정"; color: Colors.textTertiary; font.pixelSize: 16; font.bold: true }

                            InfoRow { label: "광원"; value: scanSettings.lightSource || "Laser" }
                            InfoRow { label: "스캔 모드"; value: {
                                var m = scanSettings.scanMode || "mesh"
                                if (m === "mesh") return "메쉬 스캔"
                                if (m === "marker") return "마커 스캔"
                                return "포토메트리"
                            }}
                            InfoRow { label: "레이저 라인수"; value: (scanSettings.laserLine || "cross") === "cross" ? "크로스" : "단일 라인" }
                            InfoRow { label: "대상물 재질"; value: {
                                var m = scanSettings.material || "normal"
                                if (m === "normal") return "일반"
                                if (m === "reflective") return "반사되는"
                                return "어두운"
                            }}

                            Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 2; color: Colors.borderSecondary }

                            Text { text: "후처리 설정"; color: Colors.textTertiary; font.pixelSize: 16; font.bold: true }

                            InfoRow { label: "필터"; value: (scanSettings.filterLevel || "2") + "단계" }
                            InfoRow { label: "스무스"; value: (scanSettings.smoothLevel || "1") + "단계" }
                            InfoRow { label: "파일 형식"; value: (scanSettings.fileFormat || "stl").toUpperCase() }
                        }
                    }

                    // 실시간 ZMQ 로그
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 200
                        color: Colors.bgCard
                        radius: 16

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 20
                            spacing: 12

                            Text {
                                text: "SDK 로그"
                                color: Colors.textPrimary
                                font.pixelSize: 18
                                font.bold: true
                            }

                            ListView {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                clip: true
                                model: {
                                    var logs = scannerController.logMessages
                                    return logs.length > 8 ? logs.slice(logs.length - 8) : logs
                                }

                                delegate: Text {
                                    width: parent.width
                                    text: modelData
                                    color: Colors.textTertiary
                                    font.pixelSize: 12
                                    font.family: "Courier New"
                                    wrapMode: Text.Wrap
                                }

                                onCountChanged: positionViewAtEnd()
                            }
                        }
                    }
                }
            }
        }
    }

    // 중지 확인 다이얼로그
    Dialog {
        id: stopConfirmDialog
        title: "스캔 중지"
        modal: true
        anchors.centerIn: parent

        contentItem: Text {
            text: "스캔을 중단하시겠습니까?"
            color: Colors.textPrimary
            font.pixelSize: 18
            padding: 20
        }

        standardButtons: Dialog.Yes | Dialog.No

        onAccepted: scannerController.stopScan()
    }
}

// 정보 행 컴포넌트
component InfoRow: RowLayout {
    property string label: ""
    property string value: ""

    Layout.fillWidth: true
    spacing: 12

    Text {
        Layout.preferredWidth: 120
        text: label + ":"
        color: Colors.textTertiary
        font.pixelSize: 14
    }

    Text {
        Layout.fillWidth: true
        text: value
        color: Colors.textPrimary
        font.pixelSize: 16
        font.bold: true
        wrapMode: Text.WordWrap
    }
}

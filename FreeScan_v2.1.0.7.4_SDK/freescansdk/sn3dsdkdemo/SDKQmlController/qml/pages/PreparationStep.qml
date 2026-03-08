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

    // 시스템 설정에서 WiFi 정보 로드
    property var sysSettings: dataManager.loadSettings()
    property string wifiIp: sysSettings.wifiIp || "192.168.1.100"
    property int    wifiPort: parseInt(sysSettings.wifiPort) || 11080

    function saveWifiSettings() {
        var s = dataManager.loadSettings()
        s.wifiIp   = wifiIpField.text
        s.wifiPort = parseInt(wifiPortField.text) || 11080
        dataManager.saveSettings(s)
        wifiIp   = s.wifiIp
        wifiPort = s.wifiPort
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
                text: "자동화 3D스캔 준비"
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

                // === 3D 스캐너 연결 ===
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 680
                    color: Colors.bgCard
                    radius: 16

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 32
                        spacing: 24

                        Text {
                            text: "3D 스캐너 연결 (FreeScan UE Pro2)"
                            color: Colors.textPrimary
                            font.pixelSize: 24
                            font.bold: true
                        }

                        // WiFi 설정
                        Rectangle {
                            Layout.fillWidth: true
                            color: Colors.bgTertiary
                            radius: 12
                            height: wifiLayout.height + 32

                            ColumnLayout {
                                id: wifiLayout
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.top: parent.top
                                anchors.margins: 16
                                spacing: 12

                                Text {
                                    text: "WiFi 연결 설정"
                                    color: Colors.textSecondary
                                    font.pixelSize: 16
                                    font.bold: true
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 12

                                    Text {
                                        text: "IP 주소"
                                        color: Colors.textTertiary
                                        font.pixelSize: 14
                                        Layout.preferredWidth: 60
                                    }

                                    TextField {
                                        id: wifiIpField
                                        Layout.fillWidth: true
                                        text: root.wifiIp
                                        color: Colors.textPrimary
                                        font.pixelSize: 15
                                        placeholderText: "192.168.1.100"

                                        background: Rectangle {
                                            color: Colors.bgCard
                                            radius: 8
                                            border.color: parent.activeFocus ? Colors.borderBlue : Colors.borderSecondary
                                            border.width: 2
                                        }
                                    }

                                    Text {
                                        text: "포트"
                                        color: Colors.textTertiary
                                        font.pixelSize: 14
                                        Layout.preferredWidth: 40
                                    }

                                    TextField {
                                        id: wifiPortField
                                        Layout.preferredWidth: 90
                                        text: root.wifiPort.toString()
                                        color: Colors.textPrimary
                                        font.pixelSize: 15
                                        placeholderText: "11080"
                                        inputMethodHints: Qt.ImhDigitsOnly

                                        background: Rectangle {
                                            color: Colors.bgCard
                                            radius: 8
                                            border.color: parent.activeFocus ? Colors.borderBlue : Colors.borderSecondary
                                            border.width: 2
                                        }
                                    }
                                }
                            }
                        }

                        // 스캐너 아이콘 영역
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 180
                            color: Colors.bgTertiary
                            radius: 12
                            border.color: Colors.borderSecondary
                            border.width: 2

                            Column {
                                anchors.centerIn: parent
                                spacing: 16

                                Text {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: "💻"
                                    font.pixelSize: 80
                                }

                                Text {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: {
                                        switch(scannerController.status) {
                                            case "connecting": return "🔄"
                                            case "success": return "✅"
                                            case "error": return "❌"
                                            default: return "⚠"
                                        }
                                    }
                                    font.pixelSize: 40

                                    RotationAnimation on rotation {
                                        running: scannerController.status === "connecting"
                                        from: 0
                                        to: 360
                                        duration: 2000
                                        loops: Animation.Infinite
                                    }
                                }
                            }
                        }

                        // 연결 버튼
                        Button {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 70
                            text: {
                                switch(scannerController.status) {
                                    case "connecting": return "연결 중..."
                                    case "success": return "재연결"
                                    case "error": return "다시 시도"
                                    default: return "3D 스캐너 준비"
                                }
                            }
                            enabled: scannerController.status !== "connecting" && !root.emergencyStop

                            background: Rectangle {
                                color: {
                                    if (!parent.enabled) return Colors.btnGray
                                    switch(scannerController.status) {
                                        case "connecting": return Colors.btnYellow
                                        case "success": return Colors.btnGreen
                                        case "error": return Colors.btnRed
                                        default: return Colors.btnBlue
                                    }
                                }
                                radius: 12
                            }

                            contentItem: Text {
                                text: parent.text
                                color: Colors.textPrimary
                                font.pixelSize: 20
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }

                            onClicked: {
                                saveWifiSettings()
                                if (scannerController.isConnected)
                                    scannerController.disconnectScanner()
                                // wifiIp/wifiPort를 FreeScanController 프로퍼티에 반영
                                scannerController.wifiIp   = wifiIpField.text
                                scannerController.wifiPort = parseInt(wifiPortField.text) || 11080
                                scannerController.connectScanner()
                            }
                        }

                        // 스캐너 정보
                        Rectangle {
                            Layout.fillWidth: true
                            color: Colors.bgTertiary
                            radius: 12
                            height: infoCol.height + 32

                            ColumnLayout {
                                id: infoCol
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.top: parent.top
                                anchors.margins: 16
                                spacing: 8

                                Text {
                                    text: "모델: " + (scannerController.deviceModel || "FreeScan UE Pro2")
                                    color: Colors.textTertiary
                                    font.pixelSize: 14
                                }

                                Text {
                                    text: "연결 방식: WiFi"
                                    color: Colors.textTertiary
                                    font.pixelSize: 14
                                }

                                Text {
                                    text: "상태: " + (scannerController.isConnected ? "연결됨" : "대기중")
                                    color: scannerController.isConnected ? Colors.success : Colors.textTertiary
                                    font.pixelSize: 16
                                    font.bold: true
                                }
                            }
                        }

                        // 상태 메시지
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 70
                            color: {
                                switch(scannerController.status) {
                                    case "success": return Qt.rgba(Colors.success.r, Colors.success.g, Colors.success.b, 0.2)
                                    case "error":   return Qt.rgba(Colors.error.r,   Colors.error.g,   Colors.error.b,   0.2)
                                    default: return Colors.bgTertiary
                                }
                            }
                            radius: 12
                            border.color: {
                                switch(scannerController.status) {
                                    case "success": return Colors.success
                                    case "error":   return Colors.error
                                    default: return Colors.borderSecondary
                                }
                            }
                            border.width: 2

                            Text {
                                anchors.fill: parent
                                anchors.margins: 16
                                text: scannerController.statusMessage
                                color: Colors.textPrimary
                                font.pixelSize: 15
                                wrapMode: Text.WordWrap
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                }

                // === 시뮬레이션 / 연결 가이드 ===
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 680
                    color: Colors.bgCard
                    radius: 16

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 32
                        spacing: 24

                        Text {
                            text: "시스템 상태"
                            color: Colors.textPrimary
                            font.pixelSize: 24
                            font.bold: true
                        }

                        // 연결 단계 표시
                        Repeater {
                            model: [
                                { step: "1", label: "FreeScan SDK 초기화", done: scannerController.isConnected || scannerController.status !== "idle" },
                                { step: "2", label: "WiFi 소켓 연결",      done: scannerController.isConnected },
                                { step: "3", label: "장치 인증",           done: scannerController.isConnected },
                                { step: "4", label: "스캔 준비 완료",       done: scannerController.isConnected }
                            ]

                            delegate: Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 80
                                color: modelData.done ? Qt.rgba(Colors.success.r, Colors.success.g, Colors.success.b, 0.1)
                                                     : Colors.bgTertiary
                                radius: 12
                                border.color: modelData.done ? Colors.success : Colors.borderSecondary
                                border.width: 2

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 20
                                    spacing: 16

                                    Rectangle {
                                        width: 44
                                        height: 44
                                        radius: 22
                                        color: modelData.done ? Colors.success : Colors.btnGray

                                        Text {
                                            anchors.centerIn: parent
                                            text: modelData.done ? "✓" : modelData.step
                                            color: Colors.textPrimary
                                            font.pixelSize: 18
                                            font.bold: true
                                        }
                                    }

                                    Text {
                                        Layout.fillWidth: true
                                        text: modelData.label
                                        color: modelData.done ? Colors.textPrimary : Colors.textTertiary
                                        font.pixelSize: 18
                                        font.bold: modelData.done
                                    }
                                }
                            }
                        }

                        // 로그 최신 5줄
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: Colors.bgTertiary
                            radius: 12

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 8

                                Text {
                                    text: "연결 로그"
                                    color: Colors.textSecondary
                                    font.pixelSize: 16
                                    font.bold: true
                                }

                                ListView {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    clip: true
                                    model: {
                                        var logs = scannerController.logMessages
                                        // 최근 10개
                                        return logs.length > 10 ? logs.slice(logs.length - 10) : logs
                                    }

                                    delegate: Text {
                                        width: parent.width
                                        text: modelData
                                        color: Colors.textTertiary
                                        font.pixelSize: 13
                                        font.family: "Courier New"
                                        wrapMode: Text.Wrap
                                    }

                                    onCountChanged: {
                                        positionViewAtEnd()
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // 준비 완료 배너
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 120
                visible: scannerController.isConnected

                gradient: Gradient {
                    GradientStop { position: 0.0; color: Colors.gradientGreenStart }
                    GradientStop { position: 1.0; color: Colors.gradientGreenEnd }
                }
                radius: 16

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 32
                    spacing: 24

                    Text { text: "✅"; font.pixelSize: 60 }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4

                        Text {
                            text: "준비 완료"
                            color: Colors.textPrimary
                            font.pixelSize: 28
                            font.bold: true
                        }

                        Text {
                            text: "FreeScan UE Pro2가 연결되었습니다. 스캔 시작 페이지로 이동하세요."
                            color: Colors.textPrimary
                            font.pixelSize: 16
                        }
                    }
                }

                SequentialAnimation on scale {
                    running: true
                    loops: Animation.Infinite
                    NumberAnimation { to: 1.01; duration: 1000; easing.type: Easing.InOutQuad }
                    NumberAnimation { to: 1.0;  duration: 1000; easing.type: Easing.InOutQuad }
                }
            }
        }
    }
}

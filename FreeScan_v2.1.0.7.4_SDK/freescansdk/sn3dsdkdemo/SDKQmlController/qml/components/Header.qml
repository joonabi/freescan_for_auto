import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../styles"

Rectangle {
    id: root
    color: Colors.bgSecondary
    border.color: Colors.borderPrimary
    border.width: 1

    property string clientName: "J3D"
    property string clientLogo: ""
    property bool emergencyStop: false

    signal emergencyStopClicked()
    signal resumeClicked()

    RowLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 20

        // 좌측: 시스템 타이틀
        ColumnLayout {
            Layout.alignment: Qt.AlignVCenter
            spacing: 4

            Text {
                text: "3D FOR AUTOMATION"
                color: Colors.textPrimary
                font.pixelSize: 24
                font.bold: true
            }

            Text {
                text: "자동 3D스캐너 컨트롤러 (FreeScan UE Pro2)"
                color: Colors.textTertiary
                font.pixelSize: 14
            }
        }

        Item { Layout.fillWidth: true }

        // 중앙: 수요처 로고 및 이름
        RowLayout {
            Layout.alignment: Qt.AlignVCenter
            spacing: 16

            // 로고
            Rectangle {
                Layout.preferredWidth: 60
                Layout.preferredHeight: 60
                color: Colors.bgTertiary
                radius: 8
                border.color: Colors.borderSecondary
                border.width: 1
                visible: root.clientLogo !== ""

                Image {
                    anchors.fill: parent
                    anchors.margins: 4
                    source: root.clientLogo
                    fillMode: Image.PreserveAspectFit
                    asynchronous: true
                }
            }

            // 수요처 정보
            ColumnLayout {
                spacing: 4

                Text {
                    text: "수요처"
                    color: Colors.textTertiary
                    font.pixelSize: 12
                }

                Text {
                    text: root.clientName
                    color: Colors.textPrimary
                    font.pixelSize: 20
                    font.bold: true
                }
            }
        }

        Item { Layout.fillWidth: true }

        // 우측: 시스템 상태 및 제어 버튼
        RowLayout {
            Layout.alignment: Qt.AlignVCenter
            spacing: 16

            // 스캐너 연결 상태
            Rectangle {
                Layout.preferredWidth: 160
                Layout.preferredHeight: 60
                color: {
                    if (scannerController.isConnected)
                        return Qt.rgba(Colors.success.r, Colors.success.g, Colors.success.b, 0.15)
                    return Qt.rgba(Colors.textDisabled.r, Colors.textDisabled.g, Colors.textDisabled.b, 0.15)
                }
                radius: 8
                border.color: scannerController.isConnected ? Colors.success : Colors.borderSecondary
                border.width: 1

                RowLayout {
                    anchors.centerIn: parent
                    spacing: 8

                    Rectangle {
                        width: 10
                        height: 10
                        radius: 5
                        color: scannerController.isConnected ? Colors.success : Colors.textDisabled

                        SequentialAnimation on opacity {
                            running: scannerController.isConnected && !root.emergencyStop
                            loops: Animation.Infinite
                            NumberAnimation { to: 0.3; duration: 1000 }
                            NumberAnimation { to: 1.0; duration: 1000 }
                        }
                    }

                    Text {
                        text: scannerController.isConnected ? "스캐너 연결됨" : "미연결"
                        color: Colors.textPrimary
                        font.pixelSize: 14
                        font.bold: true
                    }
                }
            }

            // 긴급 제어 버튼
            Button {
                Layout.preferredWidth: 160
                Layout.preferredHeight: 60

                text: root.emergencyStop ? "재시작" : "긴급 정지"

                background: Rectangle {
                    color: {
                        if (root.emergencyStop) {
                            return parent.pressed ? Colors.btnGreenHover :
                                   parent.hovered ? Qt.lighter(Colors.btnGreen, 1.1) :
                                   Colors.btnGreen
                        } else {
                            return parent.pressed ? Colors.btnRedHover :
                                   parent.hovered ? Qt.lighter(Colors.btnRed, 1.1) :
                                   Colors.btnRed
                        }
                    }
                    radius: 8
                    Behavior on color { ColorAnimation { duration: 150 } }
                }

                contentItem: RowLayout {
                    spacing: 8
                    Text {
                        text: root.emergencyStop ? "🔄" : "🛑"
                        font.pixelSize: 20
                    }
                    Text {
                        text: parent.parent.text
                        color: Colors.textPrimary
                        font.pixelSize: 16
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                onClicked: {
                    if (root.emergencyStop)
                        root.resumeClicked()
                    else
                        root.emergencyStopClicked()
                }
            }
        }
    }
}

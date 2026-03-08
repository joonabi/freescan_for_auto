import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15
import "../styles"

Rectangle {
    id: root
    color: Colors.bgSecondary
    border.color: Colors.borderPrimary
    border.width: 1

    property string currentStep: "workplace"
    property bool emergencyStop: false

    signal stepChanged(string step)
    signal emergencyStopClicked()
    signal resumeClicked()

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // 로고/타이틀 영역
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            color: Colors.bgTertiary

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 4

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "3D FOR"
                    color: Colors.textPrimary
                    font.pixelSize: 20
                    font.bold: true
                }

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "AUTOMATION"
                    color: Colors.btnBlue
                    font.pixelSize: 16
                    font.bold: true
                }
            }
        }

        // 구분선
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 2
            color: Colors.borderPrimary
        }

        // 메뉴 영역
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AsNeeded

            ColumnLayout {
                width: parent.width
                spacing: 12

                Item { Layout.preferredHeight: 12 }

                Repeater {
                    model: ListModel {
                        ListElement { stepId: "workplace"; label: "작업장 관리";      icon: "🏢" }
                        ListElement { stepId: "prepare";   label: "자동화 3D스캔 준비"; icon: "💻" }
                        ListElement { stepId: "start";     label: "AUTO 3D스캔 시작";  icon: "▶" }
                        ListElement { stepId: "settings";  label: "자동화 설정";       icon: "⚙" }
                        ListElement { stepId: "logs";      label: "Log 정보";          icon: "📄" }
                        ListElement { stepId: "manual";    label: "사용 메뉴얼";       icon: "📖" }
                    }

                    delegate: Button {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 80
                        Layout.leftMargin: 16
                        Layout.rightMargin: 16

                        enabled: !root.emergencyStop

                        background: Rectangle {
                            color: {
                                if (!parent.enabled)
                                    return Qt.rgba(Colors.bgTertiary.r, Colors.bgTertiary.g, Colors.bgTertiary.b, 0.5)
                                if (model.stepId === root.currentStep)
                                    return Colors.btnBlue
                                if (parent.hovered)
                                    return Colors.bgCard
                                return Colors.bgTertiary
                            }
                            radius: 12

                            Behavior on color { ColorAnimation { duration: 200 } }

                            layer.enabled: model.stepId === root.currentStep
                            layer.effect: DropShadow {
                                horizontalOffset: 0
                                verticalOffset: 4
                                radius: 8.0
                                samples: 17
                                color: "#40000000"
                            }
                        }

                        contentItem: RowLayout {
                            spacing: 12

                            Text {
                                text: model.icon
                                font.pixelSize: 28
                                opacity: parent.parent.enabled ? 1.0 : 0.5
                            }

                            Text {
                                Layout.fillWidth: true
                                text: model.label
                                color: parent.parent.enabled ? Colors.textPrimary : Colors.textDisabled
                                font.pixelSize: parent.parent.width > 280 ? 18 : 16
                                font.bold: true
                                wrapMode: Text.WordWrap
                                maximumLineCount: 2
                                elide: Text.ElideRight
                            }
                        }

                        onClicked: root.stepChanged(model.stepId)
                    }
                }

                Item { Layout.preferredHeight: 12 }
            }
        }

        // 구분선
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 2
            color: Colors.borderPrimary
        }

        // 긴급 제어 영역
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 220
            color: Colors.bgTertiary

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 12

                RowLayout {
                    spacing: 12

                    Text {
                        text: "⚠"
                        font.pixelSize: 28
                        color: Colors.error
                    }

                    Text {
                        text: "긴급 제어"
                        color: Colors.textPrimary
                        font.pixelSize: 20
                        font.bold: true
                    }
                }

                Text {
                    Layout.fillWidth: true
                    text: root.emergencyStop ? "모든 장비가 정지됨" : "비상 시 즉시 정지"
                    color: Colors.textTertiary
                    font.pixelSize: 14
                    wrapMode: Text.WordWrap
                }

                Item { Layout.fillHeight: true }

                Button {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 70

                    text: root.emergencyStop ? "🔄 다시 동작" : "🛑 긴급 정지"

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
                        radius: 12

                        Behavior on color { ColorAnimation { duration: 150 } }

                        SequentialAnimation on opacity {
                            running: !root.emergencyStop
                            loops: Animation.Infinite
                            NumberAnimation { to: 0.7; duration: 1000 }
                            NumberAnimation { to: 1.0; duration: 1000 }
                        }
                    }

                    contentItem: Text {
                        text: parent.text
                        color: Colors.textPrimary
                        font.pixelSize: 18
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
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
}

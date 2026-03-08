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
    property string filterLevel: "전체"
    property string searchText: ""

    // SDK 로그 메시지 (FreeScanController.logMessages QStringList)
    // 형식: "[HH:mm:ss] [LEVEL] 메시지"
    property var rawLogs: scannerController.logMessages

    // 필터링된 로그
    property var filteredLogs: {
        var result = []
        for (var i = 0; i < rawLogs.length; i++) {
            var line = rawLogs[i]
            if (filterLevel !== "전체" && line.indexOf("[" + filterLevel + "]") < 0)
                continue
            if (searchText.length > 0 && line.toLowerCase().indexOf(searchText.toLowerCase()) < 0)
                continue
            result.push(line)
        }
        return result
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 32
        spacing: 24

        // 헤더
        RowLayout {
            Layout.fillWidth: true

            Text {
                text: "Log 정보"
                color: Colors.textPrimary
                font.pixelSize: 32
                font.bold: true
            }

            Item { Layout.fillWidth: true }

            Text {
                text: rawLogs.length + " 항목"
                color: Colors.textTertiary
                font.pixelSize: 18
            }
        }

        // 필터 및 액션
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            color: Colors.bgCard
            radius: 12

            RowLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 16

                // 레벨 필터
                ComboBox {
                    Layout.preferredWidth: 150
                    model: ["전체", "INFO", "WARN", "ERROR", "DEBUG"]

                    onActivated: {
                        if (currentIndex === 0) filterLevel = "전체"
                        else filterLevel = model[currentIndex]
                    }

                    delegate: ItemDelegate {
                        width: parent.width
                        contentItem: Text {
                            text: modelData
                            color: Colors.textPrimary
                            font.pixelSize: 14
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            color: parent.highlighted ? Colors.btnBlue : Colors.bgTertiary
                        }
                    }
                }

                // 검색
                TextField {
                    Layout.fillWidth: true
                    placeholderText: "검색..."
                    color: Colors.textPrimary
                    font.pixelSize: 16
                    onTextChanged: root.searchText = text

                    background: Rectangle {
                        color: Colors.bgTertiary
                        radius: 8
                        border.color: parent.activeFocus ? Colors.borderBlue : Colors.borderSecondary
                        border.width: 2
                    }
                }

                Button {
                    text: "🔄 새로고침"

                    background: Rectangle { color: Colors.btnBlue; radius: 8 }

                    contentItem: Text {
                        text: parent.text
                        color: Colors.textPrimary
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                    }

                    onClicked: {
                        // 강제 리프레시
                        var temp = root.filterLevel
                        root.filterLevel = ""
                        root.filterLevel = temp
                    }
                }

                Button {
                    text: "🗑 초기화"

                    background: Rectangle { color: Colors.btnRed; radius: 8 }

                    contentItem: Text {
                        text: parent.text
                        color: Colors.textPrimary
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                    }

                    onClicked: scannerController.clearLog()
                }
            }
        }

        // 로그 목록
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Colors.bgCard
            radius: 16

            ListView {
                id: logListView
                anchors.fill: parent
                anchors.margins: 16
                clip: true
                model: filteredLogs

                // 새 항목이 추가될 때 맨 아래로 스크롤
                onCountChanged: {
                    if (count > 0) Qt.callLater(positionViewAtEnd)
                }

                delegate: Rectangle {
                    width: logListView.width
                    height: logText.height + 16
                    color: {
                        if (modelData.indexOf("[ERROR]") >= 0)
                            return Qt.rgba(Colors.error.r, Colors.error.g, Colors.error.b, 0.1)
                        if (modelData.indexOf("[WARN]") >= 0)
                            return Qt.rgba(Colors.warning.r, Colors.warning.g, Colors.warning.b, 0.1)
                        return index % 2 === 0 ? Colors.bgTertiary : "transparent"
                    }
                    radius: 6
                    border.color: {
                        if (modelData.indexOf("[ERROR]") >= 0) return Colors.error
                        if (modelData.indexOf("[WARN]") >= 0)  return Colors.warning
                        return "transparent"
                    }
                    border.width: modelData.indexOf("[ERROR]") >= 0 || modelData.indexOf("[WARN]") >= 0 ? 1 : 0

                    Text {
                        id: logText
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.margins: 12
                        text: modelData
                        color: {
                            if (modelData.indexOf("[ERROR]") >= 0) return Colors.error
                            if (modelData.indexOf("[WARN]") >= 0)  return Colors.warning
                            if (modelData.indexOf("[INFO]") >= 0)  return Colors.textPrimary
                            return Colors.textTertiary
                        }
                        font.pixelSize: 13
                        font.family: "Courier New"
                        wrapMode: Text.Wrap
                    }
                }

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                }
            }
        }
    }
}

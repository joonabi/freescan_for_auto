import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../styles"

Item {
    id: root
    implicitWidth: 200
    implicitHeight: 48

    property string value: "auto"  // "manual" or "auto"
    property var labels: ["수동", "자동"]
    property bool enabled: true

    signal valueChanged(string newValue)

    Rectangle {
        anchors.fill: parent
        color: Colors.bgTertiary
        radius: 8
        border.color: Colors.borderSecondary
        border.width: 1
        opacity: root.enabled ? 1.0 : 0.5

        Row {
            anchors.fill: parent
            spacing: 0

            Repeater {
                model: root.labels

                Rectangle {
                    width: root.width / 2
                    height: root.height
                    color: {
                        var isSelected = (index === 0 && root.value === "manual") ||
                                       (index === 1 && root.value === "auto")
                        return isSelected ? Colors.btnBlue : "transparent"
                    }
                    radius: 8

                    Text {
                        anchors.centerIn: parent
                        text: modelData
                        color: {
                            var isSelected = (index === 0 && root.value === "manual") ||
                                           (index === 1 && root.value === "auto")
                            return isSelected ? Colors.textPrimary : Colors.textTertiary
                        }
                        font.pixelSize: 16
                        font.bold: true
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: root.enabled ? Qt.PointingHandCursor : Qt.ForbiddenCursor
                        enabled: root.enabled

                        onClicked: {
                            var newValue = index === 0 ? "manual" : "auto"
                            root.value = newValue
                            root.valueChanged(newValue)
                        }
                    }

                    Behavior on color { ColorAnimation { duration: 200 } }
                }
            }
        }
    }
}

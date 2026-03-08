import QtQuick 2.15
import QtQuick.Controls 2.15
import "../styles"

Item {
    id: root
    implicitWidth: 150
    implicitHeight: 40

    property string label: "Option"
    property bool checked: false
    property bool enabled: true

    signal clicked()

    Rectangle {
        anchors.fill: parent
        color: root.checked ? Colors.btnBlue : Colors.bgTertiary
        radius: 8
        border.color: root.checked ? Colors.borderBlue : Colors.borderSecondary
        border.width: 2
        opacity: root.enabled ? 1.0 : 0.5

        Behavior on color { ColorAnimation { duration: 200 } }
        Behavior on border.color { ColorAnimation { duration: 200 } }

        Row {
            anchors.centerIn: parent
            spacing: 8

            Rectangle {
                width: 16
                height: 16
                radius: 8
                color: "transparent"
                border.color: root.checked ? Colors.textPrimary : Colors.textTertiary
                border.width: 2

                Rectangle {
                    anchors.centerIn: parent
                    width: 8
                    height: 8
                    radius: 4
                    color: Colors.textPrimary
                    visible: root.checked

                    scale: root.checked ? 1 : 0
                    Behavior on scale { NumberAnimation { duration: 150; easing.type: Easing.OutBack } }
                }
            }

            Text {
                text: root.label
                color: root.checked ? Colors.textPrimary : Colors.textTertiary
                font.pixelSize: 16
                font.bold: root.checked
            }
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: root.enabled ? Qt.PointingHandCursor : Qt.ForbiddenCursor
            enabled: root.enabled
            onClicked: root.clicked()
        }
    }
}

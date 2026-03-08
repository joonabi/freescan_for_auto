import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../styles"

Item {
    id: root
    implicitWidth: 200
    implicitHeight: 40

    property string label: "Option"
    property bool checked: false
    property bool enabled: true

    signal toggled(bool checked)

    Rectangle {
        anchors.fill: parent
        color: "transparent"

        RowLayout {
            anchors.fill: parent
            spacing: 12

            Rectangle {
                width: 28
                height: 28
                radius: 4
                color: root.checked ? Colors.btnBlue : Colors.bgTertiary
                border.color: root.checked ? Colors.borderBlue : Colors.borderSecondary
                border.width: 2
                opacity: root.enabled ? 1.0 : 0.5

                Behavior on color { ColorAnimation { duration: 200 } }

                Text {
                    anchors.centerIn: parent
                    text: "✓"
                    color: Colors.textPrimary
                    font.pixelSize: 18
                    font.bold: true
                    visible: root.checked

                    scale: root.checked ? 1 : 0
                    Behavior on scale { NumberAnimation { duration: 150; easing.type: Easing.OutBack } }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: root.enabled ? Qt.PointingHandCursor : Qt.ForbiddenCursor
                    enabled: root.enabled
                    onClicked: {
                        root.checked = !root.checked
                        root.toggled(root.checked)
                    }
                }
            }

            Text {
                Layout.fillWidth: true
                text: root.label
                color: root.enabled ? Colors.textPrimary : Colors.textDisabled
                font.pixelSize: 18
                font.bold: root.checked
                wrapMode: Text.WordWrap

                MouseArea {
                    anchors.fill: parent
                    cursorShape: root.enabled ? Qt.PointingHandCursor : Qt.ForbiddenCursor
                    enabled: root.enabled
                    onClicked: {
                        root.checked = !root.checked
                        root.toggled(root.checked)
                    }
                }
            }
        }
    }
}

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

    // DataManager에서 설정 로드
    property var scanSettings: dataManager.loadScanSettings()

    // 스캔 설정 상태 (로드된 값으로 초기화)
    property string selectedLightSource: scanSettings.lightSource  || "Laser"
    property string selectedAlignment:   scanSettings.scanMode     || "mesh"
    property string selectedLaserLine:   scanSettings.laserLine    || "cross"
    property string selectedMaterial:    scanSettings.material     || "normal"
    property string saveDirectory:       scanSettings.saveDirectory || "C:/3DScan/Results"

    property string filterLevel:  scanSettings.filterLevel  || "2"
    property string smoothLevel:  scanSettings.smoothLevel  || "1"
    property bool fillMarkerHoles: scanSettings.fillMarkerHoles !== undefined ? scanSettings.fillMarkerHoles : true
    property bool fillSmallHoles:  scanSettings.fillSmallHoles  !== undefined ? scanSettings.fillSmallHoles  : true
    property bool removeAnomalies: scanSettings.removeAnomalies !== undefined ? scanSettings.removeAnomalies : true
    property bool removeSpikes:    scanSettings.removeSpikes    !== undefined ? scanSettings.removeSpikes    : true
    property string fileFormat:   scanSettings.fileFormat  || "stl"

    // 자동/수동 토글 상태
    property string lightSourceMode: "auto"
    property string alignmentMode:   "auto"
    property string laserLineMode:   "auto"
    property string materialMode:    "auto"
    property string filterMode:      "auto"
    property string saveMode:        "auto"

    // 설정 저장 함수
    function saveAllSettings() {
        dataManager.saveScanSettings({
            "saveDirectory":   saveDirectory,
            "lightSource":     selectedLightSource,
            "scanMode":        selectedAlignment,
            "laserLine":       selectedLaserLine,
            "material":        selectedMaterial,
            "filterLevel":     filterLevel,
            "smoothLevel":     smoothLevel,
            "fillMarkerHoles": fillMarkerHoles,
            "fillSmallHoles":  fillSmallHoles,
            "removeAnomalies": removeAnomalies,
            "removeSpikes":    removeSpikes,
            "fileFormat":      fileFormat
        })
    }

    ScrollView {
        anchors.fill: parent
        clip: true

        ColumnLayout {
            width: parent.parent.width
            spacing: 32
            padding: 32

            Text {
                text: "자동화 설정"
                color: Colors.textPrimary
                font.pixelSize: 32
                font.bold: true
            }

            GridLayout {
                Layout.fillWidth: true
                columns: parent.width > 1200 ? 2 : 1
                columnSpacing: 32
                rowSpacing: 32

                // === 스캔 설정 ===
                Rectangle {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 800
                    color: Colors.bgCard
                    radius: 16

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 32
                        spacing: 24

                        Text {
                            text: "폴더 저장 경로 및 스캔 파라미터"
                            color: Colors.textPrimary
                            font.pixelSize: 24
                            font.bold: true
                        }

                        ScrollView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true

                            ColumnLayout {
                                width: parent.width
                                spacing: 24

                                // 저장 경로
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 12

                                    Text { text: "저장 경로"; color: Colors.textPrimary; font.pixelSize: 18; font.bold: true }

                                    TextField {
                                        id: saveDirField
                                        Layout.fillWidth: true
                                        text: root.saveDirectory
                                        color: Colors.textPrimary
                                        font.pixelSize: 16
                                        enabled: !root.emergencyStop
                                        onTextChanged: root.saveDirectory = text

                                        background: Rectangle {
                                            color: Colors.bgTertiary
                                            radius: 8
                                            border.color: parent.activeFocus ? Colors.borderBlue : Colors.borderSecondary
                                            border.width: 2
                                        }
                                    }
                                }

                                // 광원 선택
                                SettingItem {
                                    Layout.fillWidth: true
                                    label: "광원 선택"
                                    mode: lightSourceMode
                                    enabled: !root.emergencyStop
                                    onModeChanged: lightSourceMode = mode

                                    content: Row {
                                        spacing: 12
                                        RadioButton {
                                            label: "Laser"
                                            checked: selectedLightSource === "Laser"
                                            onClicked: selectedLightSource = "Laser"
                                        }
                                        RadioButton {
                                            label: "IR"
                                            checked: selectedLightSource === "IR"
                                            onClicked: selectedLightSource = "IR"
                                        }
                                    }
                                }

                                // 스캔 모드
                                SettingItem {
                                    Layout.fillWidth: true
                                    label: "스캔 모드"
                                    mode: alignmentMode
                                    enabled: !root.emergencyStop
                                    onModeChanged: alignmentMode = mode

                                    content: Row {
                                        spacing: 12
                                        RadioButton {
                                            label: "마커 스캔"
                                            checked: selectedAlignment === "marker"
                                            onClicked: selectedAlignment = "marker"
                                        }
                                        RadioButton {
                                            label: "메쉬 스캔"
                                            checked: selectedAlignment === "mesh"
                                            onClicked: selectedAlignment = "mesh"
                                        }
                                        RadioButton {
                                            label: "포토메트리"
                                            checked: selectedAlignment === "photometry"
                                            onClicked: selectedAlignment = "photometry"
                                        }
                                    }
                                }

                                // 레이저 라인수
                                SettingItem {
                                    Layout.fillWidth: true
                                    label: "레이저 라인수"
                                    mode: laserLineMode
                                    enabled: !root.emergencyStop
                                    onModeChanged: laserLineMode = mode

                                    content: Row {
                                        spacing: 12
                                        RadioButton {
                                            label: "크로스"
                                            checked: selectedLaserLine === "cross"
                                            onClicked: selectedLaserLine = "cross"
                                        }
                                        RadioButton {
                                            label: "단일 라인"
                                            checked: selectedLaserLine === "single"
                                            onClicked: selectedLaserLine = "single"
                                        }
                                    }
                                }

                                // 대상물 재질
                                SettingItem {
                                    Layout.fillWidth: true
                                    label: "대상물 재질"
                                    mode: materialMode
                                    enabled: !root.emergencyStop
                                    onModeChanged: materialMode = mode

                                    content: Row {
                                        spacing: 12
                                        RadioButton {
                                            label: "일반"
                                            checked: selectedMaterial === "normal"
                                            onClicked: selectedMaterial = "normal"
                                        }
                                        RadioButton {
                                            label: "반사되는"
                                            checked: selectedMaterial === "reflective"
                                            onClicked: selectedMaterial = "reflective"
                                        }
                                        RadioButton {
                                            label: "어두운"
                                            checked: selectedMaterial === "dark"
                                            onClicked: selectedMaterial = "dark"
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // === 후처리 설정 ===
                Rectangle {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 800
                    color: Colors.bgCard
                    radius: 16

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 32
                        spacing: 24

                        Text {
                            text: "3D 스캔 후처리"
                            color: Colors.textPrimary
                            font.pixelSize: 24
                            font.bold: true
                        }

                        ScrollView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true

                            ColumnLayout {
                                width: parent.width
                                spacing: 24

                                // 필터
                                SettingItem {
                                    Layout.fillWidth: true
                                    label: "필터"
                                    mode: filterMode
                                    borderColor: Colors.borderGreen
                                    enabled: !root.emergencyStop
                                    onModeChanged: filterMode = mode

                                    content: ColumnLayout {
                                        spacing: 16

                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 8

                                            Text { text: "필터 단계"; color: Colors.textSecondary; font.pixelSize: 16; font.bold: true }

                                            Row {
                                                spacing: 8
                                                Repeater {
                                                    model: ["none", "1", "2", "3"]
                                                    RadioButton {
                                                        label: modelData === "none" ? "처리 안함" : modelData + "단계"
                                                        checked: filterLevel === modelData
                                                        onClicked: filterLevel = modelData
                                                    }
                                                }
                                            }
                                        }

                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 8

                                            Text { text: "스무스 강도"; color: Colors.textSecondary; font.pixelSize: 16; font.bold: true }

                                            Row {
                                                spacing: 8
                                                Repeater {
                                                    model: ["none", "1", "2", "3"]
                                                    RadioButton {
                                                        label: modelData === "none" ? "없음" : modelData + "단계"
                                                        checked: smoothLevel === modelData
                                                        onClicked: smoothLevel = modelData
                                                    }
                                                }
                                            }
                                        }

                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 12

                                            Checkbox {
                                                label: "마커 구멍 채우기"
                                                checked: fillMarkerHoles
                                                onToggled: fillMarkerHoles = checked
                                            }
                                            Checkbox {
                                                label: "작은 구멍 채우기"
                                                checked: fillSmallHoles
                                                onToggled: fillSmallHoles = checked
                                            }
                                            Checkbox {
                                                label: "이상치 제거"
                                                checked: removeAnomalies
                                                onToggled: removeAnomalies = checked
                                            }
                                            Checkbox {
                                                label: "스파이크 제거"
                                                checked: removeSpikes
                                                onToggled: removeSpikes = checked
                                            }
                                        }
                                    }
                                }

                                // 저장 형식
                                SettingItem {
                                    Layout.fillWidth: true
                                    label: "저장 형식"
                                    mode: saveMode
                                    enabled: !root.emergencyStop
                                    onModeChanged: saveMode = mode

                                    content: Row {
                                        spacing: 12
                                        RadioButton { label: "STL"; checked: fileFormat === "stl"; onClicked: fileFormat = "stl" }
                                        RadioButton { label: "OBJ"; checked: fileFormat === "obj"; onClicked: fileFormat = "obj" }
                                        RadioButton { label: "PLY"; checked: fileFormat === "ply"; onClicked: fileFormat = "ply" }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // 저장 버튼
            Button {
                Layout.fillWidth: true
                Layout.preferredHeight: 70
                text: "✓ 설정 저장"
                enabled: !root.emergencyStop

                background: Rectangle {
                    color: parent.enabled ? Colors.btnGreen : Colors.btnGray
                    radius: 12
                }

                contentItem: Text {
                    text: parent.text
                    color: Colors.textPrimary
                    font.pixelSize: 22
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                }

                onClicked: {
                    root.saveAllSettings()
                    savedMsg.visible = true
                    savedTimer.restart()
                }
            }

            // 저장 완료 메시지
            Rectangle {
                id: savedMsg
                Layout.fillWidth: true
                Layout.preferredHeight: 60
                visible: false
                color: Qt.rgba(Colors.success.r, Colors.success.g, Colors.success.b, 0.2)
                radius: 12
                border.color: Colors.success
                border.width: 2

                Text {
                    anchors.centerIn: parent
                    text: "설정이 저장되었습니다."
                    color: Colors.success
                    font.pixelSize: 18
                    font.bold: true
                }

                Timer {
                    id: savedTimer
                    interval: 3000
                    repeat: false
                    onTriggered: savedMsg.visible = false
                }
            }
        }
    }
}

// 설정 항목 컴포넌트
component SettingItem: ColumnLayout {
    id: settingItem

    property string label: ""
    property string mode: "auto"
    property alias content: contentLoader.sourceComponent
    property color borderColor: Colors.borderBlue
    property bool enabled: true

    signal modeChanged(string mode)

    spacing: 12

    RowLayout {
        Layout.fillWidth: true

        Text {
            text: settingItem.label
            color: Colors.textPrimary
            font.pixelSize: 18
            font.bold: true
        }

        Item { Layout.fillWidth: true }

        ToggleSwitch {
            value: settingItem.mode
            enabled: settingItem.enabled
            onValueChanged: settingItem.modeChanged(newValue)
        }
    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: contentLoader.height + 32
        visible: settingItem.mode === "auto"
        color: Colors.bgTertiary
        radius: 12
        border.color: settingItem.borderColor
        border.width: 3

        Loader {
            id: contentLoader
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 16
        }
    }
}
